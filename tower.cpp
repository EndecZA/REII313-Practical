#include "tower.h"

Tower::Tower(towerType t)
{
    type = t;

    switch (type)
    {
        case barricade:
            cost = 50;
            damage = 0;
            fireRate = 0;
            range = 1;
        break;
        case melee:
            cost = 75;
            damage = 15;
            fireRate = 10;
            range = 2;
        break;
        case archer:
            cost = 100;
            damage = 20;
            fireRate = 20;
            range = 5;
        break;
        case fire:
            cost = 125;
            damage = 50;
            fireRate = 10;
            range = 3;
        break;
        case wizard:
            cost = 150;
            damage = 100;
            fireRate = 5;
            range = 10;
        break;
    }

    towerLevel = 1;
    state = idle;
    counter = 1;
    Tick();

}

int Tower::Upgrade(int balance) // Input: Currency balance. Output: Balance after upgrade.
{
    if (balance >= cost && towerLevel < 4)
    {
        ++towerLevel;
        balance -= cost;
        cost += 25;
        switch (type) // Update attributes:
        {
            case barricade:

            break;
            case melee:
                damage += 10;
                fireRate += 10;
                range += 1;
            break;
            case archer:
                damage += 10;
                fireRate += 10;
                range += 1;
            break;
            case fire:
                damage += 10;
                fireRate += 10;
                range += 1;
            break;
            case wizard:
                damage += 10;
                fireRate += 10;
                range += 1;
            break;
        }
    }

    return balance;
}

void Tower::Tick() // Tick function for tower.
{
    if (state == idle)
    {
        // Play idle animation.
    }
    else
    {
        if (counter%fireRate == 0)
        {
            emit Attack(damage); // Attack all connected enemies.
            counter = 0;
        }
        ++counter;
    }

}
