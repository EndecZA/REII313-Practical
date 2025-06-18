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
#include <QThread>
#include <QCoreApplication>

GameMapDialog::GameMapDialog(QWidget *parent)
    : QDialog(parent)
{
    setModal(true);
    showMaximized();
    setFixedSize(1920,1080);
    setWindowTitle("Dungeons & Towers");

    gameScene = new QGraphicsScene(this);
    gameScene->setSceneRect(0, 0, tileSize*(mapWidth+0.5), tileSize/2*mapHeight+tileSize);
    gameScene->setItemIndexMethod(QGraphicsScene::NoIndex);
    gameScene->setBackgroundBrush(Qt::black);
    gameView = new QGraphicsView(gameScene, this);
    gameView->setFixedSize(1920, 1080);
    gameView->centerOn(0, 0);
    gameView->setRenderHint(QPainter::Antialiasing);
    gameView->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    gameView->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    float scale = 1920.0/(tileSize*(mapWidth+0.6));
    gameView->scale(scale, scale);
    gameView->show();

    // Initialize game attributes:
    gameDifficulty = medium;
    mapType = map1;
    isMultiplayer = false;
    bitcoinCount = 200; // SUBJECT TO CHANGE!
    baseRow = -1;
    baseCol = -1;

    currentWave = 0;
    totalEnemiesPerWave = 5; // Set the number of enemies per wave
    waveTimer = new QTimer(this);
    connect(waveTimer, &QTimer::timeout, this, &GameMapDialog::spawnWave);

    gameTick = new QTimer(this);
    connect(gameTick, &QTimer::timeout, this, &GameMapDialog::updateGame);

    enemyTick = new QTimer(this);
    connect(enemyTick, &QTimer::timeout, this, &GameMapDialog::tickEnemies);

    pauseMenu = nullptr;

}

