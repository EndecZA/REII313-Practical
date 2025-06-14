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
#include <QSound> // Added for QSound

GameMapDialog::GameMapDialog(QWidget *parent)
    : QDialog(parent)
{
    setModal(true);
    showMaximized();
    setFixedSize(1920,1080);
    setWindowTitle("Game");

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
    gameView->centerOn(gameView->width()/2, 0);
    gameView->show();

    gameDifficulty = medium;
    mapType = map1;
    isMultiplayer = false;
    bitcoinCount = 0;

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
    bitcoinGroup->setZValue(11);
    gameScene->addItem(bitcoinGroup);

    bitcoinBackground->setPos(5, 5);
    bitcoinIcon->setPos(10 + 5, 8);
    bitcoinText->setPos(10 + 32 + 8, 10);

    tileset = new QPixmap(":/resources/images/tileset.png");
    drawMap();

//    QGraphicsRectItem* debugRect = gameScene->addRect(0, 0, tileSize*mapWidth, tileSize/2*mapHeight, QPen(Qt::red));
//    debugRect->setZValue(10);

    QVector<QPointF> spawnPoints = getSpawnPoints();
//    for (const QPointF& point : spawnPoints) {
//        QGraphicsRectItem* spawnMarker = gameScene->addRect(point.x() - 16, point.y() - 16, 32, 32, QPen(Qt::yellow), QBrush(Qt::yellow));
//        spawnMarker->setZValue(9);
//    }

    currentWave = 0;
    enemiesPerWave = 5;
    enemiesToSpawn = 0;
    waveTimer = new QTimer(this);
    connect(waveTimer, &QTimer::timeout, this, &GameMapDialog::startNextWave);
    waveTimer->start(10000);

    updateTimer = new QTimer(this);
    connect(updateTimer, &QTimer::timeout, this, &GameMapDialog::updateGame);
    updateTimer->start(125); // TEMP TEST: Animate on 3s => 8 frames per second.

    // Initialize background music with QSound
    backgroundSound = new QSound(":/resources/audio/audio.wav", this);
    backgroundSound->setLoops(-1); // Infinite looping
    backgroundSound->play();

    // Debug resource availability
    QFile file(":/resources/audio/audio.wav");
    if (!file.exists()) {
        qDebug() << "Audio file not found in resources!";
    } else {
        qDebug() << "Audio file found in resources.";
    }

    // TEMP TEST:
    tower = new Tower(archer);
    tileGrid[16][16]->addTower(tower);
    gameScene->addItem(tower);

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
    enemy->setZValue(1);
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
    spawnPoints << QPointF(80, 35) << QPointF(320, 95);
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

void GameMapDialog::updateGame()
{
    QVector<Enemy*> enemiesToRemove;
    for (Enemy* enemy : enemies) {
        if (!enemy->isAlive()) {
            bitcoinCount += enemy->getBitcoinReward();
            enemiesToRemove.append(enemy);
            gameScene->removeItem(enemy);
        } else {
            enemy->update();
        }
    }
    for (Enemy* enemy : enemiesToRemove) {
        enemies.removeOne(enemy);
        delete enemy;
    }
    updateBitcoinDisplay();
    gameScene->update();

    // TEST:
    tower->Tick();
}

void GameMapDialog::updateBitcoinDisplay()
{
    bitcoinText->setPlainText(QString("Bitcoins: %1").arg(bitcoinCount));
    qreal bgWidth = bitcoinText->boundingRect().width() + 32 + 12;
    qreal bgHeight = qMax(bitcoinText->boundingRect().height(), 32.0) + 8;
    bitcoinBackground->setRect(0, 0, bgWidth, bgHeight);
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
                tileGrid[i][j]->setZValue(-1);
            } 
            else
            {
                tileGrid[i][j] = nullptr;
            }
        }
    }
    qDebug() << "Added tiles and barriers to GraphicsView.";

}

