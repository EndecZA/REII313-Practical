#ifndef TILE_H
#define TILE_H

#include <QGraphicsPixmapItem>
#include <QWidget>
#include <QPixmap>
#include <QVector>
#include <QGraphicsSceneMouseEvent>
#include <QMenu>
#include <QDebug>

#include "enemy.h"
#include "tower.h"

class Tile : public QObject, public QGraphicsPixmapItem
{
    Q_OBJECT
public:
    explicit Tile(int tileType, int barrierType, int row, int col);
    int row, col;
    bool isBarrier;
    bool isBarricade;
    bool hasTower;
    bool isBase;
    int dist; // Distance from current tile to base. Updated during flood filling algorithm.
    Tile *next; // Next tile in shortest path to the base.
    Tower *tower; // Pointer to tower object contained in tile.

    QVector<Enemy*> enemies; // Vector for all enemies currently positioned at tile.

    void addTower(Tower*); // Add a tower to the tile.
    Tower* removeTower(); // Remove tower from tile.
    void addEnemy(Enemy*); // Add enemy to tile.
    Enemy* removeEnemy(Enemy*); // Remove enemy from tile.

private:
    static const int tileSize = 32;
    static const int mapWidth = 15;
    static const int mapHeight = 30;
    int pos[2];
    QGraphicsPixmapItem *barrier; // QGraphicsPixmapItem whose parent item is the Tile itself.
    bool isSpawn;

    void mousePressEvent(QGraphicsSceneMouseEvent*); // Handle click events.


public slots:
    void fetchNext(Enemy*); // Remove enemy from list and add to next tile's list.
    void killEnemy(Enemy*); // Delete enemy.
//    void attackEnemy(int damage); // Attack enemies contained in tile.

signals:
    void buildTower(towerType, int row, int col); // Send signal to build tower on the map.
    void sellTower(int row, int col); // Send signal to sell the tower.
    void upgradeTower(int row, int col); // Send signal to upgrade tower.

};

#endif // TILE_H
