#ifndef ENEMY_H
#define ENEMY_H

#include <QGraphicsPixmapItem>
#include <QPointF>
#include <QTime>

enum EnemyType {Skeleton, Skeleton_Archer, Armoured_Skeleton, Wizard, Orc, Armoured_Orc, Elite_Orc, Orc_rider, Knight, Knight_Templar, Werebear, Cleric };

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
    void setState();
    void UpdateAnimation();

    void update();

private:
    EnemyType type;
    int health;
    int damage;
    float speed;
    float attackRange;
   QPointF target;
   QTime lastAttackTime;
   float attackCooldown;

};

#endif // ENEMY_H
