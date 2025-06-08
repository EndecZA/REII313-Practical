#include "enemy.h"
#include <QGraphicsPixmapItem>

Enemy::Enemy(EnemyType type, const QPointF& position, QWidget* parent)
//    : QGraphicsPixmapItem(parent), type(type), state(Idle), currentFrame(0), animationTimer(0), frameDuration(0.1f))
{
    setPos(position);
    //enum EnemyType {Skeleton, Skeleton_Archer, Armoured_Skeleton, Wizard, Orc, Armoured_Orc, Elite_Orc, Orc_rider, Knight, Knight_Templar, Werebear, Cleric };
    switch (type) {
        case Skeleton:
        health = 16; damage = 1; walkSpeed = 0.8; attackSpeed = 1; attackRange = 1;
        setPixmap(QPixmap(":/resources/images/Skeleton.png"));
        break;
        case Skeleton_Archer:
        health = 16; damage = 1; walkSpeed = 0.8; attackSpeed = 1; attackRange = 5; //subject to change
        setPixmap(QPixmap(":/resources/images/Skeleton_Archer.png"));
        break;
        case Armoured_Skeleton:
        health = 64; damage = 4; walkSpeed = 0.8; attackSpeed = 1; attackRange = 1;
        setPixmap(QPixmap(":/resources/images/Armored_Skeleton.png"));
        break;
        case Wizard:
        health = 80; damage = 3; walkSpeed = 1; attackSpeed = 1; attackRange = 5;
        setPixmap(QPixmap(":/resources/images/Armored_Skeleton.png"));
        break;
        case Orc:
        health = 56; damage = 20; walkSpeed = 0.8; attackSpeed = 1.2; attackRange = 1;
        setPixmap(QPixmap(":/resources/images/Orc.png"));
        break;
        case Armoured_Orc:
        health = 128; damage = 20; walkSpeed = 0.8; attackSpeed = 1; attackRange = 1;
        setPixmap(QPixmap(":/resources/images/Armored_Orc.png"));
        break;
        case Elite_Orc:
        health = 480; damage = 20; walkSpeed = 1; attackSpeed = 1; attackRange = 1;
        setPixmap(QPixmap(":/resources/images/Elite_Orc.png"));
        break;
        case Orc_rider:
        health = 48; damage = 10; walkSpeed = 1.5; attackSpeed = 1; attackRange = 1;
        setPixmap(QPixmap(":/resources/images/Orc_Rider.png"));
        break;
        case Knight:
        health = 240; damage = 15; walkSpeed = 0.8; attackSpeed = 0.8; attackRange = 1;
        setPixmap(QPixmap(":/resources/images/Knight.png"));
        break;
        case Knight_Templar:
        health = 960; damage = 25; walkSpeed = 0.8; attackSpeed = 0.8; attackRange = 1;
        setPixmap(QPixmap(":/resources/images/Knight_Templar.png"));
        break;
        case Werebear:
        health = 280; damage = 20; walkSpeed = 1.5; attackSpeed = 1; attackRange = 1;
        setPixmap(QPixmap(":/resources/images/Werebear.png"));
        break;
        case Cleric:
        health = 560; damage = 20; walkSpeed = 1; attackSpeed = 1; attackRange = 5;
        setPixmap(QPixmap(":/resources/images/Cleric.png"));
        break;

    }

    //Definitions for the Animations of the Enemies
    attackCooldown = 1.0f;
    lastAttackTime = QTime::currentTime();

    frameWidth = 16;
    frameHeight = 16;
    stateFrameCounts[Idle] = 6;
    stateFrameCounts[Moving] = 6;
    stateFrameCounts[Attacking] = 6;
    stateFrameCounts[Dying] = 6;


}
