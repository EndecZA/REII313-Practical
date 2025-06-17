#include "tile.h"

Tile::Tile(int tileType, int barrierType, int r, int c) : QObject(), QGraphicsPixmapItem()
{
    // Initialize tile parameters:
    row = r;
    col = c;
    isBarrier = false;
    hasTower = false;
    isBase = false;
    dist = -1; // Initialize distance to destination tile.
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
        default:
            [[fallthrough]];
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
        case 6: // Base Tower position: (use brick variant)
            rowPixmap = rand()%3 + 8;
            colPixmap = 0;
            isBase = true;
        break;
    }
    rowPixmap *= tileSize;
    colPixmap *= tileSize;
    setPixmap(tileset.copy(colPixmap, rowPixmap, tileSize, tileSize));
    setPos(pos[0], pos[1]);
    setZValue(y()); // NB! Set Z Values according to y-position in scene!

    // Add barrier if there is a barrier to be added:
    if (barrierType == 0) // No barrier:
    {
        // Randomly add tile accessories:
        if (rand()%20 == 0 && (tileType == 1 || tileType == 2)) // Add tile accessories for 1 in every 20 empty tiles (decorative items).
        {
            int rowPixmap = 0;
            int colPixmap = 0;
            switch (rand()%7)
            {
                default:
                    [[fallthrough]];
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
            }
            rowPixmap *= tileSize;
            colPixmap *= tileSize;
            QGraphicsPixmapItem *barrier = new QGraphicsPixmapItem(tileset.copy(colPixmap, rowPixmap, tileSize, tileSize));
            barrier->setParentItem(this); // Make the tile the parent of the barrier.
            barrier->setPos(rand()%3 - 1, rand()%3 - 1 - tileSize/4); // Add vertical offset to accessory.
            barrier->setZValue(barrier->y());
        }
        else if (rand()%20 == 0 && tileType == 5)
        {
            int rowPixmap = 0;
            int colPixmap = 0;
            switch (rand()%2)
            {
                default:
                    [[fallthrough]];
                case 0: // Rock Variant:
                    rowPixmap = rand()%3 + 3;
                    colPixmap = 10;
                break;
                case 1: // Container Variant:
                    rowPixmap = 11;
                    colPixmap = rand()%6 + 2;
                break;
            }
            rowPixmap *= tileSize;
            colPixmap *= tileSize;
            QGraphicsPixmapItem *barrier = new QGraphicsPixmapItem(tileset.copy(colPixmap, rowPixmap, tileSize, tileSize));
            barrier->setParentItem(this); // Make the tile the parent of the barrier.
            barrier->setPos(rand()%3 - 1, rand()%3 - 1 - tileSize/4); // Add vertical offset to accessory.
            barrier->setZValue(barrier->y());
        }
        else
        {
            barrier = nullptr;
        }
    }
    else if (!isBase) // If the tile is a barrier:
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
            default:
                [[fallthrough]];
            case 5: // Brick Variant:
                rowPixmap = rand()%3 + 8;
                colPixmap = 0;
            break;
        }
        rowPixmap *= tileSize;
        colPixmap *= tileSize;
        QGraphicsPixmapItem *barrier = new QGraphicsPixmapItem(tileset.copy(colPixmap, rowPixmap, tileSize, tileSize));
        barrier->setParentItem(this); // Make the tile the parent of the barrier.
        barrier->setPos(0, -tileSize/2); // Add vertical offset to accessory
        barrier->setZValue(barrier->y());
    }
    else
        barrier = nullptr;

}

void Tile::addTower(Tower *t)
{
    if (!hasTower && !isBarrier && t != nullptr)
    {
        if (barrier != nullptr)
        {
            barrier->hide(); // Hide accessory.
            update();
        }

        if (!isBase)
            hasTower = true;

        tower = t;
        tower->tile = this;
        tower->setPos(pos[0], pos[1]);
        tower->setZValue(tower->y());
    }

}

