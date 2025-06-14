#ifndef TILE_H
#define TILE_H

#include <QGraphicsPixmapItem>
#include <QWidget>
#include <QPixmap>
#include <QVector>

#include "enemy.h"
#include "tower.h"

class Tile : public QObject, public QGraphicsPixmapItem
{
    Q_OBJECT
public:
    explicit Tile(int tileType, int barrierType, int row, int col);
    int weight; // Weight modifier for the type of terrain. 1 => normal terain, 2 => difficult terrain
    int dist; // Distance from current tile to base. Updated during flood filling algorithm.
    Tile *next; // Next tile in shortest path to the base.
    int pos[2];
    void addTower(Tower*); // Add a tower to the tile.
    Tower* removeTower(); // Remove tower from tile.
    void addEnemy(Enemy*); // Add enemy to tile.
    Enemy* removeEnemy(Enemy*); // Remove enemy from tile.

private:
    static const int tileSize = 32;
    static const int mapWidth = 15;
    static const int mapHeight = 30;
    bool isBarrier;
    bool hasTower;
    QGraphicsPixmapItem *barrier; // QGraphicsPixmapItem whose parent item is the Tile itself.
    Tower *tower; // Pointer to tower object contained in tile.
    QVector<Enemy*> enemies; // Vector for all enemies currently positioned at tile.


public slots: // TO DO!!
//    void upgradeTower(int balance); // Function to upgrade the tower contained in the tile.
//    Tile* fetchNext(); // Remove enemy from list and add to next tile's list.
//    void Attack(int damage); // Attack enemies contained in tile.

signals:
    void flood(); // If the tile is modified, reflood shortest path.

};

#endif // TILE_H
