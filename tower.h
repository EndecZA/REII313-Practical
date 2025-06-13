#ifndef TOWER_H
#define TOWER_H

#include <QGraphicsPixmapItem>
#include <QObject>
#include <QWidget>
#include <QPixmap>
#include "enemy.h"

enum towerType {barricade, melee, archer, fire, wizard};
enum towerState {idle, attacking};

class Tower : public QGraphicsPixmapItem
{
    Q_OBJECT

public:
    Tower(towerType);
    int getCost();
    void connectEnemy(Enemy*); // Function to connect/disconnect any enemy to the tower's attacking signal.
                               // Each connected enemy takes damage per attack.

private:
    QPixmap *pixmap;
    towerType type;
    towerState state;
    int towerLevel;
    int damage;
    int fireRate; // Number of ticks that the tower waits before attacking next.
    int counter; // Counter to keep track of the amount of received ticks.
    int range;
    int cost; // Cost of construction or upgrade.

    int Upgrade(int); // Input: Currency balance. Output: Balance after upgrade.
    void Tick(); // Tick function for tower.

signals:
    void Attack(int); // Attack connected enemies.

};

#endif // TOWER_H