Tower* Tile::removeTower() // Remove tower from tile.
{
    if (hasTower)
    {
        if (barrier != nullptr)
        {
            barrier->show(); // Show accessory.
            update();
        }

        hasTower = false;
        Tower* output = tower;
        tower = nullptr;

        return output;
    }
    else
        return nullptr;

}

void Tile::addEnemy(Enemy *e) // Add enemy to tile.
{
    if (e != nullptr)
    {
        enemies.append(e);
        connect(e, &Enemy::moveEnemy, this, &Tile::fetchNext);
        connect(e, &Enemy::killEnemy, this, &Tile::killEnemy);
        e->setDest(pos[0], pos[1]);
    }

}

void Tile::killEnemy(Enemy *e)
{
    removeEnemy(e);
}

Enemy* Tile::removeEnemy(Enemy *e) // Remove enemy from tile.
{
    if (enemies.removeOne(e))
    {
        disconnect(e, &Enemy::moveEnemy, this, &Tile::fetchNext);
        disconnect(e, &Enemy::killEnemy, this, &Tile::killEnemy);
        return e;
    }
    else
        return nullptr; // Return nullptr of the enemy is not contained in the vector.

}

void Tile::fetchNext(Enemy *e) // Remove enemy from list and add to next tile's list.
{
    if (next != nullptr && !next->isBase && !next->isBarrier)
    {
        next->addEnemy(removeEnemy(e)); // Move enemy to the next tile in the shortest path.
    }
    else if (next != nullptr && next->isBase)
    {
        e->setState(Attacking);
    }
    else
        qDebug() << "Tile::Unable to fetch next destination.";

}

void Tile::mousePressEvent(QGraphicsSceneMouseEvent *e) // Handle click events.
{
    if (e->button() == Qt::RightButton)
    {
        if (!isBarrier && !hasTower && !isBase && enemies.isEmpty())
        {
            QMenu *towerMenu = new QMenu();
            towerMenu->setStyleSheet(
                "QMenu {"
                "   background-color: #FFB347;"
                "   color: black;"
                "   border: 1px solid black;"
                "   border-radius: 5px;"
                "   font-family: 'Press Start 2P';"
                "   font-size: 15px;"
                "}");

            QAction* barricadeTower = towerMenu->addAction("Build Barricade: ADD COST");
            QAction* meleeTower = towerMenu->addAction("Build Melee Tower: ADD COST");
            QAction* archerTower = towerMenu->addAction("Build Archer Tower: ADD COST");
            QAction* fireTower = towerMenu->addAction("Build Fire Tower: ADD COST");
            QAction* wizardTower = towerMenu->addAction("Build Wizard Tower: ADD COST");

            connect(towerMenu, &QMenu::triggered, this, [=](QAction* action)
            {
                if (action == barricadeTower)
                    emit buildTower(barricade, row, col);
                else if (action == meleeTower)
                    emit buildTower(melee, row, col);
                else if (action == archerTower)
                    emit buildTower(archer, row, col);
                else if (action == fireTower)
                    emit buildTower(fire, row, col);
                else if (action == wizardTower)
                    emit buildTower(wizard, row, col);

                towerMenu->deleteLater();
            });

            towerMenu->popup(e->screenPos()); // Show menu at mouse position.
        }
        else if (hasTower)
        {
            QMenu *towerMenu = new QMenu();
            towerMenu->setStyleSheet(
                "QMenu {"
                "   background-color: #FFB347;"
                "   color: black;"
                "   border: 1px solid black;"
                "   border-radius: 5px;"
                "   font-family: 'Press Start 2P';"
                "   font-size: 15px;"
                "}");

            QAction* sell = towerMenu->addAction("Sell Tower");
            QAction* upgrade = towerMenu->addAction("Upgrade Tower");

            connect(towerMenu, &QMenu::triggered, this, [=](QAction* action)
            {
                if (action == sell)
                    emit sellTower(row, col);
                else if (action == upgrade)
                    emit upgradeTower(row, col);

                towerMenu->deleteLater();
            });

            towerMenu->popup(e->screenPos()); // Show menu at mouse position.
        }
    }

}
