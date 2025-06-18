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


    QGraphicsTextItem *waveText = new QGraphicsTextItem("Wave: 0");
    waveText->setFont(QFont("Arial", 10));
    waveText->setDefaultTextColor(Qt::white);
    gameScene->addItem(waveText);
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

}

void GameMapDialog::updateGame()
{
    QVector<Enemy*> enemiesToRemove;
    for (Enemy* &enemy : enemies) {
        if (!enemy->isAlive()) {
            bitcoinCount += enemy->getBitcoinReward();
            enemiesToRemove.append(enemy);
            gameScene->removeItem(enemy);
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

    // Update state of all towers:



    // Build base tower and run flooding algorithm:
    if (baseRow != -1 && baseCol != -1 && !spawnPoints.isEmpty()) // Map read successfully.
    {
        buildTower(base, baseRow, baseCol);

        // Only start game if a valid map was read:
        gameTick->start(1000/frameRate);
//        waveTimer->start(10000);
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
}


// New slot for spawning waves based on your pattern and increasing total enemies per wave
void GameMapDialog::spawnWave()
{
    if (spawnPoints.isEmpty())
    {
        qDebug() << "No spawn points available to spawn enemies.";
        return;
    }

    Tile* spawnTile = spawnPoints.dequeue();
    if (spawnTile == nullptr)
    {
        qDebug() << "Invalid spawn tile for spawning enemies.";
        return;
    }

    QVector<EnemyType> waveEnemies;

    // Define enemy waves:
    switch (currentWave)
    {
        case 0: // Wave 1 (index 0):
            // 3 Skeletons
            waveEnemies.fill(Skeleton, 3);
            break;
        case 1: // Wave 2:
            // 4 Skeletons and 1 Skeleton Archer
            waveEnemies.fill(Skeleton, 4);
            waveEnemies.append(Skeleton_Archer);
            break;
        case 2: // Wave 3:
            // 6 Skeletons and 1 Armoured Skeleton
            waveEnemies.fill(Skeleton, 6);
            waveEnemies.append(Armoured_Skeleton);
            break;
        case 3: // Wave 4:
            // 4 skeletons and 4 armoured skeletons
            waveEnemies.fill(Skeleton, 4);
            waveEnemies.append(Armoured_Skeleton);
            waveEnemies.append(Armoured_Skeleton);
            waveEnemies.append(Armoured_Skeleton);
            waveEnemies.append(Armoured_Skeleton);
            break;
        case 4: // Wave 5:
            // 10 Orcs
            waveEnemies.fill(Orc, 10);
            break;
        default:
            // Wave 6+ random enemies with totalEnemiesPerWave increasing by 2 each wave
            waveEnemies.clear();
            int enemiesToSpawn = totalEnemiesPerWave; // Use the updated total enemies tracker
            for (int i = 0; i < enemiesToSpawn; ++i)
            {
                int enemyTypeInt = rand() % 12; // Random enemy type between 0 and 11 (all enemy types)
                waveEnemies.append(static_cast<EnemyType>(enemyTypeInt));
            }
            break;
    }

    // Spawn each enemy on the spawnTile
    for (EnemyType etype : qAsConst(waveEnemies))
    {
        Enemy* enemy = new Enemy(etype);
        spawnTile->addEnemy(enemy);
        connect(enemy, &Enemy::killEnemy, this, &GameMapDialog::killEnemy);
        enemies.append(enemy);
        gameScene->addItem(enemy);
    }

    spawnPoints.enqueue(spawnTile); // Re-enqueue spawn point to cycle spawn locations

    currentWave++; // Increment wave
    totalEnemiesPerWave += 2; // Increase enemies per wave by 2 for next wave



    qDebug() << "Spawned Wave" << currentWave << "with" << waveEnemies.size() << "enemies.";
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
    if (!waveText)
        return;

    waveText->setPlainText(QString("Wave: %1").arg(currentWave));
    waveText->setPos(5, 50);
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
        bitcoinCount -= tower->getCost(); // Pay amount for tower.

        tileGrid[row][col]->addTower(tower);
        gameScene->addItem(tower);
        towers.append(tower);

        connect(tower, &Tower::destroyTower, this, &GameMapDialog::destroyTower);

        // Connect tower attacking signal to tiles that are in range:
        int range = tower->getRange();
        for (int i = row-2*range; i <= row+2*range; ++i)
        {
            for (int j = col-2*range; j <= col+2*range; ++j)
            {
                if (i >= 0 && i < 2*mapHeight && j >= 0 && j < 2*mapWidth)
                    if (tileGrid[i][j] != nullptr)
                        connect(tower, &Tower::Attack, tileGrid[i][j], &Tile::damageEnemy);
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

//    QGraphicsRectItem* marker = gameScene->addRect(pos.x() - 32, pos.y() - 32, 64, 64, QPen(Qt::blue), QBrush(Qt::blue));
//    marker->setZValue(2);
//    marker->setOpacity(0.5);
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


void GameMapDialog::attackAnimation(Tower* tower, Enemy* enemy)
{
    QPen pen(Qt::red);
    pen.setWidth(1);
    pen.setStyle(Qt::DashLine);
    float x1 = tower->x();
    float x2 = enemy->x();
    float y1 = tower->y();
    float y2 = enemy->y();
    QGraphicsLineItem* line = gameScene->addLine(QLineF(x1, y1, x2, y2), pen);
    line->setZValue(1000);
    gameScene->update();
}

void GameMapDialog::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape && !pauseMenu) {
        pauseGame();
    }
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
    // waveTimer->start();
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


GameMapDialog::~GameMapDialog()
{
    gameScene->clear();
    delete gameScene;
    delete gameView;
}

void GameMapDialog::clearGameState()
{
    // Remove all enemies from scene and clear vector
    for (Enemy* enemy : enemies) {
        gameScene->removeItem(enemy);
        delete enemy;
    }
    enemies.clear();

    // Remove all towers from scene and clear vector
    for (Tower* tower : towers) {
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
    out << "BitcoinCount: " << bitcoinCount << "\n";
    out << "CurrentWave: " << currentWave << "\n";

    out << "\nEnemies:\n";
    for (Enemy* enemy : enemies) {
        out << (int)enemy->getType() << " "
            << enemy->pos().x() << " "
            << enemy->pos().y() << " "
            << enemy->getHealth() << "\n";
    }

    out << "\nTowers:\n";
    for (Tower* tower : towers) {
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
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Cannot open file for loading:" << filePath;
        qDebug() << "Current working directory:" << QDir::currentPath();
        return false;
    }

    QTextStream in(&file);

    // Clear current game state BEFORE loading new:
    clearGameState();

    // Read header lines for map/difficulty/bitcoin/wave:
    while (!in.atEnd()) {
        QString line = in.readLine();
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
            bool ok;
            bitcoinCount = line.section(':', 1).trimmed().toInt(&ok);
            if (ok) {
                qDebug() << "Loaded bitcoinCount:" << bitcoinCount;
            } else {
                qDebug() << "Invalid BitcoinCount format in line:" << line;
                bitcoinCount = 200; // Default value
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
    updateBitcoinDisplay(); // Update UI immediately

    // Read enemies:
    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();
        if (line.isEmpty()) break; // Empty line means end of enemies section
        QStringList parts = line.split(' ', QString::SkipEmptyParts);

        if (parts.size() < 4) {
            qDebug() << "Invalid enemy line format:" << line;
            break;
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
            enemy->setHealth(health);
            enemies.append(enemy);
            gameScene->addItem(enemy);
            qDebug() << "Loaded enemy type" << etype << "at" << x << y << "health" << health;
        } else {
            qDebug() << "Invalid enemy position loaded at" << x << y;
        }
    }

    // Recompute paths for loaded enemies
    QPointF target(450, tileSize / 2 * mapHeight); // Adjust target as per your game’s target point
    for (Enemy* enemy : enemies) {
        QVector<QPointF> path = findPath(enemy->pos(), target);
        if (!path.isEmpty()) {
            enemy->setPath(path); // Now works with updated Enemy class
        } else {
            qDebug() << "No valid path found for enemy at" << enemy->pos();
        }
    }

    // Read towers:
    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();
        if (line.isEmpty()) continue;
        QStringList parts = line.split(' ', QString::SkipEmptyParts);
        if (parts.size() < 4) {
            qDebug() << "Invalid tower line format:" << line;
            break;
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
    file.close();
    return true;
}
