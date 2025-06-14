#include "tile.h"

Tile::Tile(int tileType, int barrierType, int row, int col) : QObject(), QGraphicsPixmapItem()
{
    // Initialize flood fill parameters:
    isBarrier = false;
    hasTower = false;
    weight = 1;
    dist = 999; // Initialize minimum distance.
    next = nullptr;
    tower = nullptr;

    // Save tileset on the stack to copy from:
    QPixmap tileset(":/resources/images/tileset.png");

    pos[0] = col*tileSize/2 - tileSize/4;
    pos[1] = row*tileSize/4 + rand()%5 - 2; // Add vertical variation for interest.

    // Add tile Pixmap to tile:
    int rowPixmap = 0;
    int colPixmap = 0;
    switch (tileType)
    {
        case 1: // Grass Variant:
            rowPixmap = rand()%2 + 6;
            colPixmap = rand()%3 + 3;
        break;
        case 2: // Sand Variant:
            rowPixmap = 2;
            colPixmap = 0;
        break;
        case 3: // Water:
            rowPixmap = 0;
            colPixmap = 1;
            weight = 2; // Difficult terrain.
        break;
        case 4: // Lava:
            rowPixmap = 0;
            colPixmap = 3;
            weight = 2; // Difficult terrain.
        break;
        case 5: // Brick Variant:
            rowPixmap = rand()%3 + 8;
            colPixmap = 0;
        break;
        default: // Grass Variant:
            rowPixmap = rand()%2 + 6;
            colPixmap = rand()%3 + 3;
        break;
    }
    rowPixmap *= tileSize;
    colPixmap *= tileSize;
    setPixmap(tileset.copy(colPixmap, rowPixmap, tileSize, tileSize));
    setPos(pos[0], pos[1]);
    setZValue(-1);

    // Add barrier if there is a barrier to be added:
    if (barrierType == 0)
    {
        if (rand()%20 == 0 && (tileType == 1 || tileType == 2)) // Add tile accessories for 1 in every 20 empty tiles (decorative items).
        {
            int rowPixmap = 0;
            int colPixmap = 0;
            switch (rand()%7)
            {
                case 0: // Rock Variant:
                case 1:
                    rowPixmap = rand()%3 + 3;
                    colPixmap = 10;
                break;
                case 2: // Flowers Variant:
                case 3:
                    rowPixmap = rand()%2 + 6;
                    colPixmap = rand()%3;
                break;
                case 4: // Bush Variant:
                case 5:
                    rowPixmap = rand()%2 + 6;
                    colPixmap = 10;
                break;
                case 6: // Container Variant:
                    rowPixmap = 11;
                    colPixmap = rand()%6 + 2;
                break;
                default: // Rock Variant:
                    rowPixmap = rand()%3 + 3;
                    colPixmap = 10;
                break;
            }
            rowPixmap *= tileSize;
            colPixmap *= tileSize;
            QGraphicsPixmapItem *barrier = new QGraphicsPixmapItem(tileset.copy(colPixmap, rowPixmap, tileSize, tileSize));
            barrier->setParentItem(this); // Make the tile the parent of the barrier.
            barrier->setPos(rand()%3 - 1, rand()%3 - 1 - tileSize/4); // Add vertical offset to accessory.
            barrier->setZValue(-1);
        }
        else if (rand()%20 == 0 && tileType == 5)
        {
            int rowPixmap = 0;
            int colPixmap = 0;
            switch (rand()%2)
            {
                case 0: // Rock Variant:
                    rowPixmap = rand()%3 + 3;
                    colPixmap = 10;
                break;
                case 1: // Container Variant:
                    rowPixmap = 11;
                    colPixmap = rand()%6 + 2;
                break;
                default: // Rock Variant:
                    rowPixmap = rand()%3 + 3;
                    colPixmap = 10;
                break;
            }
            rowPixmap *= tileSize;
            colPixmap *= tileSize;
            QGraphicsPixmapItem *barrier = new QGraphicsPixmapItem(tileset.copy(colPixmap, rowPixmap, tileSize, tileSize));
            barrier->setParentItem(this); // Make the tile the parent of the barrier.
            barrier->setPos(rand()%3 - 1, rand()%3 - 1 - tileSize/4); // Add vertical offset to accessory.
            barrier->setZValue(-1);
        }
        else
        {
            barrier = nullptr;
        }
    }
    else
    {
        isBarrier = true;
        int rowPixmap = 0;
        int colPixmap = 0;
        switch (barrierType)
        {
            case 1: // Grass Variant:
                rowPixmap = rand()%2 + 6;
                colPixmap = rand()%3 + 3;
            break;
            case 2: // Sand Variant:
                rowPixmap = 2;
                colPixmap = 0;
            break;
            case 3: // Water:
                rowPixmap = 0;
                colPixmap = 1;
            break;
            case 4: // Lava:
                rowPixmap = 0;
                colPixmap = 3;
            break;
            case 5: // Brick Variant:
                rowPixmap = rand()%3 + 8;
                colPixmap = 0;
            break;
            default: // Brick Variant:
                rowPixmap = rand()%3 + 8;
                colPixmap = 0;
            break;
        }
        rowPixmap *= tileSize;
        colPixmap *= tileSize;
        QGraphicsPixmapItem *barrier = new QGraphicsPixmapItem(tileset.copy(colPixmap, rowPixmap, tileSize, tileSize));
        barrier->setParentItem(this); // Make the tile the parent of the barrier.
        barrier->setPos(0, -tileSize/2); // Add vertical offset to accessory
        barrier->setZValue(-1);
    }
    setFlag(QGraphicsItem::ItemStacksBehindParent, false);

}

void Tile::addTower(Tower *t)
{
    if (!hasTower && !isBarrier)
    {
        if (barrier != nullptr)
            barrier->hide(); // Hide any accessories.

        hasTower = true;
        tower = t;
        tower->setZValue(0);
        tower->setPos(pos[0], pos[1]);
    }
}

Tower* Tile::removeTower() // Remove tower from tile.
{
    if (hasTower)
    {
        if (barrier != nullptr)
            barrier->show(); // Show any accessories.

        hasTower = false;
        Tower* out = tower;
        tower = nullptr;

        return out;
    }
    else
        return nullptr;
}

void Tile::addEnemy(Enemy *e) // Add enemy to tile.
{
    enemies.append(e);
    // Set enemy position here: e->setPos(pos[0], pos[1]); Take the old destination and make it the source, then add the new destination.

}

Enemy* Tile::removeEnemy(Enemy *e) // Remove enemy from tile.
{
    int p = enemies.indexOf(e);
    if (p >= 0)
    {
        enemies.remove(p);
        return e;
    }
    else
        return nullptr; // Return nullptr of the enemy looked for is not contained in the vector.

}
