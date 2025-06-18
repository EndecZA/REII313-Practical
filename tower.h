#ifndef TOWER_H
#define TOWER_H

#include <QGraphicsPixmapItem>
#include <QObject>
#include <QWidget>
#include <QPixmap>
#include <QDebug>
#include <QProgressBar>

class Tile;

enum towerType {barricade, melee, archer, fire, wizard, base};

class Tower : public QObject, public QGraphicsPixmapItem
{
    Q_OBJECT

public:
    explicit Tower(towerType);
    towerType type;
    int getCost();
    int getRange();
    int getPiercing();

    void Tick(); // Tick function for tower.
    Tile *tile;
    int towerLevel;

private:
    QPixmap *pixmap;
    static const int towerW = 70;
    static const int towerH = 130;

    int animationCounter; // Iterate over animation frames.
    int attackCounter; // Counter to keep track of the amount of received ticks.
    int maxHealth; // Tower health: Only applicable to barricades and the base type.
    int health;
    int damage;
    int fireRate; // Number of ticks that the tower waits before attacking next.
    int range; // Square attack radius.
    int piercing; // Number of enemies that a tower can attack at a time.
    int cost; // Cost of construction or upgrade.

    QGraphicsRectItem *healthBarBack;
    QGraphicsRectItem *healthBarFront;

public slots:
    int Upgrade(int balance); // Input: Currency balance. Output: Balance after upgrade.
    void Damage(int damage); // Damage tower.

signals:
    void Attack(int damage, int piercing, Tower*); // Attack connected enemies.
    void destroyTower(int row, int col);
};

#endif // TOWER_H
