#include "gamemapdialog.h"
#include "singleplayeroptionsdialog.h"
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
#include <QQueue>
#include <QDir>
#include <QMessageBox>
#include <QCoreApplication>

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

    // Initialize game attributes:
    gameDifficulty = medium;
    mapType = map1;
    isMultiplayer = false;
    bitcoinCount = 200; // SUBJECT TO CHANGE!
    baseRow = -1;
    baseCol = -1;
    spawnRow = 0;
    spawnCol = mapWidth-1;

    SinglePlayerOptionsDialog *singlePlayerOptions = new SinglePlayerOptionsDialog(this);
    connect(singlePlayerOptions, &SinglePlayerOptionsDialog::loadGameRequested, this, &GameMapDialog::loadGameFromFile);

    currentWave = 0;
    enemiesPerWave = 5;
    enemiesToSpawn = 0;

    updateTimer = new QTimer(this);
    connect(updateTimer, &QTimer::timeout, this, &GameMapDialog::updateGame);
    updateTimer->start(1000/frameRate);

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
        default:
            [[fallthrough]];
        case 1:
            gameDifficulty = medium;
            enemiesPerWave = 5;
            break;
        case 2:
            gameDifficulty = hard;
            enemiesPerWave = 7;
            break;
    }
}

void GameMapDialog::setMap(int map)
{
    switch (map)
    {
        default:
            [[fallthrough]];
        case 0:
            mapType = map1;
            break;
        case 1:
            mapType = map2;
            break;
        case 2:
            mapType = map3;
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

                if (tileGrid[i][j]->isBase)
                {
                    baseRow = i;
                    baseCol = j;
                    qDebug() << "Base tile located at: " << baseRow << ";" << baseCol;
                }

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

    // Add currency display:
    bitcoinText = new QGraphicsTextItem("Bitcoins: 0");
    bitcoinText->setFont(QFont("Arial", 10));
    bitcoinText->setDefaultTextColor(Qt::white);

    bitcoinBackground = new QGraphicsRectItem(0, 0, 140, 40);
    bitcoinBackground->setBrush(QBrush(QColor(0, 0, 0, 128)));
    bitcoinBackground->setPen(Qt::NoPen);

    QPixmap bitcoinPixmap(":/resources/images/bitcoin.png");
    if (bitcoinPixmap.isNull()) {
        qDebug() << "Failed to load bitcoin.png";
        bitcoinPixmap = QPixmap(32, 32);
        bitcoinPixmap.fill(Qt::yellow);
    }
    bitcoinIcon = new QGraphicsPixmapItem(bitcoinPixmap.scaled(32, 32, Qt::KeepAspectRatio));

    bitcoinGroup = new QGraphicsItemGroup();
    bitcoinGroup->addToGroup(bitcoinBackground);
    bitcoinGroup->addToGroup(bitcoinIcon);
    bitcoinGroup->addToGroup(bitcoinText);
    bitcoinGroup->setZValue(bitcoinGroup->y() + bitcoinGroup->boundingRect().width());
    gameScene->addItem(bitcoinGroup);

    bitcoinBackground->setPos(5, 5);
    bitcoinIcon->setPos(10 + 5, 8);
    bitcoinText->setPos(10 + 32 + 8, 10);

    // Build base tower and run flooding algorithm:
    if (baseRow != -1 && baseCol != -1)
        buildTower(base, baseRow, baseCol);
}

void GameMapDialog::floodFill()
{
    if (baseRow == -1 || baseCol == -1)
    {
        qDebug() << "No base specified.";
        return;
    }

    // Reset all tiles:
    for (int i=0; i<2*mapHeight; ++i)
    {
        for (int j=0; j<2*mapWidth; ++j)
        {
            if (tileGrid[i][j] != nullptr)
            {
                tileGrid[i][j]->dist = -1;
                tileGrid[i][j]->next = nullptr;
            }
        }
    }

    Tile* baseTile = tileGrid[baseRow][baseCol];
    baseTile->dist = 0; // Set destination distance to be zero.

    QQueue<Tile*> queue;
    queue.enqueue(baseTile);
    while (!queue.isEmpty())
    {
        Tile* tile = queue.dequeue();
        int row = tile->row;
        int col = tile->col;
        int dist = tile->dist;

        // Iterate over all eight adjacent tiles:
        for (int i=0; i<8; ++i)
        {
            int adjRow = row;
            int adjCol = col;
            bool addToQueue = true;
            switch (i)
            {
                // Prioritize straight moves:
                case 0: // NW:
                    --adjRow;
                    --adjCol;
                break;
                case 1: // SW:
                    ++adjRow;
                    --adjCol;
                break;
                case 2: // SE:
                    ++adjRow;
                    ++adjCol;
                break;
                case 3: // NE:
                    --adjRow;
                    ++adjCol;
                break;
                // Handle diagonal movement:
                case 4: // N:
                    adjRow -= 2;

                    // Don't allow diagonal move if a barrier is in the way:
                    if (adjRow+1 >=0 && adjRow+1 < 2*mapHeight && adjCol-1 >= 0 && adjCol-1 < 2*mapWidth)
                    {
                        Tile *tile = tileGrid[adjRow+1][adjCol-1];
                        if (tile == nullptr || tile->isBarrier || tile->hasTower)
                            addToQueue = false;
                    }

                    if (adjRow+1 >=0 && adjRow+1 < 2*mapHeight && adjCol+1 >= 0 && adjCol+1 < 2*mapWidth)
                    {
                        Tile *tile = tileGrid[adjRow+1][adjCol+1];
                        if (tile == nullptr || tile->isBarrier || tile->hasTower)
                            addToQueue = false;
                    }
                break;
                case 5: // W:
                    adjCol -= 2;

                    // Don't allow diagonal move if a barrier is in the way:
                    if (adjRow-1 >=0 && adjRow-1 < 2*mapHeight && adjCol+1 >= 0 && adjCol+1 < 2*mapWidth)
                    {
                        Tile *tile = tileGrid[adjRow-1][adjCol+1];
                        if (tile == nullptr || tile->isBarrier || tile->hasTower)
                            addToQueue = false;
                    }

                    if (adjRow+1 >=0 && adjRow+1 < 2*mapHeight && adjCol+1 >= 0 && adjCol+1 < 2*mapWidth)
                    {
                        Tile *tile = tileGrid[adjRow+1][adjCol+1];
                        if (tile == nullptr || tile->isBarrier || tile->hasTower)
                            addToQueue = false;
                    }
                break;
                case 6: // S:
                    adjRow += 2;

                    // Don't allow diagonal move if a barrier is in the way:
                    if (adjRow-1 >=0 && adjRow-1 < 2*mapHeight && adjCol-1 >= 0 && adjCol-1 < 2*mapWidth)
                    {
                        Tile *tile = tileGrid[adjRow-1][adjCol-1];
                        if (tile == nullptr || tile->isBarrier || tile->hasTower)
                            addToQueue = false;
                    }

                    if (adjRow-1 >=0 && adjRow-1 < 2*mapHeight && adjCol+1 >= 0 && adjCol+1 < 2*mapWidth)
                    {
                        Tile *tile = tileGrid[adjRow-1][adjCol+1];
                        if (tile == nullptr || tile->isBarrier || tile->hasTower)
                            addToQueue = false;
                    }
                break;
                case 7: // E:
                    adjCol += 2;

                    // Don't allow diagonal move if a barrier is in the way:
                    if (adjRow-1 >=0 && adjRow-1 < 2*mapHeight && adjCol-1 >= 0 && adjCol-1 < 2*mapWidth)
                    {
                        Tile *tile = tileGrid[adjRow-1][adjCol-1];
                        if (tile == nullptr || tile->isBarrier || tile->hasTower)
                            addToQueue = false;
                    }

                    if (adjRow+1 >=0 && adjRow+1 < 2*mapHeight && adjCol-1 >= 0 && adjCol-1 < 2*mapWidth)
                    {
                        Tile *tile = tileGrid[adjRow+1][adjCol-1];
                        if (tile == nullptr || tile->isBarrier || tile->hasTower)
                            addToQueue = false;
                    }
                break;
            }

            // Check whether the adjacent tile is valid and can be visited from the current node:
            if (adjRow < 0 || adjRow >= 2*mapHeight || adjCol < 0 || adjCol >= 2*mapWidth)
                addToQueue = false; // Break if the index is out of bounds.
            else if (tileGrid[adjRow][adjCol] == nullptr || tileGrid[adjRow][adjCol]->isBarrier || tileGrid[adjRow][adjCol]->hasTower)
                addToQueue = false; // Break if tile is inaccessible by enemies.
            else if (tileGrid[adjRow][adjCol]->dist != -1)
                addToQueue = false; // Break if tile has already been visited.

            if (addToQueue)
            {
                tileGrid[adjRow][adjCol]->dist = dist+1; // Update distance to destination.
                tileGrid[adjRow][adjCol]->next = tile; // Store pointer to next tile.
                queue.enqueue(tileGrid[adjRow][adjCol]); // Add adjacent tile to queue.
            }
        }
    }

    // Ensure that enemies can always reach the base:
    Tile *tile = tileGrid[spawnRow][spawnCol];
    if (tile == nullptr)
    {
        qDebug() << "Invalid spawn point chosen.";
        return;
    }
    else
    {
        while (tile->next != nullptr)
        {
            tile = tile->next;
        }
        if (!tile->isBase && !towers.isEmpty())
        {
            qDebug() << "Base has been closed off. Selling last added tower.";
            Tile *tile = towers.last()->tile;
            sellTower(tile->row, tile->col);
        }
    }

    qDebug() << "Game map flooded.";

    // TEST ENEMY IMPLEMENTATION:
    if (!enemies.isEmpty())
    {
        for (Enemy* &enemy : enemies)
        {
            enemy->setState(Dying);
        }
    }

    Enemy* enemy1 = new Enemy(Wizard);
    Tile *spawnTile = tileGrid[spawnRow][spawnCol];
    spawnTile->addEnemy(enemy1);
    connect(enemy1, &Enemy::killEnemy, this, &GameMapDialog::killEnemy);
    enemies.append(enemy1);
    gameScene->addItem(enemy1);

    Enemy* enemy2 = new Enemy(Orcastor);
    spawnTile->addEnemy(enemy2);
    connect(enemy2, &Enemy::killEnemy, this, &GameMapDialog::killEnemy);
    enemies.append(enemy2);
    gameScene->addItem(enemy2);
}

void GameMapDialog::updateGame()
{
    QVector<Enemy*> enemiesToRemove;
    for (Enemy* &enemy : enemies)
    {
        enemy->Tick();
        if (enemy->getState() == Dying)
        {
            if (!enemy->isJustLoaded()) {
                bitcoinCount += enemy->getBitcoinReward();
            }
            enemiesToRemove.append(enemy);
            gameScene->removeItem(enemy);
        }
        else if (enemy->isJustLoaded()) {
            enemy->setJustLoaded(false);  // Clear flag after first update
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
        connect(tower, &Tower::destroyTower, this, &GameMapDialog::destroyTower);

        bitcoinCount -= tower->getCost(); // Pay amount for tower.

        floodFill(); // Recalculate shortest paths.
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
        bitcoinCount += tower->getCost(); // Receive back amount paid for tower.
        towers.removeOne(tower);
        tower->deleteLater();

        floodFill(); // Recalculate shortest paths.
    }
}

void GameMapDialog::upgradeTower(int row, int col) // Upgrade tower at tile that sent the signal.
{
    Tower *tower = tileGrid[row][col]->tower;
    bitcoinCount = tower->Upgrade(bitcoinCount);
}

void GameMapDialog::destroyTower(int row, int col)
{
    Tower *tower = tileGrid[row][col]->removeTower();
    if (tower != nullptr)
    {
        towers.removeOne(tower);
        tower->deleteLater();
        floodFill(); // Recalculate shortest paths.
    }
}

void GameMapDialog::killEnemy(Enemy *e)
{
    enemies.removeOne(e);
    e->deleteLater();
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
    updateTimer->start();
    pauseMenu = nullptr;
}

void GameMapDialog::onResumeGame()
{
    resumeGame();
}

void GameMapDialog::onSaveGame()
{
    QString filename = "savegame.txt"; // You can modify this to allow for different filenames if needed
    if (saveGameToFile(filename)) {
        qDebug() << "Game saved successfully.";
    } else {
        qDebug() << "Failed to save game.";
    }
    resumeGame();
}


bool GameMapDialog::saveGameToFile(const QString& filename)
{
    savedBitcoinCount = bitcoinCount; // Save current bitcoin count
    QString saveDirPath = QCoreApplication::applicationDirPath() + "/saves";
    QDir saveDir(saveDirPath);
    if (!saveDir.exists()) {
        saveDir.mkpath(".");
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
    out << "BitcoinCount: " << bitcoinCount << "\n";
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


void GameMapDialog::onExitGame()
{
    close();
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

    // Display the file content in a message box
    QMessageBox::information(this, "Loaded Game Content", fileContent);

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
   // updateBitcoinDisplay(); // Update UI immediately

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
            Enemy* enemy = new Enemy(etype);
            enemy->setPos(x, y);
            enemy->setHealth(health > 0 ? health : 1);
            enemy->setJustLoaded(true);
            enemies.append(enemy);
            gameScene->addItem(enemy);
            qDebug() << "Loaded enemy type" << etype << "at" << x << y << "health" << health;
        } else {
            qDebug() << "Invalid enemy position loaded at" << x << y;
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
            tower->towerLevel = level; // Ensure tower level is set correctly
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
    bitcoinCount = savedBitcoinCount; // Ensure bitcoin count is set correctly
    updateBitcoinDisplay(); // Update UI immediately

    file.close();
    return true;
}
