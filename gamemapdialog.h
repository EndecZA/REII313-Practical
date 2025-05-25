#ifndef GAMEMAPDIALOG_H
#define GAMEMAPDIALOG_H

#include <QDialog>
#include <QWidget>
#include <QGraphicsView>
#include <QGraphicsScene>

class GameMapDialog : public QDialog
{
public:
    explicit GameMapDialog(QWidget *parent = nullptr);

private:
    QGraphicsView *gameView;
    QGraphicsScene *gameScene;

    void drawMap(int); // Function to create the game map from prebuilt maps.

};

#endif // GAMEMAPDIALOG_H
