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
//    drawMap(mapType); // Add tiles to the background of the scene and barriers to the foreground.

    gameView = new QGraphicsView(this);
    gameView->setFixedSize(1920, 1080);
    gameView->setRenderHint(QPainter::Antialiasing);
    gameView->setScene(gameScene);

}

void GameMapDialog::drawMap(int mapType)
{

}
