#include "tower.h"
#include "tile.h"

Tower::Tower(towerType t) : QObject(), QGraphicsPixmapItem()
{
    setScale(0.5); // Halve the size of each tower on the scene (from 70x130px to 35x65px).
    setOffset(-1, -towerH+24); // Adjust the datum for the tower.

    type = t;
    maxHealth = -1; // Default: Tower is unkillable.

    QString path = "";
    switch (type) // Initialize base attributes:
    {
    // SUBJECT TO CHANGE!!
        default:
            [[fallthrough]];
        case barricade:
            path = ":/resources/images/towers/archer_tower.png";
            cost = 50; damage = 5; fireRate = 10; range = 1; piercing = 2; maxHealth = 50;
        break;
        case melee:
            path = ":/resources/images/towers/archer_tower.png";
            cost = 75; damage = 15; fireRate = 10; range = 2; piercing = 10;
        break;
        case archer:
            path = ":/resources/images/towers/archer_tower.png";
            cost = 100; damage = 20; fireRate = 20; range = 3; piercing = 2;
        break;
        case fire:
            path = ":/resources/images/towers/archer_tower.png";
            cost = 125; damage = 50; fireRate = 10; range = 2; piercing = 10;
        break;
        case wizard:
            path = ":/resources/images/towers/archer_tower.png";
            cost = 150; damage = 100; fireRate = 5; range = 5; piercing = 4;
        break;
        case base:
            path = ":/resources/images/towers/archer_tower.png";
            cost = 0; damage = 0; fireRate = 0; range = 0; piercing = 0; maxHealth = 1000;
        break;
    }
    pixmap = new QPixmap(path);

    towerLevel = 0; // Levels: 0, 1, 2
    animationCounter = 1;
    attackCounter = 1;
    health = maxHealth;

    if (maxHealth != -1)
    {
        // Create background bar (grey)
        healthBarBack = new QGraphicsRectItem(this);
        healthBarBack->setRect(0, 0, 40, 5);
        healthBarBack->setBrush(Qt::gray);
        healthBarBack->setZValue(1);

        // Create foreground bar (green)
        healthBarFront = new QGraphicsRectItem(healthBarBack);
        healthBarFront->setRect(0, 0, 40, 5);
        healthBarFront->setBrush(Qt::green);
        healthBarFront->setZValue(2);

        // Position bar above tower
        healthBarBack->setPos(14, -80);
    }
    else
    {
        healthBarBack = nullptr;
        healthBarFront = nullptr;
    }

    Tick(); // Initialize tower.

}

int Tower::getCost()
{
    return cost;
}

int Tower::getRange()
{
    return range;
}

int Tower::Upgrade(int balance) // Input: Currency balance. Output: Balance after upgrade.
{
    if (balance >= cost && towerLevel < 3) // Won't upgrade above 3rd level.
    {
        ++towerLevel;
        balance -= cost; // Remove cost of tower from balance.
        cost += 25; // Increase the cost of next upgrade. SUBJECT TO CHANGE!

        switch (type) // Update attributes:
        {
        // CODE HERE!
            default:
                [[fallthrough]];
            case barricade:

            break;
            case melee:

            break;
            case archer:

            break;
            case fire:

            break;
            case wizard:

            break;
            case base:

            break;
        }
    }
    return balance;

}

void Tower::Damage(int damage) // Damage tower.
{
    if (type == barricade || type == base)
    {
        health -= damage;
    }
}

void Tower::Tick() // Tick function for tower.
{
    if (type == barricade && health <= 0 && tile != nullptr)
    {
        emit destroyTower(tile->row, tile->col);
    }
    else if (type == base && health <= 0)
    {
        // Emit gameLost() signal here!!
    }

    // Update health bar:
    if (maxHealth != -1 && health != maxHealth)
    {
        float ratio = float(health)/maxHealth;
        healthBarFront->setRect(0, 0, 40 * ratio, 5);
    }

    if (fireRate != 0)
    {
        if (attackCounter%fireRate == 0)
        {
            setPixmap(pixmap->copy(0, 0, 70, 130)); // TEMP: Should be replaced with some sort of attacking tile (perhaps red hue?).
            emit Attack(damage, piercing, this); // Attack all connected enemies.
            attackCounter = 0;
        }
        ++attackCounter;
    }

    int row = towerLevel; // Row between 0 and 2.
    int col = animationCounter-1; // Col between 0 and 5.
    setPixmap(pixmap->copy(col*towerW, row*towerH, towerW, towerH));

    if (animationCounter%6 == 0) // Reached end of animation.
    {
        animationCounter = 0;
    }
    ++animationCounter; // Cycle animation.
}
