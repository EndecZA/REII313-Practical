#ifndef ENEMY_H
#define ENEMY_H

#include <QGraphicsPixmapItem>
#include <QPointF>
#include <QTime>

enum EnemyType {Skeleton, Skeleton_Archer, Armoured_Skeleton, Wizard, Orc, Armoured_Orc, Elite_Orc, Orc_rider, Knight, Knight_Templar, Werebear, Cleric };
enum EnemyState {Moving, Attacking, Dying, Idle};

class Enemy : public QGraphicsPixmapItem
{
public:
    Enemy(EnemyType type, const QPointF& position);
    Enemy();

    int getHealth() const;
    void setHealth(int health);
    int getDamage() const;
    void setDamage(int newDamage);
    float getSpeed() const;
    EnemyType getType() const;
    void takeDamage(int damage);
    bool isAlive() const;
    int getBitcoinReward() const; // New getter for Bitcoin reward

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
    int bitcoinReward; // New member to store Bitcoin reward
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
};

#endif // ENEMY_H
