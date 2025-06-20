#include "enemy.h"
#include <QDebug>

const double PI = std::atan(1.0) * 4; // Calculate Pi.

Enemy::Enemy(EnemyType t) : QObject(), QGraphicsPixmapItem(), justLoaded(false)
{
    setOffset(-spriteSize/2 + 16, -spriteSize/2 + 4); // Adjust the datum for the enemy.
    hide();

    // Initialize Parameters:
    type = t;
    source[0] = 0;
    source[1] = 0;
    dest[0] = 0;
    dest[1] = 0;
    isMirrored = false;

    moveProgress = 0;
    attackCooldown = 1;

    state = Idle;
    prevState = Idle;
    animationCounter[Idle] = 1;
    animationCounter[Moving] = 1;
    animationCounter[Attacking] = 1;
    animationCounter[Damaged] = 1;
    animationCounter[Dying] = 1;

    QString pixmapPath;
    switch (type) {
        default:
            [[fallthrough]];
        case Skeleton: {

            health = 30; damage = 1; walkSpeed = 1; attackSpeed = 1; attackRange = 1;
            bitcoinReward = 5;

            animationFrames[Moving] = 8;
            animationFrames[Attacking] = 6;
            animationFrames[Damaged] = 4;
            animationFrames[Dying] = 4;

            pixmapPath = ":/resources/images/Skeleton.png";
            break;
        }
        case Skeleton_Archer: {

            health = 30; damage = 1; walkSpeed = 1; attackSpeed = 1; attackRange = 5;
            bitcoinReward = 10;

            animationFrames[Moving] = 8;
            animationFrames[Attacking] = 9;
            animationFrames[Damaged] = 4;
            animationFrames[Dying] = 4;

            pixmapPath = ":/resources/images/Skeleton_Archer.png";
            break;
        }
        case Armoured_Skeleton: {
            health = 64; damage = 4; walkSpeed = 1; attackSpeed = 1; attackRange = 1;
            bitcoinReward = 10;

            animationFrames[Moving] = 8;
            animationFrames[Attacking] = 8;
            animationFrames[Damaged] = 4;
            animationFrames[Dying] = 4;

            pixmapPath = ":/resources/images/Armored_Skeleton.png";
            break;
        }
        case Wizard: {
            health = 50; damage = 30; walkSpeed = 1.5; attackSpeed = 0.8; attackRange = 10;
            bitcoinReward = 20;

            animationFrames[Moving] = 8;
            animationFrames[Attacking] = 15;
            animationFrames[Damaged] = 4;
            animationFrames[Dying] = 4;

            pixmapPath = ":/resources/images/Wizard.png";
            break;
        }
        case Orc: {
            health = 56; damage = 20; walkSpeed = 1; attackSpeed = 1.5; attackRange = 1;
            bitcoinReward = 15;

            animationFrames[Moving] = 8;
            animationFrames[Attacking] = 6;
            animationFrames[Damaged] = 4;
            animationFrames[Dying] = 4;

            pixmapPath = ":/resources/images/Orc.png";
            break;
        }
        case Armoured_Orc: {
            health = 128; damage = 20; walkSpeed = 1; attackSpeed = 1.2; attackRange = 1;
            bitcoinReward = 20;

            animationFrames[Moving] = 8;
            animationFrames[Attacking] = 7;
            animationFrames[Damaged] = 4;
            animationFrames[Dying] = 4;

            pixmapPath = ":/resources/images/Armored_Orc.png";
            break;
        }
        case Elite_Orc: {
            health = 480; damage = 20; walkSpeed = 1.5; attackSpeed = 1; attackRange = 1;
            bitcoinReward = 50;

            animationFrames[Moving] = 8;
            animationFrames[Attacking] = 7;
            animationFrames[Damaged] = 4;
            animationFrames[Dying] = 4;

            pixmapPath = ":/resources/images/Elite_Orc.png";
            break;
        }
        case Orcastor: {
            health = 80; damage = 15; walkSpeed = 2; attackSpeed = 1.5; attackRange = 2;
            bitcoinReward = 30;

            animationFrames[Moving] = 8;
            animationFrames[Attacking] = 8;
            animationFrames[Damaged] = 4;
            animationFrames[Dying] = 4;

            pixmapPath = ":/resources/images/Orc_Rider.png";
            break;
        }
        case Knight: {
            health = 240; damage = 30; walkSpeed = 1; attackSpeed = 0.8; attackRange = 2;
            bitcoinReward = 40;

            animationFrames[Moving] = 8;
            animationFrames[Attacking] = 7;
            animationFrames[Damaged] = 4;
            animationFrames[Dying] = 4;

            pixmapPath = ":/resources/images/Knight.png";
            break;
        }
        case Knight_Templar: {
            health = 960; damage = 40; walkSpeed = 1; attackSpeed = 0.8; attackRange = 2;
            bitcoinReward = 80;

            animationFrames[Moving] = 8;
            animationFrames[Attacking] = 7;
            animationFrames[Damaged] = 4;
            animationFrames[Dying] = 4;

            pixmapPath = ":/resources/images/Knight_Templar.png";
            break;
        }
        case Werebear: {
            health = 280; damage = 20; walkSpeed = 2; attackSpeed = 2; attackRange = 1;
            bitcoinReward = 50;

            animationFrames[Moving] = 8;
            animationFrames[Attacking] = 9;
            animationFrames[Damaged] = 4;
            animationFrames[Dying] = 4;

            pixmapPath = ":/resources/images/Werebear.png";
            break;
        }
        case Cleric: {
            health = 560; damage = 15; walkSpeed = 1.5; attackSpeed = 1; attackRange = 5;
            bitcoinReward = 50;

            animationFrames[Moving] = 8;
            animationFrames[Attacking] = 9;
            animationFrames[Damaged] = 4;
            animationFrames[Dying] = 4;

            pixmapPath = ":/resources/images/Cleric.png";
            break;
        }
    }
    animationFrames[Idle] = 6;

    spriteSheet = new QPixmap(pixmapPath);
    if (!spriteSheet->isNull()) {
        setPixmap(spriteSheet->copy(0, 0, spriteSize, spriteSize));
    }
    else
        qDebug() << "Invalid sprite sheet added.";

    if (pixmap().isNull()) {
        qDebug() << "Pixmap null for type:" << type;
        QPixmap fallback(32, 32);
        fallback.fill(Qt::red);
        setPixmap(fallback);
    }

    setTransformOriginPoint(boundingRect().center());
}

