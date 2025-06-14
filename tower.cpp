#include "tower.h"

Tower::Tower(towerType t, QObject *parent) : QObject(parent), QGraphicsPixmapItem()
{
    setScale(0.5); // Halve the size of each tower on the scene (from 70x130px to 35x65px).
    setOffset(-towerW/2, -towerH); // Make the bottom of the tower the datum.
    type = t;
    QString path = "";

    switch (type) // Initialize base attributes:
    {
    // SUBJECT TO CHANGE!!
        case barricade:
            path = ":/resources/images/towers/archer_tower.png";
            cost = 50; damage = 0; fireRate = 0; range = 1;
        break;
        case melee:
            path = ":/resources/images/towers/archer_tower.png";
            cost = 75; damage = 15; fireRate = 10; range = 2;
        break;
        case archer:
            path = ":/resources/images/towers/archer_tower.png";
            cost = 100; damage = 20; fireRate = 20; range = 5;
        break;
        case fire:
            path = ":/resources/images/towers/archer_tower.png";
            cost = 125; damage = 50; fireRate = 10; range = 3;
        break;
        case wizard:
            path = ":/resources/images/towers/archer_tower.png";
            cost = 150; damage = 100; fireRate = 5; range = 10;
        break;
        default:
            path = ":/resources/images/towers/archer_tower.png";
            cost = 50; damage = 0; fireRate = 0; range = 1;
        break;
    }
    pixmap = new QPixmap(path);

    towerLevel = 0; // Levels: 0, 1, 2
    state = idle;
    animationCounter = 1;
    attackCounter = 1;
    Tick(); // Initialize tower.

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
            default:

            break;
        }
    }
    return balance;

}

void Tower::Tick() // Tick function for tower.
{
    if (state == attacking)
    {
        if (attackCounter%fireRate == 0)
        {
            setPixmap(pixmap->copy(0, 0, 70, 130)); // TEMP: Should be replaced with some sort of attacking tile (perhaps red hue?).
            emit Attack(damage); // Attack all connected enemies.
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
