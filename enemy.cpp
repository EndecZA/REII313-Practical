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



//        case Wizard:
//        health = 80; damage = 3; walkSpeed = 1; attackSpeed = 1; attackRange = 5;
//        setPixmap(QPixmap(":/resources/images/Armored_Skeleton.png"));
//        break;
//        case Wizard:
//        health = 80; damage = 3; walkSpeed = 1; attackSpeed = 1; attackRange = 5;
//        setPixmap(QPixmap(":/resources/images/Armored_Skeleton.png"));
//        break;
//        case Wizard:
//        health = 80; damage = 3; walkSpeed = 1; attackSpeed = 1; attackRange = 5;
//        setPixmap(QPixmap(":/resources/images/Armored_Skeleton.png"));
//        break;
//        case Wizard:
//        health = 80; damage = 3; walkSpeed = 1; attackSpeed = 1; attackRange = 5;
//        setPixmap(QPixmap(":/resources/images/Armored_Skeleton.png"));
//        break;
//        case Wizard:
//        health = 80; damage = 3; walkSpeed = 1; attackSpeed = 1; attackRange = 5;
//        setPixmap(QPixmap(":/resources/images/Armored_Skeleton.png"));
//        break;
//        case Wizard:
//        health = 80; damage = 3; walkSpeed = 1; attackSpeed = 1; attackRange = 5;
//        setPixmap(QPixmap(":/resources/images/Armored_Skeleton.png"));
//        break;
//        case Wizard:
//        health = 80; damage = 3; walkSpeed = 1; attackSpeed = 1; attackRange = 5;
//        setPixmap(QPixmap(":/resources/images/Armored_Skeleton.png"));
//        break;

    }

}