int Enemy::getHealth()
{
    return health;
}

int Enemy::getDamage()
{
    return damage;
}

int Enemy::getRange()
{
    return attackRange;
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
    if (state != Damaged)
        setState(Damaged);

    health -= damage;
}

void Enemy::setState(EnemyState newState)
{
    prevState = state;

    if (prevState != newState && prevState != Dying) {
        state = newState;
        animationCounter[state] = 1; // Reset animation.
        attackCooldown = 1; // Reset attack cooldown.
    }
}

EnemyState Enemy::getState()
{
    return state;
}

void Enemy::setJustLoaded(bool value) {
    justLoaded = value;
}

bool Enemy::isJustLoaded() const {
    return justLoaded;
}

// Tick function for handling animation, movement and actions:
void Enemy::Tick()
{
    int row = 0;
    int col = animationCounter[state] - 1;

    switch (state)
    {
        case Idle:
            row = 0;
        break;
        case Moving:
        {
            row = 1;

            // Move enemy:
            moveProgress = (moveProgress < 0 || moveProgress > 1) ? 1 : moveProgress; // Make sure the movement is bounded.
            float f = (1 - cos(PI * moveProgress)) / 2; // Interpolation function for ease-in ease-out movement.
            float x = (1-f)*source[0] + f*dest[0];
            float y = (1-f)*source[1] + f*dest[1];
            setPos(x, y);
            setZValue(y + 8); // Ensure that enemy is drawn at correct ZValue.

            if (moveProgress == 1)
            {
                setState(Idle); // Wait until enemy is successfully moved.
                moveProgress = 0; // Reset movement.
                emit moveEnemy(this);
                show();
            }
            else
                moveProgress += walkSpeed/frameRate; // Enemy will move by walkSpeed amount of tiles in one second.
        }
        break;
        case Attacking:
        {
            if (type == Knight_Templar)
                row = 3; // Special row for Knight Templar attacking animation.
            else
                row = 2;

            attackCooldown = (attackCooldown < 0 || attackCooldown > 1) ? 0 : attackCooldown;
            if (attackCooldown == 0)
            {
                setState(Idle);
                attackCooldown = 1; // Reset attack cooldown.
                emit Attack(this); // Emit attacking signal.
            }
            else
                attackCooldown -= attackSpeed/frameRate; // Enemy will attack attackSpeed times in one second.
        }
        break;
        case Damaged:
        {
            row = spriteSheet->height()/spriteSize - 2; // Second to last row.

            // Move enemy while being damaged:
            moveProgress = (moveProgress < 0 || moveProgress > 1) ? 1 : moveProgress; // Make sure the movement is bounded.
            float f = (1 - cos(PI * moveProgress)) / 2; // Interpolation function for ease-in ease-out movement.
            float x = (1-f)*source[0] + f*dest[0];
            float y = (1-f)*source[1] + f*dest[1];
            setPos(x, y);
            setZValue(y + 8); // Ensure that enemy is drawn at correct ZValue.

            moveProgress += walkSpeed/frameRate; // Enemy will move by walkSpeed amount of tiles in one second.
        }
        break;
        case Dying:
            row = spriteSheet->height()/spriteSize - 1; // Last row.
        break;
    }

    // Set pixmap based on animation frame:
    QPixmap pixmap = spriteSheet->copy(col*spriteSize, row*spriteSize, spriteSize, spriteSize);
    if (isMirrored)
        pixmap = QPixmap::fromImage(pixmap.toImage().mirrored(true, false)); // Mirror pixmap.
    setPixmap(pixmap);

    if (animationCounter[state] % animationFrames[state] == 0) // Reached end of animation.
    {
        animationCounter[state] = 0; // Loop back to start of animation.

        if (state == Damaged)
        {
            if (health <= 0)
                setState(Dying); // Kill enemy.
            else
                setState(prevState); // Go back to previous state before enemy was damaged.
        }
        else if (state == Dying)
        {
            emit killEnemy(this);
            hide();
        }
        else if (state == Idle)
        {
            setState(Moving); // Try to move after being idle.
        }
    }
    ++animationCounter[state]; // Cycle animation.
}

void Enemy::setDest(int x, int y) // Set new destination position.
{
    // Correctly orient enemy:
    if (dest[0] > x)
        isMirrored = true;
    else if (dest[0] < x)
        isMirrored = false;

    // Make the old destination the new source:
    if (source[0] == 0 && source[1] == 0)
    {
        source[0] = x;
        source[1] = y;
    }
    else
    {
        source[0] = dest[0];
        source[1] = dest[1];
    }

    // Add new destination:
    dest[0] = x;
    dest[1] = y;

    setState(Moving);
}
