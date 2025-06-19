#include "tower.h"
#include "tile.h"

Tower::Tower(towerType t) : QObject(), QGraphicsPixmapItem()
{
    setScale(0.5); // Halve the size of each tower on the scene (from 70x130px to 35x65px).
    setOffset(-1, -towerH+24); // Adjust the datum for the tower.

    type = t;
    maxHealth = -1; // Default: Tower is unkillable.
    animationFrames = 4; // Defaut value.

    QString path = "";
    switch (type) // Initialize base attributes:
    {
        case barricade:
            animationFrames = 6;
            path = ":/resources/images/towers/barricade_tower.png";
            cost = 50; upgradeCost = 25; damage = 5; attackSpeed = 1; range = 1; piercing = 2; maxHealth = 25;
        break;
        case melee:
            animationFrames = 4;
            path = ":/resources/images/towers/melee_tower.png";
            cost = 75; upgradeCost = 25; damage = 15; attackSpeed = 5; range = 2; piercing = 5;
        break;
        case archer:
            animationFrames = 6;
            path = ":/resources/images/towers/archer_tower.png";
            cost = 100; upgradeCost = 25; damage = 20; attackSpeed = 2; range = 3; piercing = 2;
        break;
        case fire:
            animationFrames = 4;
            path = ":/resources/images/towers/fire_tower.png";
            cost = 125; upgradeCost = 25; damage = 50; attackSpeed = 3; range = 2; piercing = 10;
        break;
        case wizard:
            animationFrames = 4;
            path = ":/resources/images/towers/wizard_tower.png";
            cost = 150; upgradeCost = 25; damage = 100; attackSpeed = 0.8; range = 5; piercing = 5;
        break;
        case base:
            animationFrames = 6;
            path = ":/resources/images/towers/base_tower.png";
            cost = 0; upgradeCost = 300; damage = 0; attackSpeed = 0; range = 0; piercing = 0; maxHealth = 500;
        break;
    }
    pixmap = new QPixmap(path);

    towerLevel = 0; // Levels: 0, 1, 2
    animationCounter = 1;
    attackCooldown = 1; // Reset attack cooldown.
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

        healthBarBack->hide();
        healthBarFront->hide();
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

int Tower::getUpgradeCost()
{
    return upgradeCost;
}

int Tower::getLevel()
{
    return towerLevel;
}

int Tower::getRange()
{
    return range;
}

int Tower::Upgrade(int balance) // Input: Currency balance. Output: Balance after upgrade.
{
    if (balance >= (cost+upgradeCost) && towerLevel < 2) // Won't upgrade above 3rd level.
    {
        ++towerLevel;
        cost += upgradeCost; // Increase the cost of next upgrade. SUBJECT TO CHANGE!
        balance -= cost; // Remove cost of upgraded tower from balance.
        upgradeCost += upgradeCost; // Double next upgrade cost.

        switch (type) // Update attributes:
        {
            case barricade:
                maxHealth += 50;
                health += 50;
                damage += 10; attackSpeed += 1; range += 1; piercing += 2;
            break;
            case melee:
                damage += 10; attackSpeed += 1; range += 1; piercing += 2;
            break;
            case archer:
                damage += 10; attackSpeed += 1; range += 1; piercing += 2;
            break;
            case fire:
                damage += 15; attackSpeed += 1; range += 1; piercing += 2;
            break;
            case wizard:
                damage += 25; attackSpeed += 1; range += 1; piercing += 2;
            break;
            case base:
                maxHealth += 250;
                health += 250;
            break;
        }
    }

    return balance;

}

void Tower::Damage(Enemy* e) // Damage tower.
{
    if (type == barricade || type == base)
    {
        health -= e->getDamage();
        health = (health < 0) ? 0 : health;
        e->setState(Attacking); // Keep attacking tower untill it is destroyed.
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
        emit gameLost();
    }

    // Update health bar:
    if (maxHealth != -1 && health != maxHealth)
    {
        healthBarBack->show();
        healthBarFront->show();
        float ratio = float(health)/maxHealth;
        healthBarFront->setRect(0, 0, 40 * ratio, 5);
    }

    if (attackSpeed != 0)
    {
        attackCooldown = (attackCooldown < 0 || attackCooldown > 1) ? 0 : attackCooldown;
        if (attackCooldown == 0)
        {
            attackCooldown = 1; // Reset attack cooldown.
            emit Attack(damage, piercing, this); // Attack all connected enemies.
        }
        else
            attackCooldown -= attackSpeed/frameRate; // Tower will attack attackSpeed times in one second.
    }

    int row = towerLevel; // Row between 0 and 2.
    int col = animationCounter-1; // Col between 0 and 5.
    setPixmap(pixmap->copy(col*towerW, row*towerH, towerW, towerH));

    if (animationCounter%animationFrames == 0) // Reached end of animation.
    {
        animationCounter = 0;
    }
    ++animationCounter; // Cycle animation.
}
