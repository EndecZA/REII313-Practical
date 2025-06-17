#ifndef ENEMY_H
#define ENEMY_H

#include <QGraphicsPixmapItem>
#include <QWidget>
#include <QObject>
#include <QPixmap>
#include <cmath>

#include "tower.h"

class GameMapDialog;

enum EnemyType {Skeleton, Skeleton_Archer, Armoured_Skeleton, Wizard, Orc, Armoured_Orc, Elite_Orc, Orcastor, Knight, Knight_Templar, Werebear, Cleric };
enum EnemyState {Moving, Attacking, Damaged, Dying, Idle};

class Enemy : public QObject, public QGraphicsPixmapItem
{
    Q_OBJECT
public:
    Enemy(EnemyType type);

    EnemyType getType();
    void setState(EnemyState state);
    EnemyState getState();
    int getHealth();
    int getDamage();
    int getRange();
    int getBitcoinReward();

    void setDest(int x, int y); // Set new destination position.
    void Tick(); // Tick function for enemy.

    void setPath(const QVector<QPointF>& newPath);
    void setJustLoaded(bool value) { justLoaded = value; }
    bool isJustLoaded() const { return justLoaded; }



private:
    static const int spriteSize = 100;
    static const int frameRate = 8; // Framerate in FPS.
    int source[2];
    int dest[2];
    float moveProgress; // Value between zero and one used for interpolation.
    float attackCooldown; // Value used to delay attacks based on attack speed.
    EnemyType type;
    EnemyState state;
    EnemyState prevState;
    QPixmap *spriteSheet;
    int animationFrames[5];
    int animationCounter[5];
    int health;
    int damage;
    float walkSpeed; // Walking speed of enemy in tiles/second.
    float attackSpeed; // Attacking speed of enemy in attacks/second.
    int attackRange;
    int bitcoinReward;
    bool isMirrored;

signals:
    void Attack(int damage); // Attack any towers that are in range.
    void moveEnemy(Enemy*); // Move enemy to new tile.
    void killEnemy(Enemy*); // Signal to remove enemy from the game map.

public slots:
    void takeDamage(int damage);

};

#endif // ENEMY_H