void GameMapDialog::setDifficulty(int dif)
{
    switch (dif)
    {
        case 0:
            gameDifficulty = easy;
            totalEnemiesPerWave = 3;
            break;
        default:
            [[fallthrough]];
        case 1:
            gameDifficulty = medium;
            totalEnemiesPerWave = 5;
            break;
        case 2:
            gameDifficulty = hard;
            totalEnemiesPerWave = 7;
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
        default:
            [[fallthrough]];
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
                if (mapGrid[i][j] == 7)
                {
                    tileGrid[i][j] = new Tile(7, 0, i, j); // Force tile to not be a barrier.
                    spawnPoints.enqueue(tileGrid[i][j]);
                    qDebug() << "Spawn tile located at: " << i << ";" << j;
                }
                else
                    tileGrid[i][j] = new Tile(mapGrid[i][j], barrierGrid[i][j], i, j); // Add tile normally.

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
                connect(tileGrid[i][j], &Tile::attackAnimation, this, &GameMapDialog::attackAnimation);
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

    waveText = new QGraphicsTextItem("Wave: 0");
    waveText->setFont(QFont("Arial", 10));
    waveText->setDefaultTextColor(Qt::white);
    waveText->setPos(5, 50);


    bitcoinGroup = new QGraphicsItemGroup();
    bitcoinGroup->addToGroup(bitcoinBackground);
    bitcoinGroup->addToGroup(bitcoinIcon);
    bitcoinGroup->addToGroup(bitcoinText);
    bitcoinGroup->addToGroup(waveText);
    bitcoinGroup->setZValue(bitcoinGroup->y() + bitcoinGroup->boundingRect().width());
    gameScene->addItem(bitcoinGroup);

    bitcoinBackground->setPos(5, 5);
    bitcoinIcon->setPos(10 + 5, 8);
    bitcoinText->setPos(10 + 32 + 8, 10);

    // Start game if the base and spawnpoints were read successfully:
    if (baseRow != -1 && baseCol != -1 && !spawnPoints.isEmpty()) // Map read successfully.
    {
        buildTower(base, baseRow, baseCol); // Build the base tower and run flooding algorithm.

        // Only start game if a valid map was read:
        gameTick->start(1000/frameRate);
        waveTimer->start(10000);
        // Modify enemy walking speed and attack rate based on difficulty:
        switch (gameDifficulty)
        {
        case easy:
            enemyTick->start(1000/(frameRate-2));
            break;
        default:
            [[fallthrough]];
        case medium:
            enemyTick->start(1000/frameRate);
            break;
        case hard:
            enemyTick->start(1000/(frameRate+2));
            break;
        }
    }
    else
    {
        qDebug() << "No spawn points specified or base position has been omitted.";
    }

}

void GameMapDialog::floodFill()
{
    if (baseRow == -1 || baseCol == -1)
    {
        qDebug() << "No base specified.";
        return;
    }

    // Reset all tiles:
    for (int i = 0; i < 2 * mapHeight; ++i)
    {
        for (int j = 0; j < 2 * mapWidth; ++j)
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
        for (int i = 0; i < 8; ++i)
        {
            int adjRow = row;
            int adjCol = col;
            bool addToQueue = true;
            switch (i)
            {
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
                case 4: // N:
                    adjRow -= 2;
                    if (adjRow + 1 >= 0 && adjRow + 1 < 2 * mapHeight && adjCol - 1 >= 0 && adjCol - 1 < 2 * mapWidth)
                    {
                        Tile* t = tileGrid[adjRow + 1][adjCol - 1];
                        if (t == nullptr || t->isBarrier || t->hasTower)
                            addToQueue = false;
                    }
                    if (adjRow + 1 >= 0 && adjRow + 1 < 2 * mapHeight && adjCol + 1 >= 0 && adjCol + 1 < 2 * mapWidth)
                    {
                        Tile* t = tileGrid[adjRow + 1][adjCol + 1];
                        if (t == nullptr || t->isBarrier || t->hasTower)
                            addToQueue = false;
                    }
                    break;
                case 5: // W:
                    adjCol -= 2;
                    if (adjRow - 1 >= 0 && adjRow - 1 < 2 * mapHeight && adjCol + 1 >= 0 && adjCol + 1 < 2 * mapWidth)
                    {
                        Tile* t = tileGrid[adjRow - 1][adjCol + 1];
                        if (t == nullptr || t->isBarrier || t->hasTower)
                            addToQueue = false;
                    }
                    if (adjRow + 1 >= 0 && adjRow + 1 < 2 * mapHeight && adjCol + 1 >= 0 && adjCol + 1 < 2 * mapWidth)
                    {
                        Tile* t = tileGrid[adjRow + 1][adjCol + 1];
                        if (t == nullptr || t->isBarrier || t->hasTower)
                            addToQueue = false;
                    }
                    break;
                case 6: // S:
                    adjRow += 2;
                    if (adjRow - 1 >= 0 && adjRow - 1 < 2 * mapHeight && adjCol - 1 >= 0 && adjCol - 1 < 2 * mapWidth)
                    {
                        Tile* t = tileGrid[adjRow - 1][adjCol - 1];
                        if (t == nullptr || t->isBarrier || t->hasTower)
                            addToQueue = false;
                    }
                    if (adjRow - 1 >= 0 && adjRow - 1 < 2 * mapHeight && adjCol + 1 >= 0 && adjCol + 1 < 2 * mapWidth)
                    {
                        Tile* t = tileGrid[adjRow - 1][adjCol + 1];
                        if (t == nullptr || t->isBarrier || t->hasTower)
                            addToQueue = false;
                    }
                    break;
                case 7: // E:
                    adjCol += 2;
                    if (adjRow - 1 >= 0 && adjRow - 1 < 2 * mapHeight && adjCol - 1 >= 0 && adjCol - 1 < 2 * mapWidth)
                    {
                        Tile* t = tileGrid[adjRow - 1][adjCol - 1];
                        if (t == nullptr || t->isBarrier || t->hasTower)
                            addToQueue = false;
                    }
                    if (adjRow + 1 >= 0 && adjRow + 1 < 2 * mapHeight && adjCol - 1 >= 0 && adjCol - 1 < 2 * mapWidth)
                    {
                        Tile* t = tileGrid[adjRow + 1][adjCol - 1];
                        if (t == nullptr || t->isBarrier || t->hasTower)
                            addToQueue = false;
                    }
                    break;
            }

            if (adjRow < 0 || adjRow >= 2 * mapHeight || adjCol < 0 || adjCol >= 2 * mapWidth)
                addToQueue = false;
            else if (tileGrid[adjRow][adjCol] == nullptr || tileGrid[adjRow][adjCol]->isBarrier || tileGrid[adjRow][adjCol]->hasTower)
                addToQueue = false;
            else if (tileGrid[adjRow][adjCol]->dist != -1)
                addToQueue = false;

            if (addToQueue)
            {
                tileGrid[adjRow][adjCol]->dist = dist + 1;
                tileGrid[adjRow][adjCol]->next = tile;
                queue.enqueue(tileGrid[adjRow][adjCol]);
            }
        }
    }

    qDebug() << "Game map flooded.";

    // Ensure that the player cannot close off the base with towers:
    Tile* tile = spawnPoints.front();
    while (tile->next != nullptr && !tile->isBase)
    {
        tile = tile->next;
    }
    if (!tile->isBase)
    {
        // Base has been closed off, remove last added tower:
        Tower* tower = towers.last();
        sellTower(tower->tile->row, tower->tile->col);
    }

}


// New slot for spawning waves based on your pattern and increasing total enemies per wave
void GameMapDialog::spawnWave()
{
    if (spawnPoints.isEmpty())
    {
        qDebug() << "No spawn points available to spawn enemies.";
        return;
    }

    waveEnemies.clear();

    // Define enemy waves by currentWave:
    switch (currentWave)
    {
        case 0: // Wave 1: 3 Skeletons
            waveEnemies.fill(Skeleton, 3);
            break;
        case 1: // Wave 2: 4 Skeletons + 1 Skeleton Archer
            waveEnemies.fill(Skeleton, 4);
            waveEnemies.append(Skeleton_Archer);
            break;
        case 2: // Wave 3: 6 Skeletons + 1 Armoured Skeleton
            waveEnemies.fill(Skeleton, 6);
            waveEnemies.append(Armoured_Skeleton);
            break;
        case 3: // Wave 4: 4 Skeletons + 4 Armoured Skeletons
            waveEnemies.fill(Skeleton, 4);
            waveEnemies.append(Armoured_Skeleton);
            waveEnemies.append(Armoured_Skeleton);
            waveEnemies.append(Armoured_Skeleton);
            waveEnemies.append(Armoured_Skeleton);
            break;
        case 4: // Wave 5: 10 Orcs
            waveEnemies.fill(Orc, 10);
            break;
        default: // Wave 6+: random enemies, totalEnemiesPerWave increases by 2 per wave
        {
            int enemiesToSpawn = totalEnemiesPerWave;
            for (int i = 0; i < enemiesToSpawn; ++i)
            {
                int enemyTypeInt = rand() % 12;
                waveEnemies.append(static_cast<EnemyType>(enemyTypeInt));
            }
            break;
        }
    }

    waveText->setPlainText(QString("Wave: %1").arg(currentWave + 1)); // Update wave number displayed


    for (EnemyType etype : qAsConst(waveEnemies))
    {
        if (spawnPoints.isEmpty())
        {
            qDebug() << "Spawn points empty during enemy spawning!";
            break;
        }

        Tile* spawnTile = spawnPoints.dequeue();
        if (spawnTile == nullptr)
        {
            qDebug() << "Invalid spawn tile during enemy spawning";
            spawnPoints.enqueue(spawnTile);
            continue;
        }

        Enemy* enemy = new Enemy(etype);
        spawnTile->addEnemy(enemy);
        connect(enemy, &Enemy::killEnemy, this, &GameMapDialog::killEnemy);
        enemies.append(enemy);
        gameScene->addItem(enemy);

        spawnPoints.enqueue(spawnTile); // Re-enqueue spawn point to cycle spawn locations

        QCoreApplication::processEvents(); // Keep UI responsive
        QThread::msleep(50);  //Delay 50ms between each enemy spawn to avoid overlap visually
    }

    currentWave++; // Increment wave
    totalEnemiesPerWave += 2; // Increase enemies per wave by 2 for next wave

    qDebug() << "Spawned Wave" << currentWave << "with" << waveEnemies.size() << "enemies.";

}


void GameMapDialog::updateGame()
{
    for (Tower* &tower : towers)
    {
        tower->Tick();
    }

    gameScene->update();
    updateBitcoinDisplay();
    //updateWaveDisplay();
}

void GameMapDialog::tickEnemies()
{
    for (Enemy* &enemy : enemies)
    {
        enemy->Tick();
    }
}

void GameMapDialog::updateWaveDisplay()
{
    if (waveText == nullptr)
        return;

    waveText->setPlainText(QString("Wave: %1").arg(currentWave));
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
    if (bitcoinCount - tower->getCost() >= 0 || type == base)
    {
        if (type != base)
            bitcoinCount -= tower->getCost(); // Pay amount for tower.

        tileGrid[row][col]->addTower(tower);
        gameScene->addItem(tower);
        towers.append(tower);

        connect(tower, &Tower::destroyTower, this, &GameMapDialog::destroyTower);

        // Connect tower attacking signal to tiles that are in range:
        if (!tileGrid[row][col]->isBase)
        {
            int range = tower->getRange();
            for (int i = row-2*range; i <= row+2*range; ++i)
            {
                for (int j = col-2*range; j <= col+2*range; ++j)
                {
                    if (i >= 0 && i < 2*mapHeight && j >= 0 && j < 2*mapWidth)
                    {
                        if (tileGrid[i][j] != nullptr && !tileGrid[i][j]->isBarrier)
                        {
                            int dx = row-i;
                            int dy = col-j;
                            float r = sqrtf(dx*dx + dy*dy);
                            if (r <= 2*range) // Connect tiles that are a radius range away from the tower.
                            {
                                connect(tower, &Tower::Attack, tileGrid[i][j], &Tile::damageEnemy);
                            }
                        }
                    }
                }
            }
        }

        floodFill(); // Recalculate shortest paths.
    }
    else
    {
        delete tower;
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
    if (enemies.removeOne(e))
    {
        bitcoinCount += e->getBitcoinReward();
        e->deleteLater();
    }

}

void GameMapDialog::attackAnimation(Tower* tower, Enemy* enemy)
{
    QPen pen(Qt::red);
    pen.setWidth(1);
    pen.setStyle(Qt::SolidLine);
    pen.setCosmetic(true);
    float x1 = tower->x() + tileSize/2;
    float y1 = tower->y() - tileSize/2;
    float x2 = enemy->x() + tileSize/2;
    float y2 = enemy->y() + tileSize/4;
    QGraphicsLineItem* line = gameScene->addLine(QLineF(x1, y1, x2, y2), pen);
    line->setZValue(1000);

    QTimer::singleShot(250, this, [line, this]() {
        if (gameScene) {
            gameScene->removeItem(line);
        }
        delete line;
    });
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
        gameTick->stop();
        enemyTick->stop();
        waveTimer->stop();
        pauseMenu = new PauseMenuDialog(this);
        connect(pauseMenu, &PauseMenuDialog::resumeGame, this, &GameMapDialog::onResumeGame);
        connect(pauseMenu, &PauseMenuDialog::saveGame, this, &GameMapDialog::onSaveGame);
        connect(pauseMenu, &PauseMenuDialog::exitGame, this, &GameMapDialog::onExitGame);
        pauseMenu->exec();
    }
}

void GameMapDialog::resumeGame()
{
    gameTick->start();
    enemyTick->start();
    waveTimer->start();
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

void GameMapDialog::cleanState()
{
    // Remove and delete all enemies
    for (Enemy* enemy : qAsConst(enemies)) {
        gameScene->removeItem(enemy);
        delete enemy;
    }
    enemies.clear();

    // Remove and delete all towers
    for (Tower* tower : qAsConst(towers)) {
        gameScene->removeItem(tower);
        delete tower;
    }
    towers.clear();

    // Remove and delete all tiles
    for (int r = 0; r < 2 * mapHeight; ++r) {
        for (int c = 0; c < 2 * mapWidth; ++c) {
            if (tileGrid[r][c] != nullptr) {
                gameScene->removeItem(tileGrid[r][c]);
                delete tileGrid[r][c];
                tileGrid[r][c] = nullptr;
            }
        }
    }
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

bool GameMapDialog::loadGameFromFile(const QString& filename) {
    QString filePath = QCoreApplication::applicationDirPath() + "/saves/" + filename;
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Cannot open file for loading:" << filePath;
        return false;
    }

    QString fileContent = file.readAll();
    QStringList lines = fileContent.split('\n', QString::SkipEmptyParts);

    // Initialize default values in case of partial file
    mapType = map1;
    gameDifficulty = medium;
    bitcoinCount = 200;
    savedBitcoinCount = 200;
    currentWave = 0;

    bool readingEnemies = false;
    bool readingTowers = false;

    for (const QString& line : qAsConst(lines)) {
        if (line.isEmpty()) continue;
        if (line.startsWith("Enemies:")) {
            readingEnemies = true;
            readingTowers = false;
            continue;
        }
        if (line.startsWith("Towers:")) {
            readingEnemies = false;
            readingTowers = true;
            continue;
        }

        if (!readingEnemies && !readingTowers) {
            // Header parsing
            if (line.startsWith("MapType:")) {
                bool ok;
                int mt = line.section(':', 1).trimmed().toInt(&ok);
                if (ok && mt >= 0 && mt <= 2) mapType = static_cast<map>(mt);
            } else if (line.startsWith("Difficulty:")) {
                bool ok;
                int diff = line.section(':', 1).trimmed().toInt(&ok);
                if (ok && diff >= 0 && diff <= 2) {
                    gameDifficulty = static_cast<difficulty>(diff);
                    setDifficulty(diff);
                }
            } else if (line.startsWith("BitcoinCount:")) {
                bool ok;
                int count = line.section(':', 1).trimmed().toInt(&ok);
                if (ok && count >= 0) {
                    bitcoinCount = count;
                    savedBitcoinCount = count;
                }
            } else if (line.startsWith("CurrentWave:")) {
                bool ok;
                currentWave = line.section(':', 1).trimmed().toInt(&ok);
                if (!ok) currentWave = 0;
            }
        } else if (readingEnemies) {
            QStringList parts = line.split(' ', QString::SkipEmptyParts);
            if (parts.size() < 4) continue;
            bool ok;
            EnemyType etype = static_cast<EnemyType>(parts[0].toInt(&ok));
            if (!ok) continue;
            float x = parts[1].toFloat(&ok);
            if (!ok) continue;
            float y = parts[2].toFloat(&ok);
            if (!ok) continue;
            int health = parts[3].toInt(&ok);
            if (!ok || health <= 0) health = 1;

            int gridX = static_cast<int>(x / tileSize);
            int gridY = static_cast<int>(y / (tileSize / 2));
            if (gridX >= 0 && gridX < 2 * mapWidth && gridY >= 0 && gridY < 2 * mapHeight &&
                tileGrid[gridY][gridX] != nullptr && mapGrid[gridY][gridX] != 0) {
                Enemy* enemy = new Enemy(etype);
                enemy->setHealth(health);
                enemy->setJustLoaded(true);
                enemies.append(enemy);
                tileGrid[gridY][gridX]->addEnemy(enemy);
                gameScene->addItem(enemy);
            }
        } else if (readingTowers) {
            QStringList parts = line.split(' ', QString::SkipEmptyParts);
            if (parts.size() < 4) continue;
            bool ok;
            towerType ttype = static_cast<towerType>(parts[0].toInt(&ok));
            if (!ok) continue;
            int row = parts[1].toInt(&ok);
            if (!ok) continue;
            int col = parts[2].toInt(&ok);
            if (!ok) continue;
            int level = parts[3].toInt(&ok);
            if (!ok || level < 1) level = 1;

            if (row >= 0 && row < 2 * mapHeight && col >= 0 && col < 2 * mapWidth &&
                tileGrid[row][col] != nullptr && mapGrid[row][col] != 0) {
                Tower* tower = new Tower(ttype);
                tower->towerLevel = level;
                tower->setZValue(1);
                towers.append(tower);
                tileGrid[row][col]->addTower(tower);
                gameScene->addItem(tower);
            }
        }
    }

    cleanState();
    drawMap();

    bitcoinCount = savedBitcoinCount;
    updateBitcoinDisplay();

    file.close();
    return true;
}
