#include "gamemapdialog.h"
#include <QDebug>
#include <ctime>
#include <QByteArray>
#include <QTimer>
#include <QFont>

GameMapDialog::GameMapDialog(QWidget *parent)
    : QDialog(parent)
{
    setModal(true);
    showMaximized();
    setFixedSize(1920,1080);
    setWindowTitle("Game");

    gameScene = new QGraphicsScene(this);
    gameScene->setSceneRect(0, 0, tileSize*mapWidth, tileSize/2*mapHeight);
    gameScene->setItemIndexMethod(QGraphicsScene::NoIndex);
    gameScene->setBackgroundBrush(Qt::black);

    gameView = new QGraphicsView(gameScene, this);
    gameView->setFixedSize(1920, 1080);
    gameView->setRenderHint(QPainter::Antialiasing);
    gameView->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    gameView->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    int scale = 1920/(tileSize*mapWidth);
    gameView->scale(scale, scale);
    gameView->show();

    gameDifficulty = medium;
    mapType = map1;
    isMultiplayer = false;
    bitcoinCount = 0;

    // Initialize Bitcoin display
    bitcoinText = new QGraphicsTextItem("Bitcoins: 0");
    bitcoinText->setFont(QFont("Arial", 10)); // Reduced font size to 10
    bitcoinText->setDefaultTextColor(Qt::white);

    // Initialize Bitcoin background
    bitcoinBackground = new QGraphicsRectItem(0, 0, 140, 40); // Smaller initial size
    bitcoinBackground->setBrush(QBrush(QColor(0, 0, 0, 128))); // Black with 50% opacity
    bitcoinBackground->setPen(Qt::NoPen);

    // Initialize Bitcoin icon
    QPixmap bitcoinPixmap(":/resources/images/bitcoin.png");
    if (bitcoinPixmap.isNull()) {
        qDebug() << "Failed to load bitcoin.png";
        bitcoinPixmap = QPixmap(32, 32); // Fallback size matches icon size
        bitcoinPixmap.fill(Qt::yellow); // Fallback yellow square
    }
    bitcoinIcon = new QGraphicsPixmapItem(bitcoinPixmap.scaled(32, 32, Qt::KeepAspectRatio)); // Icon remains 32x32

    // Group text, background, and icon
    bitcoinGroup = new QGraphicsItemGroup();
    bitcoinGroup->addToGroup(bitcoinBackground);
    bitcoinGroup->addToGroup(bitcoinIcon);
    bitcoinGroup->addToGroup(bitcoinText);
    bitcoinGroup->setZValue(11); // Ensure group is on top
    gameScene->addItem(bitcoinGroup);

    // Position elements in the top-left corner
    bitcoinBackground->setPos(5, 5); // 5-pixel padding
    bitcoinIcon->setPos(10 + 5, 8); // Icon after padding, adjusted for alignment
    bitcoinText->setPos(10 + 32 + 8, 10); // Text after 32px icon with 3-pixel gap

    srand(time(0));
    tileset = new QPixmap(":/resources/images/tileset.png");
    drawMap();

    // Add debug rectangle for scene boundaries
    QGraphicsRectItem* debugRect = gameScene->addRect(0, 0, tileSize*mapWidth, tileSize/2*mapHeight, QPen(Qt::red));
    debugRect->setZValue(10);

    // Highlight spawn points
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
    updateTimer->start(16);
}

void GameMapDialog::spawnEnemy(EnemyType type, const QPointF& pos)
{
    Enemy* enemy = new Enemy(type, pos);
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
    qDebug() << "Spawned enemy type" << type << "at" << pos << "with zValue" << enemy->zValue();

    // Temporary debug marker
    QGraphicsRectItem* marker = gameScene->addRect(pos.x() - 32, pos.y() - 32, 64, 64, QPen(Qt::blue), QBrush(Qt::blue));
    marker->setZValue(2);
    marker->setOpacity(0.5);
}

