#include "gamemapdialog.h"
#include <QDebug>
#include <ctime>
#include <QByteArray>
#include <QTimer>
#include <QFont>
#include <QSet>
#include <limits>
#include <QPair>
#include <vector>
#include <cmath>
#include <QSound>
#include <queue>
#include "tower.h"
#include <QFile>
#include <QCoreApplication>
#include <QDir>


GameMapDialog::GameMapDialog(QWidget *parent)
    : QDialog(parent)
{
        setModal(true);
        showMaximized();
        setFixedSize(1920,1080);
        setWindowTitle("Dungeons & Towers");

        gameScene = new QGraphicsScene(this);
        gameScene->setSceneRect(0, 0, tileSize*mapWidth, tileSize/2*mapHeight+32);
        gameScene->setItemIndexMethod(QGraphicsScene::NoIndex);
        gameScene->setBackgroundBrush(Qt::black);

        gameView = new QGraphicsView(gameScene, this);
        gameView->setFixedSize(1920, 1080);
        gameView->setRenderHint(QPainter::Antialiasing);
        gameView->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        gameView->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        int scale = 1920/(tileSize*mapWidth);
        gameView->scale(scale, scale);
        gameView->centerOn(1920/2, 0);
        gameView->show();

        gameDifficulty = medium;
        mapType = map1;
        isMultiplayer = false;
        bitcoinCount = 200; // SUBJECT TO CHANGE!
        tileset = new QPixmap(":/resources/images/tileset.png");

            bitcoinText = new QGraphicsTextItem(QString("Bitcoins: %1").arg(bitcoinCount));
            bitcoinText->setFont(QFont("Arial", 10));
            bitcoinText->setDefaultTextColor(Qt::white);

            bitcoinBackground = new QGraphicsRectItem(0, 0, 140, 40);
            bitcoinBackground->setBrush(QBrush(QColor(0, 0, 0, 128)));
            bitcoinBackground->setPen(Qt::NoPen);

            QPixmap bitcoinPixmap(":/resources/images/bitcoin.png");
            if (bitcoinPixmap.isNull()) {
                bitcoinPixmap = QPixmap(32, 32);
                bitcoinPixmap.fill(Qt::yellow);
            }
            bitcoinIcon = new QGraphicsPixmapItem(bitcoinPixmap.scaled(32, 32, Qt::KeepAspectRatio));

            bitcoinGroup = new QGraphicsItemGroup();
            bitcoinGroup->addToGroup(bitcoinBackground);
            bitcoinGroup->addToGroup(bitcoinIcon);
            bitcoinGroup->addToGroup(bitcoinText);
            bitcoinGroup->setZValue(1000);

            gameScene->addItem(bitcoinGroup);

            bitcoinBackground->setPos(5, 5);
            bitcoinIcon->setPos(10 + 5, 8);
            bitcoinText->setPos(10 + 32 + 8, 10);

//        QGraphicsRectItem* debugRect = gameScene->addRect(0, 0, tileSize*mapWidth, tileSize/2*mapHeight, QPen(Qt::red));
//        debugRect->setZValue(10);

        QVector<QPointF> spawnPoints = getSpawnPoints();
        for (const QPointF& point : spawnPoints) {
            QGraphicsRectItem* spawnMarker = gameScene->addRect(point.x() - 16, point.y() - 16, 32, 32, QPen(Qt::yellow), QBrush(Qt::yellow));
            spawnMarker->setZValue(9);
        }

        currentWave = 0;
        enemiesPerWave = 5;
        enemiesToSpawn = 0;
        waveTimer = new QTimer(this);
        connect(waveTimer, &QTimer::timeout, this, &GameMapDialog::startNextWave);
        waveTimer->start(10000);

        updateTimer = new QTimer(this);
        connect(updateTimer, &QTimer::timeout, this, &GameMapDialog::updateGame);
        updateTimer->start(125);

        pauseMenu = nullptr;
}

