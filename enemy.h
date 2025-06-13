#ifndef ENEMY_H
#define ENEMY_H

#include <QGraphicsPixmapItem>
#include <QPointF>
#include <QTime>

class GameMapDialog;

enum EnemyType {Skeleton, Skeleton_Archer, Armoured_Skeleton, Wizard, Orc, Armoured_Orc, Elite_Orc, Orcastor, Knight, Knight_Templar, Werebear, Cleric };
enum EnemyState {Moving, Attacking, Dying, Idle};

class Enemy : public QGraphicsPixmapItem
{
public:
    Enemy(EnemyType type, const QPointF& position, GameMapDialog* mapDialog = nullptr);
    Enemy();
    ~Enemy();

    int getHealth();
    void setHealth(int health);
    int getDamage();
    void setDamage(int newDamage);
    float getSpeed();
    EnemyType getType();
    void takeDamage(int damage);
    bool isAlive();
    int getBitcoinReward();

    void attack();
    bool canAttack();
    float getAttackRange();

    void setState(EnemyState state);
    void UpdateAnimation();
    void update();

private:
    EnemyType type;
    int health;
    int damage;
    float walkSpeed;
    float attackSpeed;
    float attackRange;
    int bitcoinReward;
    QPointF target;
    QTime lastAttackTime;
    float attackCooldown;

    EnemyState state;
    QPixmap spriteSheet;
    int frameWidth;
    int frameHeight;
    int currentFrame;
    float animationTimer;
    float frameDuration;
    QMap<EnemyState, int> stateFrameCounts;
    GameMapDialog* mapDialog;
};

#endif // ENEMY_H
