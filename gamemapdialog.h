#ifndef GAMEMAPDIALOG_H
#define GAMEMAPDIALOG_H

#include <QDialog>
#include <QWidget>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QPixmap>
#include <QVector>
#include <QFile>

enum difficulty
{
    easy,medium,hard
};

enum map
{
    map1,map2,map3
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
    void drawMap(); // Function to create the game map from prebuilt maps.

private:
    // Private game setting variables:
    enum difficulty gameDifficulty; // Current selected difficulty.
    enum map mapType; // Current selected map.
    bool isMultiplayer; // Is game online.
    static const int tileSize = 32; // Tile height and width in pixels.
    static const int mapWidth = 15; // Amount of tiles horizontally.
    static const int mapHeight = 30; // Amount of tiles vertically.
    int mapGrid[2*mapHeight][2*mapWidth]; // Grid to store map and barriers.
    int barrierGrid[2*mapHeight][2*mapWidth]; // Grid to store barrier locations for pathfinding.

    QGraphicsView *gameView;
    QGraphicsScene *gameScene;
    QPixmap *tileset;
    QFile *mapFile;

    void genMap(); // Function to generate a random map.

};

#endif // GAMEMAPDIALOG_H