void GameMapDialog::setDifficulty(int dif)
{
    switch (dif)
    {
        case 0:
            gameDifficulty = easy;
            enemiesPerWave = 3;
            break;
        case 1:
            gameDifficulty = medium;
            enemiesPerWave = 5;
            break;
        case 2:
            gameDifficulty = hard;
            enemiesPerWave = 7;
            break;
        default:
            gameDifficulty = medium;
            enemiesPerWave = 5;
            break;
    }
}

void GameMapDialog::setMap(int map)
{
    switch (map)
    {
        case 0:
            mapType = map1;
            break;
        case 1:
            mapType = map2;
            break;
        case 2:
            mapType = map3;
            break;
        default:
            mapType = map1;
            break;
    }
}

void GameMapDialog::setMultiplayer(bool multi)
{
    isMultiplayer = multi;
}

int GameMapDialog::getDifficulty()
{
    return (int)gameDifficulty;
}

int GameMapDialog::getMap()
{
    return (int)mapType;
}

bool GameMapDialog::getMultiplayer()
{
    return isMultiplayer;
}

void GameMapDialog::drawMap()
{
    mapFile = new QFile(this);
    switch (mapType)
    {
        case map1:
            qDebug() << "Loaded Map 1.";
            mapFile->setFileName(":/resources/maps/map1.txt");
            break;
        case map2:
            qDebug() << "Loaded Map 2.";
            mapFile->setFileName(":/resources/maps/map2.txt");
            break;
        case map3:
            qDebug() << "Loaded Map 3.";
            mapFile->setFileName(":/resources/maps/map3.txt");
            break;
    }
    if (!mapFile->open(QIODevice::ReadOnly))
        qDebug() << "Cannot load map file.";

    int rows = 0;
    int columns = 0;
    while (!mapFile->atEnd())
    {
        columns = 0;
        QByteArray line = mapFile->readLine();
        int p = -1;
        do
        {
            p = line.indexOf("\t");
            int tile = line.mid(0, p).toInt();
            line = line.mid(p+1);

            // Populate mapGrid:
            if ((rows%2 == 0 && columns%2 == 0) || (rows%2 != 0 && columns%2 != 0))
            {
                mapGrid[rows][columns] = (tile < 1) ? 1 : tile;
            }
            else
                mapGrid[rows][columns] = 0;

            // Populate barrierGrid:
            if ((rows%2 == 0 && columns%2 != 0) || (rows%2 != 0 && columns%2 == 0))
                barrierGrid[2*mapHeight - rows - 1][columns] = tile;
            else
                barrierGrid[2*mapHeight - rows - 1][columns] = 0;

            ++columns;
        }
        while (p != -1);
        ++rows;
    }
    mapFile->close();
    qDebug() << "Extracted map from text file.";
    qDebug() << "Rows: " << rows << " Columns: " << columns;

    srand(time(0));
    for (int i = 0; i<rows; ++i)
    {
        for (int j = 0; j<columns; ++j)
        {
            if (mapGrid[i][j] != 0)
            {
                tileGrid[i][j] = new Tile(mapGrid[i][j], barrierGrid[i][j], i, j);
                gameScene->addItem(tileGrid[i][j]);

                // Connect relevant signals to slots:
                connect(tileGrid[i][j], &Tile::buildTower, this, &GameMapDialog::buildTower);
                connect(tileGrid[i][j], &Tile::sellTower, this, &GameMapDialog::sellTower);
                connect(tileGrid[i][j], &Tile::upgradeTower, this, &GameMapDialog::upgradeTower);
            }
            else
            {
                tileGrid[i][j] = nullptr; // Corner indices are null pointers.
            }
        }
    }
    qDebug() << "Added tiles and barriers to GraphicsView.";
}

