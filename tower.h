#ifndef TOWER_H
#define TOWER_H

#include <QObject>
#include <QGraphicsPixmapItem>
#include <QWidget>
#include <QPixmap>

enum towerType {barricade, melee, archer, fire, wizard, base};
enum towerState {idle, attacking};

class Tower : public QObject, public QGraphicsPixmapItem
{
    Q_OBJECT

public:
    explicit Tower(towerType);
    int getCost();
    void Tick(); // Tick function for tower.

private:
    QPixmap *pixmap;
    towerType type;
    towerState state;
    static const int towerW = 70;
    static const int towerH = 130;
    int animationCounter; // Iterate over animation frames.
    int towerLevel;
    int health; // Tower health: Only applicable to barricades and the base type.
    int damage;
    int fireRate; // Number of ticks that the tower waits before attacking next.
    int attackCounter; // Counter to keep track of the amount of received ticks.
    int range;
    int cost; // Cost of construction or upgrade.

public slots:
    int Upgrade(int balance); // Input: Currency balance. Output: Balance after upgrade.

signals:
    void Attack(int damage); // Attack connected enemies.

};

#endif // TOWER_H
