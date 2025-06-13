#include "enemy.h"
#include "gamemapdialog.h"
#include <QDebug>
#include <QtMath>

Enemy::Enemy(EnemyType type, const QPointF& position, GameMapDialog* mapDialog)
    : type(type), mapDialog(mapDialog)
{
    setPos(position);
    QString pixmapPath;
    QPixmap spriteSheet;
    QPixmap framePixmap;
    switch (type) {
        case Skeleton: {
            health = 16; damage = 1; walkSpeed = 0.8; attackSpeed = 1; attackRange = 1;
            bitcoinReward = 5;
            pixmapPath = ":/resources/images/Skeleton.png";
            break;
        }
        case Skeleton_Archer: {
            health = 16; damage = 1; walkSpeed = 0.8; attackSpeed = 1; attackRange = 5;
            bitcoinReward = 7;
            pixmapPath = ":/resources/images/Skeleton_Archer.png";
            break;
        }
        case Armoured_Skeleton: {
            health = 64; damage = 4; walkSpeed = 0.8; attackSpeed = 1; attackRange = 1;
            bitcoinReward = 15;
            pixmapPath = ":/resources/images/Armored_Skeleton.png";
            break;
        }
        case Wizard: {
            health = 80; damage = 3; walkSpeed = 1; attackSpeed = 1; attackRange = 5;
            bitcoinReward = 20;
            pixmapPath = ":/resources/images/Wizard.png";
            break;
        }
        case Orc: {
            health = 56; damage = 20; walkSpeed = 0.8; attackSpeed = 1.2; attackRange = 1;
            bitcoinReward = 12;
            pixmapPath = ":/resources/images/Orc.png";
            break;
        }
        case Armoured_Orc: {
            health = 128; damage = 20; walkSpeed = 0.8; attackSpeed = 1; attackRange = 1;
            bitcoinReward = 25;
            pixmapPath = ":/resources/images/Armored_Orc.png";
            break;
        }
        case Elite_Orc: {
            health = 480; damage = 20; walkSpeed = 1; attackSpeed = 1; attackRange = 1;
            bitcoinReward = 50;
            pixmapPath = ":/resources/images/Elite_Orc.png";
            break;
        }
        case Orcastor: {
            health = 48; damage = 10; walkSpeed = 1.5; attackSpeed = 1; attackRange = 1;
            bitcoinReward = 10;
            pixmapPath = ":/resources/images/Orc_Rider.png";
            break;
        }
        case Knight: {
            health = 240; damage = 15; walkSpeed = 0.8; attackSpeed = 0.8; attackRange = 1;
            bitcoinReward = 30;
            pixmapPath = ":/resources/images/Knight.png";
            break;
        }
        case Knight_Templar: {
            health = 960; damage = 25; walkSpeed = 0.8; attackSpeed = 0.8; attackRange = 1;
            bitcoinReward = 75;
            pixmapPath = ":/resources/images/Knight_Templar.png";
            break;
        }
        case Werebear: {
            health = 280; damage = 20; walkSpeed = 1.5; attackSpeed = 1; attackRange = 1;
            bitcoinReward = 35;
            pixmapPath = ":/resources/images/Werebear.png";
            break;
        }
        case Cleric: {
            health = 560; damage = 20; walkSpeed = 1; attackSpeed = 1; attackRange = 5;
            bitcoinReward = 40;
            pixmapPath = ":/resources/images/Cleric.png";
            break;
        }
    }
    spriteSheet = QPixmap(pixmapPath);
    if (!spriteSheet.isNull()) {
        int x = 52 - 16;
        int y = 151 - 11;
        framePixmap = spriteSheet.copy(x, y, 32, 22);
        setPixmap(framePixmap.scaled(32, 32, Qt::KeepAspectRatio));
    }
    QPixmap currentPixmap = pixmap();
    if (currentPixmap.isNull()) {
        qDebug() << "Pixmap null for type:" << type;
        QPixmap fallback(32, 32);
        fallback.fill(Qt::red);
        setPixmap(fallback);
    }

    attackCooldown = 1.0f;
    lastAttackTime = QTime::currentTime();
    frameWidth = 16;
    frameHeight = 16;
    stateFrameCounts[Idle] = 6;
    stateFrameCounts[Moving] = 6;
    stateFrameCounts[Attacking] = 6;
    stateFrameCounts[Dying] = 6;
    state = Idle;
    currentFrame = 0;
    animationTimer = 0;
    frameDuration = 0.1f;
    target = QPointF(448, 448);
}

Enemy::Enemy()
    : type(Skeleton), health(16), damage(1), walkSpeed(0.8), attackSpeed(1), attackRange(1),
      bitcoinReward(5), state(Idle), currentFrame(0), animationTimer(0), frameDuration(0.1f),
      mapDialog(nullptr)
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
    target = QPointF(448, 448);
}

Enemy::~Enemy()
{
}

void Enemy::UpdateAnimation()
{
    animationTimer += frameDuration;
    if (animationTimer >= frameDuration) {
        currentFrame = (currentFrame + 1) % stateFrameCounts[state];
        animationTimer = 0;
    }
}

int Enemy::getHealth()
{
    return health;
}

void Enemy::setHealth(int newHealth)
{
    health = newHealth;
}

int Enemy::getDamage()
{
    return damage;
}

void Enemy::setDamage(int newDamage)
{
    damage = newDamage;
}

float Enemy::getSpeed()
{
    return walkSpeed;
}

EnemyType Enemy::getType()
{
    return type;
}

int Enemy::getBitcoinReward()
{
    return bitcoinReward;
}

void Enemy::takeDamage(int damage)
{
    health -= damage;
    if (health <= 0) {
        setState(Dying);
    }
}

bool Enemy::isAlive()
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

    if (!mapDialog) {
        qDebug() << "Enemy at" << pos() << "type" << type << "no mapDialog";
        setState(Idle);
        return;
    }

    // Get next step from findPath
    QVector<QPointF> nextStep = mapDialog->findPath(pos(), target);
    if (nextStep.isEmpty()) {
        qDebug() << "Enemy at" << pos() << "type" << type << "no valid next step to" << target;
        setState(Idle);
        return;
    }

    QPointF nextPoint = nextStep[0];
    if (nextPoint == pos()) {
        qDebug() << "Enemy at" << pos() << "type" << type << "staying put";
        setState(Idle);
    } else {
        // Move to next tile's center
        setPos(nextPoint);
        setState(Moving);
    }

    UpdateAnimation();
    qDebug() << "Enemy type" << type << "at" << pos() << "zValue" << zValue() << "moved to" << nextPoint;
}