void GameMapDialog::updateGame()
{
    QVector<Enemy*> enemiesToRemove;
    for (Enemy* &enemy : enemies) {
        if (!enemy->isAlive()) {
            if (!enemy->isJustLoaded()) {
                bitcoinCount += enemy->getBitcoinReward();
            }
            enemiesToRemove.append(enemy);
            gameScene->removeItem(enemy);
        } else if (enemy->isJustLoaded()) {
            enemy->setJustLoaded(false);  // Reset flag so future deaths award bitcoins
        } else {
            enemy->update();
        }
    }
    for (Enemy* &enemy : enemiesToRemove) {
        enemies.removeOne(enemy);
        delete enemy;
    }
    updateBitcoinDisplay();
    gameScene->update();

    for (Tower* &tower : towers)
    {
        tower->Tick();
    }
}


void GameMapDialog::updateBitcoinDisplay()
{
    bitcoinText->setPlainText(QString("Bitcoins: %1").arg(bitcoinCount));
    qreal bgWidth = bitcoinText->boundingRect().width() + 32 + 12;
    qreal bgHeight = qMax(bitcoinText->boundingRect().height(), 32.0) + 8;
    bitcoinBackground->setRect(0, 0, bgWidth, bgHeight);
}


void GameMapDialog::buildTower(towerType type, int row, int col)
{
    Tower *tower = new Tower(type);
    if (bitcoinCount - tower->getCost() >= 0)
    {
        tileGrid[row][col]->addTower(tower);
        gameScene->addItem(tower);

        towers.append(tower);

        bitcoinCount -= tower->getCost(); // Pay amount for tower.
    }
    else
    {
        tower->deleteLater();
    }
}

void GameMapDialog::sellTower(int row, int col) // Sell tower at tile that sent the signal.
{
    Tower *tower = tileGrid[row][col]->removeTower();
    if (tower != nullptr)
    {
        bitcoinCount += tower->getCost(); // Receive back amount payed for tower.
        towers.removeOne(tower);
        tower->deleteLater();
    }
}

void GameMapDialog::upgradeTower(int row, int col) // Upgrade tower at tile that sent the signal.
{
    Tower *tower = tileGrid[row][col]->tower;
    bitcoinCount = tower->Upgrade(bitcoinCount);

}

void GameMapDialog::spawnEnemy(EnemyType type, const QPointF& pos)
{
    // Force spawn point to be walkable
    int gridX = static_cast<int>(pos.x() / tileSize);
    int gridY = static_cast<int>(pos.y() / (tileSize / 2));
    if (gridX >= 0 && gridX < 2 * mapWidth && gridY >= 0 && gridY < 2 * mapHeight) {
        mapGrid[gridY][gridX] = 1; // Grass tile
        barrierGrid[2 * mapHeight - gridY - 1][gridX] = 0; // No barrier
        qDebug() << "Forced spawn point" << pos << "to grass at grid (" << gridX << "," << gridY << ")";
    } else {
        qDebug() << "Error: Spawn point" << pos << "outside bounds";
    }

    Enemy* enemy = new Enemy(type, pos, this);
    if (enemy->pixmap().isNull()) {
        qDebug() << "Skipping spawn of type" << type << "due to null pixmap at" << pos;
        delete enemy;
        return;
    }
    switch (gameDifficulty) {
        case easy:
            enemy->setHealth(enemy->getHealth() * 0.8);
            break;
        case medium:
            break;
        case hard:
            enemy->setHealth(enemy->getHealth() * 1.5);
            enemy->setDamage(enemy->getDamage() * 1.2);
            break;
    }
    enemy->setZValue(0);
    enemies.append(enemy);
    gameScene->addItem(enemy);
    qDebug() << "Spawned enemy type" << type << "at" << pos;


}

QVector<QPointF> GameMapDialog::getSpawnPoints()
{
    QVector<QPointF> spawnPoints;
    spawnPoints << QPointF(80, 65) << QPointF(320, 115);
    for (const QPointF& point : spawnPoints) {
        int gridX = static_cast<int>(point.x() / tileSize);
        int gridY = static_cast<int>(point.y() / (tileSize / 2));
        if (gridX >= 0 && gridX < 2 * mapWidth && gridY >= 0 && gridY < 2 * mapHeight) {
            qDebug() << "Spawn point" << point << "grid (" << gridX << "," << gridY << ")";
        } else {
            qDebug() << "Error: Spawn point" << point << "outside bounds";
        }
    }
    return spawnPoints;
}