QVector<QPointF> GameMapDialog::getSpawnPoints()
{
    QVector<QPointF> spawnPoints;
    switch (mapType) {
        case map1:
            spawnPoints << QPointF(tileSize * 2, tileSize * 2)
                        << QPointF(tileSize * 10, tileSize * 2);
            break;
        case map2:
            spawnPoints << QPointF(tileSize * 5, tileSize * 3)
                        << QPointF(tileSize * 12, tileSize * 3);
            break;
        case map3:
            spawnPoints << QPointF(tileSize * 3, tileSize * 4)
                        << QPointF(tileSize * 8, tileSize * 4);
            break;
    }
    for (const QPointF& point : spawnPoints) {
        int gridX = static_cast<int>(point.x() / tileSize);
        int gridY = static_cast<int>(point.y() / (tileSize / 2));
        if (gridX >= 0 && gridX < 2 * mapWidth && gridY >= 0 && gridY < 2 * mapHeight) {
            if (mapGrid[gridY][gridX] != 1 && mapGrid[gridY][gridX] != 2) {
                qDebug() << "Warning: Spawn point" << point << "is on non-walkable tile" << mapGrid[gridY][gridX];
            }
            if (barrierGrid[2 * mapHeight - gridY - 1][gridX] != 0) {
                qDebug() << "Warning: Spawn point" << point << "overlaps with barrier";
            }
        } else {
            qDebug() << "Error: Spawn point" << point << "is outside map bounds";
        }
    }
    qDebug() << "Spawn points:" << spawnPoints;
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
}

void GameMapDialog::updateBitcoinDisplay()
{
    bitcoinText->setPlainText(QString("Bitcoins: %1").arg(bitcoinCount));
    // Adjust background size dynamically to fit smaller text and 32px icon
    qreal bgWidth = bitcoinText->boundingRect().width() + 32 + 12; // Text width + 32px icon + reduced padding
    qreal bgHeight = qMax(bitcoinText->boundingRect().height(), 32.0) + 8; // Max of text/icon height + reduced padding
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

            if ((rows%2 == 0 && columns%2 == 0) || (rows%2 != 0 && columns%2 != 0))
                mapGrid[rows][columns] = tile;
            else
                mapGrid[rows][columns] = 0;

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
            int x = j*tileSize/2 - tileSize/4;
            int y = i*tileSize/4 - tileSize/4 + rand()%5 - 2;

            if (mapGrid[i][j] != 0)
            {
                int row = 0;
                int col = 0;
                switch (mapGrid[i][j])
                {
                case 1:
                    row = rand()%2 + 6;
                    col = rand()%3 + 3;
                    break;
                case 2:
                    row = 2;
                    col = 0;
                    break;
                case 3:
                    row = 0;
                    col = 1;
                    break;
                case 4:
                    row = 0;
                    col = 3;
                    break;
                case 5:
                    row = rand()%3 + 8;
                    col = 0;
                    break;
                default:
                    row = rand()%2 + 6;
                    col = rand()%3 + 3;
                    break;
                }
                row *= tileSize;
                col *= tileSize;
                QGraphicsPixmapItem *tile = new QGraphicsPixmapItem(tileset->copy(col, row, tileSize, tileSize));
                tile->setPos(x, y);
                tile->setZValue(-1);
                gameScene->addItem(tile);
            }

            if (barrierGrid[i][j] != 0)
            {
                int row = 0;
                int col = 0;
                switch (barrierGrid[i][j])
                {
                    case 1:
                        row = rand()%2 + 6;
                        col = rand()%3 + 3;
                        break;
                    case 2:
                        row = 2;
                        col = 0;
                        break;
                    case 3:
                        row = 0;
                        col = 1;
                        break;
                    case 4:
                        row = 0;
                        col = 3;
                        break;
                    case 5:
                        row = rand()%3 + 8;
                        col = 0;
                        break;
                    default:
                        row = rand()%2 + 6;
                        col = rand()%3 + 3;
                        break;
                }
                row *= tileSize;
                col *= tileSize;
                QGraphicsPixmapItem *barrier = new QGraphicsPixmapItem(tileset->copy(col, row, tileSize, tileSize));
                barrier->setPos(x, y - tileSize/2);
                barrier->setZValue(0);
                gameScene->addItem(barrier);
                qDebug() << "Added barrier at" << barrier->pos() << "with zValue" << barrier->zValue() << "using tile" << barrierGrid[i][j];
            }
        }
    }
    qDebug() << "Added tiles and barriers to GraphicsView.";
}

void GameMapDialog::genMap()
{
    // Placeholder for random map generation
}
