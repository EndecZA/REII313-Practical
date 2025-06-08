#ifndef ENEMY_H
#define ENEMY_H

#include <QGraphicsPixmapItem>
#include <QPointF>
#include <QTime>

enum EnemyType {Skeleton, Skeleton_Archer, Armoured_Skeleton, Wizard, Orc, Armoured_Orc, Elite_Orc, Orc_rider, Knight, Knight_Templar, Werebear, Cleric };
enum EnemyState {Moving, Attacking, Dying, Idle}; //The idling is used between the attacks of the enemy

class Enemy : public QGraphicsPixmapItem
{
public:
    Enemy(EnemyType type, const QPointF& position, QWidget* parent = nullptr);
    Enemy();

    //For the stat blocks of the enemies
    int getHealth() const;
    void setHealth(int health);
    int getDamage() const;
    float getSpeed() const;
    EnemyType getType() const;
    void takeDamage(int damage);
    bool isAlive() const;

    //Pathfinding?


    //Combat
    void attack();
    bool canAttack(); //in range?
    float getAttackRange();

    //Animation
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
   QPointF target;
   QTime lastAttackTime;
   float attackCooldown;

   EnemyState state;
   QPixmap spriteSheet;
    int frameWidth;
    int frameHeight;
    int currentFrame;
    float animationTimer; // Tracks time since last frame
    float frameDuration; // Seconds per frame
    QMap<EnemyState, int> stateFrameCounts; // Frames per state

};

#endif // ENEMY_H