void GameMapDialog::startNextWave()
{
    currentWave++;
    enemiesToSpawn = enemiesPerWave + currentWave * 2;
    qDebug() << "Starting wave" << currentWave << "with" << enemiesToSpawn << "enemies";

    QTimer* spawnTimer = new QTimer(this);
    int spawnInterval = 1000;
    int enemiesSpawned = 0;

    connect(spawnTimer, &QTimer::timeout, this, [=]() mutable {
        if (enemiesSpawned < enemiesToSpawn) {
            QVector<QPointF> spawnPoints = getSpawnPoints();
            if (!spawnPoints.isEmpty()) {
                QPointF spawnPos = spawnPoints[rand() % spawnPoints.size()];
                EnemyType type = (currentWave < 3) ? Skeleton :
                                 (currentWave < 6) ? Orc : Knight;
                spawnEnemy(type, spawnPos);
                enemiesSpawned++;
            }
        } else {
            spawnTimer->stop();
            spawnTimer->deleteLater();
            qDebug() << "Wave" << currentWave << "completed";
        }
    });
    spawnTimer->start(spawnInterval);
}

QVector<QPointF> GameMapDialog::findPath(const QPointF& start, const QPointF& target)
{
    int startX = static_cast<int>(std::round(start.x() / tileSize));
    int startY = static_cast<int>(std::round(start.y() / (tileSize / 2)));
    int targetX = static_cast<int>(target.x() / tileSize);
    int targetY = static_cast<int>(target.y() / (tileSize / 2));

    startX = qBound(0, startX, 2 * mapWidth - 1);
    startY = qBound(0, startY, 2 * mapHeight - 1);
    targetX = qBound(0, targetX, 2 * mapWidth - 1);
    targetY = qBound(0, targetY, 2 * mapHeight - 1);

    QVector<QVector<int>> distances(2 * mapHeight, QVector<int>(2 * mapWidth, std::numeric_limits<int>::max()));
    QVector<QVector<QPoint>> previous(2 * mapHeight, QVector<QPoint>(2 * mapWidth, QPoint(-1, -1)));
    QVector<QPoint> unvisited;

    for (int y = 0; y < 2 * mapHeight; ++y)
        for (int x = 0; x < 2 * mapWidth; ++x)
            unvisited.append(QPoint(x, y));

    distances[startY][startX] = 0;

    while (!unvisited.isEmpty()) {
        int minDist = std::numeric_limits<int>::max();
        int minIndex = -1;
        for (int i = 0; i < unvisited.size(); ++i) {
            QPoint node = unvisited[i];
            int dist = distances[node.y()][node.x()];
            if (dist < minDist) {
                minDist = dist;
                minIndex = i;
            }
        }
        if (minIndex == -1)
            break;

        QPoint current = unvisited[minIndex];
        unvisited.remove(minIndex);

        int currX = current.x();
        int currY = current.y();

        if (currX == targetX && currY == targetY)
            break;

        QVector<QPoint> neighbors = {QPoint(currX, currY - 1), QPoint(currX, currY + 1), QPoint(currX - 1, currY), QPoint(currX + 1, currY)};
        for (const QPoint& neighbor : neighbors) {
            int nx = neighbor.x();
            int ny = neighbor.y();
            if (nx < 0 || nx >= 2 * mapWidth || ny < 0 || ny >= 2 * mapHeight)
                continue;

            int barrierY = 2 * mapHeight - ny - 1;
            if (barrierGrid[barrierY][nx] != 0)
                continue;

            if (!unvisited.contains(neighbor))
                continue;

            int altDist = distances[currY][currX] + 1;
            if (altDist < distances[ny][nx]) {
                distances[ny][nx] = altDist;
                previous[ny][nx] = current;
            }
        }
    }

    QVector<QPointF> path;
    QPoint step(targetX, targetY);

    if (distances[targetY][targetX] == std::numeric_limits<int>::max()) {
        return path;
    }

    QRectF sceneRect = gameScene->sceneRect();
    const qreal maxX = 450.0;

    while (step.x() != -1 && step.y() != -1 && !(step.x() == startX && step.y() == startY)) {
        qreal px = step.x() * tileSize + tileSize / 2;
        qreal py = step.y() * (tileSize / 2) + tileSize / 4;
        px = qBound(sceneRect.left(), px, qMin(sceneRect.right(), maxX));
        py = qBound(sceneRect.top(), py, sceneRect.bottom());
        path.prepend(QPointF(px, py));
        step = previous[step.y()][step.x()];
    }

    qreal startPx = startX * tileSize + tileSize / 2;
    qreal startPy = startY * (tileSize / 2) + tileSize / 4;
    startPx = qBound(sceneRect.left(), startPx, qMin(sceneRect.right(), maxX));
    startPy = qBound(sceneRect.top(), startPy, sceneRect.bottom());
    path.prepend(QPointF(startPx, startPy));

    for (int i = 0; i < path.size() - 1; ++i) {
        QGraphicsLineItem* line = gameScene->addLine(path[i].x(), path[i].y(), path[i + 1].x(), path[i + 1].y(), QPen(Qt::green, 2));
        line->setZValue(8);
    }

    return path;
}

