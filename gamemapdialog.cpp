#include "gamemapdialog.h"

GameMapDialog::GameMapDialog(QWidget *parent)
    : QDialog(parent)
{   
    showMaximized();
    setFixedSize(1920,1080);
    setWindowTitle("Game");

    gameScene = new QGraphicsScene(this);
    gameScene->setSceneRect(0, 0, 1920, 1920);
//    gameScene->setBackgroundBrush(QColor("#FFB347")); // Set background color of the scene.
    gameDifficulty = medium;
    mapType = map1;
    isMultiplayer = false;
    drawMap(); // Add tiles to the background of the scene and barriers to the foreground.

    gameView = new QGraphicsView(this);
    gameView->setFixedSize(1920, 1080);
    gameView->setRenderHint(QPainter::Antialiasing);
    gameView->setScene(gameScene);

}
//WORK IN PROGRESS
//void GameMapDialog::spawnEnemy(EnemyType type, const QPointF& pos)
//{
//    Enemy* enemy = new Enemy(type, pos, this);
//    switch (gameDifficulty) {
//        case easy:
//            enemy->setHealth(enemy->getHealth() * 0.8);
//            break;
//        case medium:
//            // Default stats
//            break;
//        case hard:
//            enemy->setHealth(enemy->getHealth() * 1.5);
//            enemy->setDamage(enemy->getDamage() * 1.2);
//            break;
//    }
//    enemies.append(enemy);
//    gameScene->addItem(enemy);
//}

void GameMapDialog::setDifficulty(int dif)
{
    switch (dif)
    {
        case 0:
            gameDifficulty = easy;
        break;
        case 1:
            gameDifficulty = medium;
        break;
        case 2:
            gameDifficulty = hard;
        break;
        default:
            gameDifficulty = medium;
        break;
    }
}

void GameMapDialog::setMap(int map)
{
    switch (map)
    {
        case 0:
            mapType = map1;
        break;
        case 1:
            mapType = map2;
        break;
        case 2:
            mapType = map3;
        break;
        case 3:
            mapType = random;
        break;
        default:
            mapType = map1;
        break;
    }
}

void GameMapDialog::setMultiplayer(bool multi)
{
    isMultiplayer = multi;
}

int GameMapDialog::getDifficulty()
{
    return (int)gameDifficulty;
}

int GameMapDialog::getMap()
{
    return (int)mapType;
}

bool GameMapDialog::getMultiplayer()
{
    return gameDifficulty;
}

void GameMapDialog::drawMap()
{
    switch (mapType)
    {
        case map1:

        break;
        case map2:

        break;
        case map3:

        break;
        case random:

        break;
    }
}