QVector<QPointF> GameMapDialog::findPath(const QPointF& start, const QPointF& target)
{
    // Snap position to grid center
    int startX = static_cast<int>(std::round(start.x() / tileSize));
    int startY = static_cast<int>(std::round(start.y() / (tileSize / 2)));
    int targetX = static_cast<int>(target.x() / tileSize);
    int targetY = static_cast<int>(target.y() / (tileSize / 2));

    // Validate bounds
    if (startX < 0 || startX >= 2 * mapWidth || startY < 0 || startY >= 2 * mapHeight ||
        targetX < 0 || targetX >= 2 * mapWidth || targetY < 0 || targetY >= 2 * mapHeight) {
        qDebug() << "Pathfinding failed: Start" << start << "(grid:" << startX << "," << startY << ") or target" << target << "(grid:" << targetX << "," << targetY << ") out of bounds";
        return QVector<QPointF>();
    }

    // Adjust start to nearest grass tile if not walkable
    QPointF adjustedStart = QPointF(startX * tileSize + tileSize / 2, startY * (tileSize / 2) + tileSize / 4);
    if (mapGrid[startY][startX] != 1 || barrierGrid[2 * mapHeight - startY - 1][startX] != 0) {
        qDebug() << "Start" << start << "not walkable (mapGrid:" << mapGrid[startY][startX]
                 << ", barrierGrid:" << barrierGrid[2 * mapHeight - startY - 1][startX] << ")";
        int minDist = std::numeric_limits<int>::max();
        int nearestX = startX;
        int nearestY = startY;
        for (int y = 0; y < 2 * mapHeight; ++y) {
            for (int x = 0; x < 2 * mapWidth; ++x) {
                if (mapGrid[y][x] == 1 && barrierGrid[2 * mapHeight - y - 1][x] == 0) {
                    int dist = abs(x - startX) + abs(y - startY);
                    if (dist < minDist) {
                        minDist = dist;
                        nearestX = x;
                        nearestY = y;
                    }
                }
            }
        }
        if (minDist != std::numeric_limits<int>::max()) {
            startX = nearestX;
            startY = nearestY;
            adjustedStart = QPointF(startX * tileSize + tileSize / 2, startY * (tileSize / 2) + tileSize / 4);
            qDebug() << "Adjusted start to" << adjustedStart << "grid (" << startX << "," << startY << ")";
        } else {
            qDebug() << "No walkable tiles near" << start;
            return QVector<QPointF>();
        }
    }

    // Track visited tiles to prevent loops
    static QSet<QPair<int, int>> visited;
    if (visited.size() > 100) visited.clear(); // Reset if too large
    visited.insert({startX, startY});

    // Check adjacent tiles
    QVector<QPair<int, int>> directions = {{0, -1}, {0, 1}, {-1, 0}, {1, 0}};
    qreal minDistance = std::numeric_limits<qreal>::max();
    QPair<int, int> bestTile = {startX, startY}; // Stay put if no valid move

    for (const auto& dir : directions) {
        int newX = startX + dir.first;
        int newY = startY + dir.second;

        // Check bounds
        if (newX < 0 || newX >= 2 * mapWidth || newY < 0 || newY >= 2 * mapHeight) continue;

        // Check walkability
        int barrierY = 2 * mapHeight - newY - 1;
        if (barrierGrid[barrierY][newX] != 0 || mapGrid[newY][newX] != 1) continue;

        // Skip recently visited tiles to prevent loops
        if (visited.contains({newX, newY})) continue;

        // Calculate Euclidean distance to target
        qreal pixelX = newX * tileSize + tileSize / 2;
        qreal pixelY = newY * (tileSize / 2) + tileSize / 4;
        qreal dx = pixelX - target.x();
        qreal dy = pixelY - target.y();
        qreal distance = std::sqrt(dx * dx + dy * dy);

        qDebug() << "Checking tile (" << newX << "," << newY << ") at" << QPointF(pixelX, pixelY)
                 << "distance to target" << distance;

        if (distance < minDistance) {
            minDistance = distance;
            bestTile = {newX, newY};
        }
    }

    // If no better tile, stay put
    if (bestTile.first == startX && bestTile.second == startY) {
        qDebug() << "No valid adjacent tile closer to target from" << adjustedStart;
        visited.clear(); // Reset visited to allow retry
        return QVector<QPointF>{adjustedStart};
    }

    // Convert best tile to pixel coordinates
    qreal pixelX = bestTile.first * tileSize + tileSize / 2;
    qreal pixelY = bestTile.second * (tileSize / 2) + tileSize / 4;
    QPointF nextPoint(pixelX, pixelY);

    // Draw debug line
    QGraphicsLineItem *line = gameScene->addLine(adjustedStart.x(), adjustedStart.y(),
                                                 nextPoint.x(), nextPoint.y(), QPen(Qt::green, 2));
    line->setZValue(8);

    qDebug() << "Next step from" << adjustedStart << "to" << nextPoint << "distance" << minDistance;
    return QVector<QPointF>{nextPoint};
}
