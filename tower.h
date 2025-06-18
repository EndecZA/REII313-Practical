#ifndef TOWER_H
#define TOWER_H

#include <QGraphicsPixmapItem>
#include <QObject>
#include <QWidget>
#include <QPixmap>
#include <QDebug>
#include <QProgressBar>

#include "enemy.h"

class Tile;

enum towerType {barricade, melee, archer, fire, wizard, base};

class Tower : public QObject, public QGraphicsPixmapItem
{
    Q_OBJECT

public:
    explicit Tower(towerType);
    towerType type;
    int getCost();
    int getUpgradeCost();
    int getLevel();
    int getRange();
    int getPiercing();

    void Tick(); // Tick function for tower.
    Tile *tile;
    int towerLevel;

private:
    QPixmap *pixmap;
    static const int towerW = 70;
    static const int towerH = 130;
    static const int frameRate = 8;

    int animationCounter; // Iterate over animation frames.
    float attackSpeed; // Attacking speed of enemy in attacks/second.
    float attackCooldown; // Value used to delay attacks based on attack speed.
    int maxHealth; // Tower health: Only applicable to barricades and the base type.
    int health;
    int damage;
    int fireRate; // Number of ticks that the tower waits before attacking next.
    int range; // Square attack radius.
    int piercing; // Number of enemies that a tower can attack at a time.
    int cost; // Cost of construction or upgrade.
    int upgradeCost; // Cost increase per upgrade.

    QGraphicsRectItem *healthBarBack;
    QGraphicsRectItem *healthBarFront;

public slots:
    int Upgrade(int balance); // Input: Currency balance. Output: Balance after upgrade.
    void Damage(Enemy*); // Damage tower.

signals:
    void Attack(int damage, int piercing, Tower*); // Attack connected enemies.
    void destroyTower(int row, int col);
    void gameLost(); // Signal game loss when base is destroyed.
};

#endif // TOWER_H
