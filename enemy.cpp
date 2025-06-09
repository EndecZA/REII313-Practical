#include "enemy.h"
#include <QGraphicsPixmapItem>
#include <QDebug>
#include <QtMath>

Enemy::Enemy(EnemyType type, const QPointF& position)
    : QGraphicsPixmapItem(), type(type)
{
    setPos(position);
    QString pixmapPath;
    QPixmap spriteSheet;
    switch (type) {
        case Skeleton:
            health = 16; damage = 1; walkSpeed = 0.8; attackSpeed = 1; attackRange = 1;
            pixmapPath = ":/resources/images/Skeleton.png";
            spriteSheet = QPixmap(pixmapPath);
            if (spriteSheet.isNull()) {
                qDebug() << "Failed to load sprite sheet for Skeleton at" << pixmapPath;
                break;
            }
            setPixmap(spriteSheet.copy(0, 0, 16, 16).scaled(64, 64, Qt::KeepAspectRatio));
            break;
        case Skeleton_Archer:
            health = 16; damage = 1; walkSpeed = 0.8; attackSpeed = 1; attackRange = 5;
            pixmapPath = ":/resources/images/Skeleton_Archer.png";
            spriteSheet = QPixmap(pixmapPath);
            if (spriteSheet.isNull()) {
                qDebug() << "Failed to load sprite sheet for Skeleton_Archer at" << pixmapPath;
                break;
            }
            setPixmap(spriteSheet.copy(0, 0, 16, 16).scaled(64, 64, Qt::KeepAspectRatio));
            break;
        case Armoured_Skeleton:
            health = 64; damage = 4; walkSpeed = 0.8; attackSpeed = 1; attackRange = 1;
            pixmapPath = ":/resources/images/Armored_Skeleton.png";
            spriteSheet = QPixmap(pixmapPath);
            if (spriteSheet.isNull()) {
                qDebug() << "Failed to load sprite sheet for Armoured_Skeleton at" << pixmapPath;
                break;
            }
            setPixmap(spriteSheet.copy(0, 0, 16, 16).scaled(64, 64, Qt::KeepAspectRatio));
            break;
        case Wizard:
            health = 80; damage = 3; walkSpeed = 1; attackSpeed = 1; attackRange = 5;
            pixmapPath = ":/resources/images/Wizard.png";
            spriteSheet = QPixmap(pixmapPath);
            if (spriteSheet.isNull()) {
                qDebug() << "Failed to load sprite sheet for Wizard at" << pixmapPath;
                break;
            }
            setPixmap(spriteSheet.copy(0, 0, 16, 16).scaled(64, 64, Qt::KeepAspectRatio));
            break;
        case Orc:
            health = 56; damage = 20; walkSpeed = 0.8; attackSpeed = 1.2; attackRange = 1;
            pixmapPath = ":/resources/images/Orc.png";
            spriteSheet = QPixmap(pixmapPath);
            if (spriteSheet.isNull()) {
                qDebug() << "Failed to load sprite sheet for Orc at" << pixmapPath;
                break;
            }
            setPixmap(spriteSheet.copy(0, 0, 16, 16).scaled(64, 64, Qt::KeepAspectRatio));
            break;
        case Armoured_Orc:
            health = 128; damage = 20; walkSpeed = 0.8; attackSpeed = 1; attackRange = 1;
            pixmapPath = ":/resources/images/Armored_Orc.png";
            spriteSheet = QPixmap(pixmapPath);
            if (spriteSheet.isNull()) {
                qDebug() << "Failed to load sprite sheet for Armoured_Orc at" << pixmapPath;
                break;
            }
            setPixmap(spriteSheet.copy(0, 0, 16, 16).scaled(64, 64, Qt::KeepAspectRatio));
            break;
        case Elite_Orc:
            health = 480; damage = 20; walkSpeed = 1; attackSpeed = 1; attackRange = 1;
            pixmapPath = ":/resources/images/Elite_Orc.png";
            spriteSheet = QPixmap(pixmapPath);
            if (spriteSheet.isNull()) {
                qDebug() << "Failed to load sprite sheet for Elite_Orc at" << pixmapPath;
                break;
            }
            setPixmap(spriteSheet.copy(0, 0, 16, 16).scaled(64, 64, Qt::KeepAspectRatio));
            break;
        case Orc_rider:
            health = 48; damage = 10; walkSpeed = 1.5; attackSpeed = 1; attackRange = 1;
            pixmapPath = ":/resources/images/Orc_Rider.png";
            spriteSheet = QPixmap(pixmapPath);
            if (spriteSheet.isNull()) {
                qDebug() << "Failed to load sprite sheet for Orc_rider at" << pixmapPath;
                break;
            }
            setPixmap(spriteSheet.copy(0, 0, 16, 16).scaled(64, 64, Qt::KeepAspectRatio));
            break;
        case Knight:
            health = 240; damage = 15; walkSpeed = 0.8; attackSpeed = 0.8; attackRange = 1;
            pixmapPath = ":/resources/images/Knight.png";
            spriteSheet = QPixmap(pixmapPath);
            if (spriteSheet.isNull()) {
                qDebug() << "Failed to load sprite sheet for Knight at" << pixmapPath;
                break;
            }
            setPixmap(spriteSheet.copy(0, 0, 16, 16).scaled(64, 64, Qt::KeepAspectRatio));
            break;
        case Knight_Templar:
            health = 960; damage = 25; walkSpeed = 0.8; attackSpeed = 0.8; attackRange = 1;
            pixmapPath = ":/resources/images/Knight_Templar.png";
            spriteSheet = QPixmap(pixmapPath);
            if (spriteSheet.isNull()) {
                qDebug() << "Failed to load sprite sheet for Knight_Templar at" << pixmapPath;
                break;
            }
            setPixmap(spriteSheet.copy(0, 0, 16, 16).scaled(64, 64, Qt::KeepAspectRatio));
            break;
        case Werebear:
            health = 280; damage = 20; walkSpeed = 1.5; attackSpeed = 1; attackRange = 1;
            pixmapPath = ":/resources/images/Werebear.png";
            spriteSheet = QPixmap(pixmapPath);
            if (spriteSheet.isNull()) {
                qDebug() << "Failed to load sprite sheet for Werebear at" << pixmapPath;
                break;
            }
            setPixmap(spriteSheet.copy(0, 0, 16, 16).scaled(64, 64, Qt::KeepAspectRatio));
            break;
        case Cleric:
            health = 560; damage = 20; walkSpeed = 1; attackSpeed = 1; attackRange = 5;
            pixmapPath = ":/resources/images/Cleric.png";
            spriteSheet = QPixmap(pixmapPath);
            if (spriteSheet.isNull()) {
                qDebug() << "Failed to load sprite sheet for Cleric at" << pixmapPath;
                break;
            }
            setPixmap(spriteSheet.copy(0, 0, 16, 16).scaled(64, 64, Qt::KeepAspectRatio));
            break;
    }
    QPixmap currentPixmap = pixmap();
    qDebug() << "Pixmap for type" << type << "size:" << currentPixmap.width() << "x" << currentPixmap.height() << "valid:" << !currentPixmap.isNull();
    if (currentPixmap.isNull()) {
        qDebug() << "Failed to set pixmap for enemy type:" << type << "path:" << pixmapPath;
    } else {
        qDebug() << "Set pixmap for enemy type:" << type << "path:" << pixmapPath;
    }

    attackCooldown = 1.0f;
    lastAttackTime = QTime::currentTime();
    frameWidth = 16;
    frameHeight = 16;
    stateFrameCounts[Idle] = 1; // Disable multi-frame animation for now
    stateFrameCounts[Moving] = 1;
    stateFrameCounts[Attacking] = 1;
    stateFrameCounts[Dying] = 1;
    state = Idle;
    currentFrame = 0;
    animationTimer = 0;
    frameDuration = 0.1f;
}
void Enemy::UpdateAnimation()
{
    // Disable animation for now, use single frame
    // animationTimer += frameDuration;
    // if (animationTimer >= frameDuration) {
    //     currentFrame = (currentFrame + 1) % stateFrameCounts[state];
    //     animationTimer = 0;
    //     // Placeholder for sprite sheet animation
    // }
}

