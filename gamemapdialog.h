#ifndef GAMEMAPDIALOG_H
#define GAMEMAPDIALOG_H

#include <QDialog>
#include <QWidget>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QPixmap>

enum difficulty
{
    easy,medium,hard
};

enum map
{
    map1,map2,map3,random
};

class GameMapDialog : public QDialog
{
public:
    explicit GameMapDialog(QWidget *parent = nullptr);
    // Public access to game settings:
    void setDifficulty(int);
    void setMap(int);
    void setMultiplayer(bool);
    int getDifficulty();
    int getMap();
    bool getMultiplayer();

private:
    // Private game setting variables:
    enum difficulty gameDifficulty; // Current selected difficulty.
    enum map mapType; // Current selected map.
    bool isMultiplayer; // Is game online.

    QGraphicsView *gameView;
    QGraphicsScene *gameScene;
    QPixmap *tileset;

    void drawMap(); // Function to create the game map from prebuilt maps.

};

#endif // GAMEMAPDIALOG_H