void GameMapDialog::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape && !pauseMenu) {
        pauseGame();
    }
}

void GameMapDialog::pauseGame()
{
    if (!pauseMenu) {
        waveTimer->stop();
        updateTimer->stop();
        pauseMenu = new PauseMenuDialog(this);
        connect(pauseMenu, &PauseMenuDialog::resumeGame, this, &GameMapDialog::onResumeGame);
        connect(pauseMenu, &PauseMenuDialog::saveGame, this, &GameMapDialog::onSaveGame);
        connect(pauseMenu, &PauseMenuDialog::exitGame, this, &GameMapDialog::onExitGame);
        pauseMenu->exec();
    }
}

void GameMapDialog::resumeGame()
{
    waveTimer->start();
    updateTimer->start();
    pauseMenu = nullptr;
}

void GameMapDialog::onResumeGame()
{
    resumeGame();
}

void GameMapDialog::onSaveGame()
{
    qDebug() << "Saving game...";
    if (saveGameToFile("savegame.txt")) {
        QFile file("savegame.txt");
        qDebug() << "Saving to:" << file.fileName();
        qDebug() << "Game saved successfully.";
    } else {
        qDebug() << "Failed to save game.";
    }
    resumeGame();
}

void GameMapDialog::onExitGame()
{
    // Stop all gameplay timers to halt spawning and updating
    waveTimer->stop();
    updateTimer->stop();


    clearGameState();

    qDebug() << "Exiting game: All timers stopped, game processes halted.";

    close();
}


GameMapDialog::~GameMapDialog()
{
    gameScene->clear();
    delete gameScene;
    delete gameView;
}

void GameMapDialog::clearGameState()
{
    // Remove all enemies from scene and clear vector
    for (Enemy* enemy : qAsConst(enemies)) {
        gameScene->removeItem(enemy);
        delete enemy;
    }
    enemies.clear();

    // Remove all towers from scene and clear vector
    for (Tower* tower : qAsConst(towers)) {
        gameScene->removeItem(tower);
        delete tower;
    }
    towers.clear();

    // Clear tileGrid
    for (int i = 0; i < 2 * mapHeight; ++i) {
        for (int j = 0; j < 2 * mapWidth; ++j) {
            if (tileGrid[i][j]) {
                gameScene->removeItem(tileGrid[i][j]);
                delete tileGrid[i][j];
                tileGrid[i][j] = nullptr;
            }
        }
    }
}