Enemy::Enemy()
    : QGraphicsPixmapItem(), type(Skeleton), health(16), damage(1), walkSpeed(0.8), attackSpeed(1), attackRange(1),
      state(Idle), currentFrame(0), animationTimer(0), frameDuration(0.1f)
{
    setPixmap(QPixmap(":/resources/images/Skeleton.png"));
    attackCooldown = 1.0f;
    lastAttackTime = QTime::currentTime();
    frameWidth = 16;
    frameHeight = 16;
    stateFrameCounts[Idle] = 6;
    stateFrameCounts[Moving] = 6;
    stateFrameCounts[Attacking] = 6;
    stateFrameCounts[Dying] = 6;
}

int Enemy::getHealth() const
{
    return health;
}

void Enemy::setHealth(int newHealth)
{
    health = newHealth;
}

int Enemy::getDamage() const
{
    return damage;
}

void Enemy::setDamage(int newDamage)
{
    damage = newDamage;
}

float Enemy::getSpeed() const
{
    return walkSpeed;
}

EnemyType Enemy::getType() const
{
    return type;
}

void Enemy::takeDamage(int damage)
{
    health -= damage;
    if (health <= 0) {
        setState(Dying);
    }
}

bool Enemy::isAlive() const
{
    return health > 0;
}

void Enemy::attack()
{
    lastAttackTime = QTime::currentTime();
    setState(Attacking);
}

bool Enemy::canAttack()
{
    return lastAttackTime.msecsTo(QTime::currentTime()) >= attackCooldown * 1000;
}

float Enemy::getAttackRange()
{
    return attackRange;
}

void Enemy::setState(EnemyState newState)
{
    if (state != newState) {
        state = newState;
        currentFrame = 0;
        animationTimer = 0;
    }
}



void Enemy::update()
{
    if (!isAlive()) {
        setState(Dying);
        return;
    }
    // Temporary movement toward bottom-right
    QPointF target(448, 448);
    QPointF direction = target - pos();
    qreal length = sqrt(direction.x() * direction.x() + direction.y() * direction.y());
    if (length > 1) {
        direction /= length;
        setPos(pos() + direction * walkSpeed);
        setState(Moving);
    }
    UpdateAnimation();
    qDebug() << "Enemy type" << type << "at" << pos() << "zValue" << zValue();
}
