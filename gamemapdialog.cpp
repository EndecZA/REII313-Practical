#include "gamemapdialog.h"
#include <QDebug>
#include <ctime>
#include <QByteArray>

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
    gameView->scale(scale,scale); // Zoom into the scene (all graphics are 4 times larger)
    gameView->show();

    gameDifficulty = medium;
    mapType = map1;
    isMultiplayer = false;

    // Load the pixmap for the tileset and draw the map grid:
    tileset = new QPixmap(":/resources/images/tileset.png");
//    drawMap(); // Add tiles to the background of the scene and barriers to the foreground.

}
//WORK IN PROGRESS
//void GameMapDialog::spawnEnemy(EnemyType type, const QPointF& pos)
//{
//    Enemy* enemy = new Enemy(type, pos, this);
//    switch (gameDifficulty) {
//        case easy:
//            enemy->setHealth(enemy->getHealth() * 0.8);
//            break;
//        case medium:
//            // Default stats
//            break;
//        case hard:
//            enemy->setHealth(enemy->getHealth() * 1.5);
//            enemy->setDamage(enemy->getDamage() * 1.2);
//            break;
//    }
//    enemies.append(enemy);
//    gameScene->addItem(enemy);
//}

void GameMapDialog::setDifficulty(int dif)
{
    switch (dif)
    {
        case 0:
            gameDifficulty = easy;
        break;
        case 1:
            gameDifficulty = medium;
        break;
        case 2:
            gameDifficulty = hard;
        break;
        default:
            gameDifficulty = medium;
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
        case 3:
            mapType = random;
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
    return gameDifficulty;
}

void GameMapDialog::drawMap()
{
    // Load appropriate text file for each map.
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
        case random:
            qDebug() << "Generating random Map.";
            genMap();
        break;
    }
    if (!mapFile->open(QIODevice::ReadOnly))
        qDebug() << "Cannot load map file.";

    // Load the values from the text file into integer array:
    int rows= 0;
    int columns = 0;
    while (!mapFile->atEnd())
    {
        columns = 0;
        QByteArray line = mapFile->readLine(); // Read line from file.
        int p = -1;
        do
        {
            p = line.indexOf("\t");
            int tile = line.mid(0, p).toInt(); // Extract tile value from grid.
            line = line.mid(p+1); // Remove tile that was added.
            mapGrid[rows][columns] = tile; // Add tile to grid.

            ++columns;
        }
        while (p != -1);
        ++rows;
    }
    mapFile->close();
    qDebug() << "Extracted map from text file.";

    // Create QGraphicsPixmaps for each tile of the floor:
    srand(time(0));
    for (int i = 0; i<rows; ++i)
    {
        for (int j = 0; j<columns; ++j)
        {
            // Only work with checkerboard indices:
            if ((i%2 == 0 && j%2 == 0) || (i%2 != 0 && j%2 != 0))
            {
                int row = 0; // Row position in pixmap to copy from.
                int col = 0; // Column position in pixmap to copy from.
                switch (mapGrid[i][j])
                {
                case 1: // Grass Variant:
                    row = rand()%2 + 6; // Random variant row index.
                    col = rand()%3 + 3; // Random variant column index.
                break;
                case 2: // Sand:
                    row = 2; // Sand pixmap row index.
                    col = 0; // Sand pixmap column index.
                break;
                case 3: // Water:
                    row = 0; // Water pixmap row index.
                    col = 1; // Water pixmap column index.
                break;
                case 4: // Lava:
                    row = 0; // Lava pixmap row index.
                    col = 3; // Lava pixmap column index.
                break;
                case 5: // Brick Variant:
                    row = rand()%3 + 8; // Random variant row index.
                    col = 0; // Brick column index.
                break;
                default: // Default: Grass Variant:
                    row = rand()%2 + 6; // Random variant row.
                    col = rand()%3 + 3; // Random variant column.
                break;
                }
                row *= tileSize;
                col *= tileSize;
                // Create GraphicsPixmapItem from specific tile in tileset and add to the scene.
                QGraphicsPixmapItem *tile = new QGraphicsPixmapItem(tileset->copy(col, row, tileSize, tileSize));
                int x = j*tileSize/2 - tileSize/2;
                int y = i*tileSize/4 - tileSize/4; // + rand()%5 - 2
                tile->setPos(x, y);
                tile->setZValue(-1); // Ensure that tiles are behind everything else.
                gameScene->addItem(tile);
            }
        }
    }
    qDebug() << "Added tiles to GraphicsView.";

    // Create QGraphicsPixmaps for each barrier:
    for (int i = rows-1; i>=0; --i) // Work from the bottom up.
    {
        for (int j = 0; j<columns; ++j)
        {
            // Only work with checkerboard indices:
            if (((i%2 == 0 && j%2 != 0) || (i%2 == 0 && j%2 != 0)) && (mapGrid[i][j] != 0))
            {
                qDebug() << mapGrid[i][j];
                int row = 0; // Row position in pixmap to copy from.
                int col = 0; // Column position in pixmap to copy from.
                switch (mapGrid[i][j])
                {
                case 1: // Grass Variant:
                    row = rand()%2 + 6; // Random variant row index.
                    col = rand()%3 + 3; // Random variant column index.
                break;
                case 2: // Sand:
                    row = 2; // Sand pixmap row index.
                    col = 0; // Sand pixmap column index.
                break;
                case 3: // Water:
                    row = 0; // Water pixmap row index.
                    col = 1; // Water pixmap column index.
                break;
                case 4: // Lava:
                    row = 0; // Lava pixmap row index.
                    col = 3; // Lava pixmap column index.
                break;
                case 5: // Brick Variant:
                    row = rand()%3 + 8; // Random variant row index.
                    col = 0; // Brick column index.
                break;
                default: // Default: Grass Variant:
                    row = rand()%2 + 6; // Random variant row.
                    col = rand()%3 + 3; // Random variant column.
                break;
                }
                row *= tileSize;
                col *= tileSize;
                // Create GraphicsPixmapItem from specific tile in tileset and add to the scene.
                QGraphicsPixmapItem *barrier = new QGraphicsPixmapItem(tileset->copy(col, row, tileSize, tileSize));
                int x = (j+1)*tileSize/2 - tileSize;
                int y = tileSize*mapHeight/2 - (i+1)*tileSize/4 + tileSize/4 - 32; // + rand()%5 - 2
                barrier->setPos(x, y);
                gameScene->addItem(barrier);
            }
        }
    }
    qDebug() << "Added barriers to GraphicsView.";

}

void GameMapDialog::genMap()
{
    // CODE HERE!!

}
