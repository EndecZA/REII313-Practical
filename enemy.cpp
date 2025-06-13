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
    QPixmap framePixmap;
    switch (type) {
        case Skeleton: {
            health = 16; damage = 1; walkSpeed = 0.8; attackSpeed = 1; attackRange = 1;
            bitcoinReward = 5; // Bitcoin reward for Skeleton
            pixmapPath = ":/resources/images/Skeleton.png";
            spriteSheet = QPixmap(pixmapPath);
            if (spriteSheet.isNull()) {
                qDebug() << "Failed to load sprite sheet for Skeleton at" << pixmapPath;
            } else {
                int x = 52 - 16; // Center x - half width
                int y = 151 - 11; // Center y - half height
                framePixmap = spriteSheet.copy(x, y, 32, 22); // First frame centered at (52, 151)
                if (framePixmap.isNull()) {
                    qDebug() << "Failed to copy frame for Skeleton from" << pixmapPath << "at" << x << "," << y;
                } else {
                    setPixmap(framePixmap.scaled(32, 32, Qt::KeepAspectRatio));
                }
            }
            break;
        }
        case Skeleton_Archer: {
            health = 16; damage = 1; walkSpeed = 0.8; attackSpeed = 1; attackRange = 5;
            bitcoinReward = 7; // Bitcoin reward for Skeleton_Archer
            pixmapPath = ":/resources/images/Skeleton_Archer.png";
            spriteSheet = QPixmap(pixmapPath);
            if (spriteSheet.isNull()) {
                qDebug() << "Failed to load sprite sheet for Skeleton_Archer at" << pixmapPath;
            } else {
                int x = 52 - 16; // Placeholder center x - half width
                int y = 151 - 11; // Placeholder center y - half height
                framePixmap = spriteSheet.copy(x, y, 32, 22); // First frame (placeholder)
                if (framePixmap.isNull()) {
                    qDebug() << "Failed to copy frame for Skeleton_Archer from" << pixmapPath << "at" << x << "," << y;
                } else {
                    setPixmap(framePixmap.scaled(32, 32, Qt::KeepAspectRatio));
                }
            }
            break;
        }
        case Armoured_Skeleton: {
            health = 64; damage = 4; walkSpeed = 0.8; attackSpeed = 1; attackRange = 1;
            bitcoinReward = 15; // Bitcoin reward for Armoured_Skeleton
            pixmapPath = ":/resources/images/Armored_Skeleton.png";
            spriteSheet = QPixmap(pixmapPath);
            if (spriteSheet.isNull()) {
                qDebug() << "Failed to load sprite sheet for Armoured_Skeleton at" << pixmapPath;
            } else {
                int x = 52 - 16; // Placeholder center x - half width
                int y = 151 - 11; // Placeholder center y - half height
                framePixmap = spriteSheet.copy(x, y, 32, 22); // First frame (placeholder)
                if (framePixmap.isNull()) {
                    qDebug() << "Failed to copy frame for Armoured_Skeleton from" << pixmapPath << "at" << x << "," << y;
                } else {
                    setPixmap(framePixmap.scaled(32, 32, Qt::KeepAspectRatio));
                }
            }
            break;
        }
        case Wizard: {
            health = 80; damage = 3; walkSpeed = 1; attackSpeed = 1; attackRange = 5;
            bitcoinReward = 20; // Bitcoin reward for Wizard
            pixmapPath = ":/resources/images/Wizard.png";
            spriteSheet = QPixmap(pixmapPath);
            if (spriteSheet.isNull()) {
                qDebug() << "Failed to load sprite sheet for Wizard at" << pixmapPath;
            } else {
                int x = 52 - 16; // Placeholder center x - half width
                int y = 151 - 11; // Placeholder center y - half height
                framePixmap = spriteSheet.copy(x, y, 32, 22); // First frame (placeholder)
                if (framePixmap.isNull()) {
                    qDebug() << "Failed to copy frame for Wizard from" << pixmapPath << "at" << x << "," << y;
                } else {
                    setPixmap(framePixmap.scaled(32, 32, Qt::KeepAspectRatio));
                }
            }
            break;
        }
        case Orc: {
            health = 56; damage = 20; walkSpeed = 0.8; attackSpeed = 1.2; attackRange = 1;
            bitcoinReward = 12; // Bitcoin reward for Orc
            pixmapPath = ":/resources/images/Orc.png";
            spriteSheet = QPixmap(pixmapPath);
            if (spriteSheet.isNull()) {
                qDebug() << "Failed to load sprite sheet for Orc at" << pixmapPath;
            } else {
                int x = 52 - 16; // Placeholder center x - half width
                int y = 151 - 11; // Placeholder center y - half height
                framePixmap = spriteSheet.copy(x, y, 32, 22); // First frame (placeholder)
                if (framePixmap.isNull()) {
                    qDebug() << "Failed to copy frame for Orc from" << pixmapPath << "at" << x << "," << y;
                } else {
                    setPixmap(framePixmap.scaled(32, 32, Qt::KeepAspectRatio));
                }
            }
            break;
        }
        case Armoured_Orc: {
            health = 128; damage = 20; walkSpeed = 0.8; attackSpeed = 1; attackRange = 1;
            bitcoinReward = 25; // Bitcoin reward for Armoured_Orc
            pixmapPath = ":/resources/images/Armored_Orc.png";
            spriteSheet = QPixmap(pixmapPath);
            if (spriteSheet.isNull()) {
                qDebug() << "Failed to load sprite sheet for Armoured_Orc at" << pixmapPath;
            } else {
                int x = 52 - 16; // Placeholder center x - half width
                int y = 151 - 11; // Placeholder center y - half height
                framePixmap = spriteSheet.copy(x, y, 32, 22); // First frame (placeholder)
                if (framePixmap.isNull()) {
                    qDebug() << "Failed to copy frame for Armoured_Orc from" << pixmapPath << "at" << x << "," << y;
                } else {
                    setPixmap(framePixmap.scaled(32, 32, Qt::KeepAspectRatio));
                }
            }
            break;
        }
        case Elite_Orc: {
            health = 480; damage = 20; walkSpeed = 1; attackSpeed = 1; attackRange = 1;
            bitcoinReward = 50; // Bitcoin reward for Elite_Orc
            pixmapPath = ":/resources/images/Elite_Orc.png";
            spriteSheet = QPixmap(pixmapPath);
            if (spriteSheet.isNull()) {
                qDebug() << "Failed to load sprite sheet for Elite_Orc at" << pixmapPath;
            } else {
                int x = 52 - 16; // Placeholder center x - half width
                int y = 151 - 11; // Placeholder center y - half height
                framePixmap = spriteSheet.copy(x, y, 32, 22); // First frame (placeholder)
                if (framePixmap.isNull()) {
                    qDebug() << "Failed to copy frame for Elite_Orc from" << pixmapPath << "at" << x << "," << y;
                } else {
                    setPixmap(framePixmap.scaled(32, 32, Qt::KeepAspectRatio));
                }
            }
            break;
        }
        case Orc_rider: {
            health = 48; damage = 10; walkSpeed = 1.5; attackSpeed = 1; attackRange = 1;
            bitcoinReward = 10; // Bitcoin reward for Orc_rider
            pixmapPath = ":/resources/images/Orc_Rider.png";
            spriteSheet = QPixmap(pixmapPath);
            if (spriteSheet.isNull()) {
                qDebug() << "Failed to load sprite sheet for Orc_rider at" << pixmapPath;
            } else {
                int x = 52 - 16; // Placeholder center x - half width
                int y = 151 - 11; // Placeholder center y - half height
                framePixmap = spriteSheet.copy(x, y, 32, 22); // First frame (placeholder)
                if (framePixmap.isNull()) {
                    qDebug() << "Failed to copy frame for Orc_rider from" << pixmapPath << "at" << x << "," << y;
                } else {
                    setPixmap(framePixmap.scaled(32, 32, Qt::KeepAspectRatio));
                }
            }
            break;
        }
        case Knight: {
            health = 240; damage = 15; walkSpeed = 0.8; attackSpeed = 0.8; attackRange = 1;
            bitcoinReward = 30; // Bitcoin reward for Knight
            pixmapPath = ":/resources/images/Knight.png";
            spriteSheet = QPixmap(pixmapPath);
            if (spriteSheet.isNull()) {
                qDebug() << "Failed to load sprite sheet for Knight at" << pixmapPath;
            } else {
                int x = 52 - 16; // Placeholder center x - half width
                int y = 151 - 11; // Placeholder center y - half height
                framePixmap = spriteSheet.copy(x, y, 32, 22); // First frame (placeholder)
                if (framePixmap.isNull()) {
                    qDebug() << "Failed to copy frame for Knight from" << pixmapPath << "at" << x << "," << y;
                } else {
                    setPixmap(framePixmap.scaled(32, 32, Qt::KeepAspectRatio));
                }
            }
            break;
        }
        case Knight_Templar: {
            health = 960; damage = 25; walkSpeed = 0.8; attackSpeed = 0.8; attackRange = 1;
            bitcoinReward = 75; // Bitcoin reward for Knight_Templar
            pixmapPath = ":/resources/images/Knight_Templar.png";
            spriteSheet = QPixmap(pixmapPath);
            if (spriteSheet.isNull()) {
                qDebug() << "Failed to load sprite sheet for Knight_Templar at" << pixmapPath;
            } else {
                int x = 52 - 16; // Placeholder center x - half width
                int y = 151 - 11; // Placeholder center y - half height
                framePixmap = spriteSheet.copy(x, y, 32, 22); // First frame (placeholder)
                if (framePixmap.isNull()) {
                    qDebug() << "Failed to copy frame for Knight_Templar from" << pixmapPath << "at" << x << "," << y;
                } else {
                    setPixmap(framePixmap.scaled(32, 32, Qt::KeepAspectRatio));
                }
            }
            break;
        }
        case Werebear: {
            health = 280; damage = 20; walkSpeed = 1.5; attackSpeed = 1; attackRange = 1;
            bitcoinReward = 35; // Bitcoin reward for Werebear
            pixmapPath = ":/resources/images/Werebear.png";
            spriteSheet = QPixmap(pixmapPath);
            if (spriteSheet.isNull()) {
                qDebug() << "Failed to load sprite sheet for Werebear at" << pixmapPath;
            } else {
                int x = 52 - 16; // Placeholder center x - half width
                int y = 151 - 11; // Placeholder center y - half height
                framePixmap = spriteSheet.copy(x, y, 32, 22); // First frame (placeholder)
                if (framePixmap.isNull()) {
                    qDebug() << "Failed to copy frame for Werebear from" << pixmapPath << "at" << x << "," << y;
                } else {
                    setPixmap(framePixmap.scaled(32, 32, Qt::KeepAspectRatio));
                }
            }
            break;
        }
        case Cleric: {
            health = 560; damage = 20; walkSpeed = 1; attackSpeed = 1; attackRange = 5;
            bitcoinReward = 40; // Bitcoin reward for Cleric
            pixmapPath = ":/resources/images/Cleric.png";
            spriteSheet = QPixmap(pixmapPath);
            if (spriteSheet.isNull()) {
                qDebug() << "Failed to load sprite sheet for Cleric at" << pixmapPath;
            } else {
                int x = 52 - 16; // Placeholder center x - half width
                int y = 151 - 11; // Placeholder center y - half height
                framePixmap = spriteSheet.copy(x, y, 32, 22); // First frame (placeholder)
                if (framePixmap.isNull()) {
                    qDebug() << "Failed to copy frame for Cleric from" << pixmapPath << "at" << x << "," << y;
                } else {
                    setPixmap(framePixmap.scaled(32, 32, Qt::KeepAspectRatio));
                }
            }
            break;
        }
    }
    QPixmap currentPixmap = pixmap();
    qDebug() << "Final pixmap for type" << type << "size:" << currentPixmap.width() << "x" << currentPixmap.height() << "valid:" << !currentPixmap.isNull();
    if (currentPixmap.isNull()) {
        qDebug() << "Final pixmap is null for enemy type:" << type << "path:" << pixmapPath;
    } else {
        qDebug() << "Final pixmap set for enemy type:" << type << "path:" << pixmapPath;
    }

    if (pixmap().isNull()) {
        qDebug() << "Using fallback pixmap for type" << type;
        QPixmap fallback(32, 32); // Match new size
        fallback.fill(Qt::red); // Red square as fallback
        setPixmap(fallback);
    }

    attackCooldown = 1.0f;
    lastAttackTime = QTime::currentTime();
    frameWidth = 32; // Match actual frame width
    frameHeight = 22; // Match actual frame height
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
     animationTimer += frameDuration;
     if (animationTimer >= frameDuration) {
         currentFrame = (currentFrame + 1) % stateFrameCounts[state];
         animationTimer = 0;
         // Placeholder for sprite sheet animation
     }
}

Enemy::Enemy()
    : QGraphicsPixmapItem(), type(Skeleton), health(16), damage(1), walkSpeed(0.8), attackSpeed(1), attackRange(1),
      bitcoinReward(5), state(Idle), currentFrame(0), animationTimer(0), frameDuration(0.1f)
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

int Enemy::getBitcoinReward() const
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
