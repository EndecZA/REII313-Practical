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
#include <QQueue>


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

        currentWave = 0;
        enemiesPerWave = 5;
        enemiesToSpawn = 0;

        // NOTE: Reimplement!
//        waveTimer = new QTimer(this);
//        connect(waveTimer, &QTimer::timeout, this, &GameMapDialog::startNextWave);
//        waveTimer->start(10000);

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
                addToQueue = false; // Break if tile is unaccessable by enemies.
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

    // Ensure that enemies can allways reach the base:
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
    // Update state of all enemies:
    for (Enemy* &enemy : enemies)
    {
        enemy->Tick();
        if (enemy->getState() == Idle)
            enemy->setState(Moving);
    }

    // Update state of all towers:
    for (Tower* &tower : towers)
    {
        tower->Tick();
    }

    updateBitcoinDisplay();
    gameScene->update();

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
        bitcoinCount += tower->getCost(); // Receive back amount payed for tower.
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

// NB!! Reimplement code here!!

//void GameMapDialog::spawnEnemy(EnemyType type, const QPointF& pos)
//{
//    // Force spawn point to be walkable
//    int gridX = static_cast<int>(pos.x() / tileSize);
//    int gridY = static_cast<int>(pos.y() / (tileSize / 2));
//    if (gridX >= 0 && gridX < 2 * mapWidth && gridY >= 0 && gridY < 2 * mapHeight) {
//        mapGrid[gridY][gridX] = 1; // Grass tile
//        barrierGrid[2 * mapHeight - gridY - 1][gridX] = 0; // No barrier
//        qDebug() << "Forced spawn point" << pos << "to grass at grid (" << gridX << "," << gridY << ")";
//    } else {
//        qDebug() << "Error: Spawn point" << pos << "outside bounds";
//    }

//    Enemy* enemy = new Enemy(type, pos, this);
//    if (enemy->pixmap().isNull()) {
//        qDebug() << "Skipping spawn of type" << type << "due to null pixmap at" << pos;
//        delete enemy;
//        return;
//    }
//    switch (gameDifficulty) {
//        case easy:
//            enemy->setHealth(enemy->getHealth() * 0.8);
//            break;
//        case medium:
//            break;
//        case hard:
//            enemy->setHealth(enemy->getHealth() * 1.5);
//            enemy->setDamage(enemy->getDamage() * 1.2);
//            break;
//    }
//    enemy->setZValue(0);
//    enemies.append(enemy);
//    gameScene->addItem(enemy);
//    qDebug() << "Spawned enemy type" << type << "at" << pos;
//}

//void GameMapDialog::startNextWave()
//{
//    currentWave++;
//    enemiesToSpawn = enemiesPerWave + currentWave * 2;
//    qDebug() << "Starting wave" << currentWave << "with" << enemiesToSpawn << "enemies";

//    QTimer* spawnTimer = new QTimer(this);
//    int spawnInterval = 1000;
//    int enemiesSpawned = 0;

//    connect(spawnTimer, &QTimer::timeout, this, [=]() mutable {
//        if (enemiesSpawned < enemiesToSpawn) {
//            QVector<QPointF> spawnPoints = getSpawnPoints();
//            if (!spawnPoints.isEmpty()) {
//                QPointF spawnPos = spawnPoints[rand() % spawnPoints.size()];
//                EnemyType type = (currentWave < 3) ? Skeleton :
//                                 (currentWave < 6) ? Orc : Knight;
//                spawnEnemy(type, spawnPos);
//                enemiesSpawned++;
//            }
//        } else {
//            spawnTimer->stop();
//            spawnTimer->deleteLater();
//            qDebug() << "Wave" << currentWave << "completed";
//        }
//    });
//    spawnTimer->start(spawnInterval);
//}

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
//        waveTimer->stop();
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
//    waveTimer->start();
    updateTimer->start();
    pauseMenu = nullptr;
}

void GameMapDialog::onResumeGame()
{
    resumeGame();
}

void GameMapDialog::onSaveGame()
{
    qDebug() << "Save game placeholder";
    resumeGame();
}

void GameMapDialog::onExitGame()
{
    close();
}