bool GameMapDialog::saveGameToFile(const QString& filename)
{

    savedBitcoinCount = bitcoinCount;
    // Construct path relative to application directory
    QString saveDirPath = QCoreApplication::applicationDirPath() + "/saves";
    QDir saveDir(saveDirPath);
    if (!saveDir.exists()) {
        saveDir.mkpath("."); // Create saves directory if it doesn't exist
    }
    QString filePath = saveDirPath + "/" + filename;

    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qDebug() << "Cannot open file for saving:" << filePath;
        return false;
    }
    QTextStream out(&file);

    out << "MapType: " << (int)mapType << "\n";
    out << "Difficulty: " << (int)gameDifficulty << "\n";
    out << "BitcoinCount: " << savedBitcoinCount << "\n";
    out << "CurrentWave: " << currentWave << "\n";

    out << "\nEnemies:\n";
    for (Enemy* enemy : qAsConst(enemies)) {
        out << (int)enemy->getType() << " "
            << enemy->pos().x() << " "
            << enemy->pos().y() << " "
            << enemy->getHealth() << "\n";
    }

    out << "\nTowers:\n";
    for (Tower* tower : qAsConst(towers)) {
        int row = 0;
        int col = 0;
        bool found = false;
        for (int r = 0; r < 2*mapHeight && !found; ++r) {
            for (int c = 0; c < 2*mapWidth && !found; ++c) {
                if (tileGrid[r][c] && tileGrid[r][c]->tower == tower) {
                    row = r;
                    col = c;
                    found = true;
                }
            }
        }
        out << (int)tower->type << " " << row << " " << col << " " << tower->towerLevel << "\n";
    }

    file.close();
    qDebug() << "Game saved successfully to:" << filePath;
    return true;
}

bool GameMapDialog::loadGameFromFile(const QString& filename)
{
    QString filePath = QCoreApplication::applicationDirPath() + "/saves/" + filename;
    qDebug() << "File Path:" << filePath;
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Cannot open file for loading:" << filePath;
        return false;
    }

    // Read the entire file into a QString
    QString fileContent = file.readAll();
    qDebug() << "Loaded file content:\n" << fileContent; // Debugging line

    // Split the content into lines
    QStringList lines = fileContent.split('\n', QString::SkipEmptyParts);

    // Read header lines for map/difficulty/bitcoin/wave:
    for (const QString& line : qAsConst(lines)) {
        qDebug() << "Read line:" << line; // Debugging line
        if (line.startsWith("MapType:")) {
            bool ok;
            int mt = line.section(':', 1).trimmed().toInt(&ok);
            if (ok) {
                mapType = static_cast<map>(mt);
                qDebug() << "Loaded mapType:" << mapType;
            } else {
                qDebug() << "Invalid MapType format in line:" << line;
            }
        } else if (line.startsWith("Difficulty:")) {
            bool ok;
            int diff = line.section(':', 1).trimmed().toInt(&ok);
            if (ok) {
                gameDifficulty = static_cast<difficulty>(diff);
                setDifficulty(diff); // Update enemiesPerWave
                qDebug() << "Loaded difficulty:" << gameDifficulty;
            } else {
                qDebug() << "Invalid Difficulty format in line:" << line;
            }
        } else if (line.startsWith("BitcoinCount:")) {
                bool ok = true;
                int count = line.section(':', 1).trimmed().toInt(&ok);
                if (ok) {
                    bitcoinCount = count; // Set bitcoinCount directly from loaded value
                    savedBitcoinCount = bitcoinCount; // Synchronize saved count
                    qDebug() << "Loaded bitcoinCount:" << bitcoinCount;
                } else {
                    qDebug() << "Invalid BitcoinCount format in line:" << line;
                    bitcoinCount = 200; // Default value
                    savedBitcoinCount = 200; // Synchronize saved count
                }
        } else if (line.startsWith("CurrentWave:")) {
            bool ok;
            currentWave = line.section(':', 1).trimmed().toInt(&ok);
            if (ok) {
                qDebug() << "Loaded currentWave:" << currentWave;
            } else {
                qDebug() << "Invalid CurrentWave format in line:" << line;
            }
        } else if (line.startsWith("Enemies:")) {
            break; // Proceed to enemies section
        }
    }

    // Draw map using loaded mapType
    drawMap();


    // Read enemies:
    for (int i = 0; i < lines.size(); ++i) {
        QString line = lines[i].trimmed();
        qDebug() << "Enemy line:" << line; // Debugging line
        if (line.isEmpty()) continue; // Skip empty lines
        QStringList parts = line.split(' ', QString::SkipEmptyParts);

        if (parts.size() < 4) {
            qDebug() << "Invalid enemy line format:" << line;
            continue;
        }

        bool ok;
        EnemyType etype = static_cast<EnemyType>(parts[0].toInt(&ok));
        if (!ok) continue;
        float x = parts[1].toFloat(&ok);
        if (!ok) continue;
        float y = parts[2].toFloat(&ok);
        if (!ok) continue;
        int health = parts[3].toInt(&ok);
        if (!ok) continue;

        int gridX = static_cast<int>(x / tileSize);
        int gridY = static_cast<int>(y / (tileSize / 2));
        if (gridX >= 0 && gridX < 2 * mapWidth && gridY >= 0 && gridY < 2 * mapHeight && mapGrid[gridY][gridX] != 0) {
            Enemy* enemy = new Enemy(etype, QPointF(x, y), this);
            enemy->setHealth(health > 0 ? health : 1);
            enemy->setJustLoaded(true);
            enemies.append(enemy);
            gameScene->addItem(enemy);
            qDebug() << "Loaded enemy type" << etype << "at" << x << y << "health" << health;
        } else {
            qDebug() << "Invalid enemy position loaded at" << x << y;
        }
    }

    // Recompute paths for loaded enemies
    QPointF target(450, tileSize / 2 * mapHeight);
    for (Enemy* enemy : qAsConst(enemies)) {
        QVector<QPointF> path = findPath(enemy->pos(), target);
        if (!path.isEmpty()) {
            enemy->setPath(path);
        } else {
            qDebug() << "No valid path found for enemy at" << enemy->pos();
        }
    }

    // Read towers:
    for (int i = 0; i < lines.size(); ++i) {
        QString line = lines[i].trimmed();
        qDebug() << "Tower line:" << line; // Debugging line
        if (line.isEmpty()) continue;
        QStringList parts = line.split(' ', QString::SkipEmptyParts);
        if (parts.size() < 4) {
            qDebug() << "Invalid tower line format:" << line;
            continue;
        }

        bool ok;
        towerType ttype = static_cast<towerType>(parts[0].toInt(&ok));
        if (!ok) continue;
        int row = parts[1].toInt(&ok);
        if (!ok) continue;
        int col = parts[2].toInt(&ok);
        if (!ok) continue;
        int level = parts[3].toInt(&ok);
        if (!ok) continue;

        if (row >= 0 && row < 2*mapHeight && col >= 0 && col < 2*mapWidth && tileGrid[row][col] && mapGrid[row][col] != 0) {
            Tower* tower = new Tower(ttype);
            tower->towerLevel = level;
            tower->setZValue(1);
            towers.append(tower);
            tileGrid[row][col]->addTower(tower);
            gameScene->addItem(tower);
            qDebug() << "Loaded tower type" << ttype << "at" << row << col << "level" << level;
        } else {
            qDebug() << "Invalid tower position loaded at row" << row << "col" << col;
        }
    }

    // Restart timers
    waveTimer->start(10000);
    updateTimer->start(125);
    bitcoinCount = savedBitcoinCount;
    updateBitcoinDisplay(); // Update UI immediately

    file.close();
    return true;
}
