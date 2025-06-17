#ifndef GAMEMAPDIALOG_H
#define GAMEMAPDIALOG_H

#include <QDialog>
#include <QWidget>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsTextItem>
#include <QGraphicsRectItem>
#include <QGraphicsPixmapItem>
#include <QGraphicsItemGroup>
#include <QPixmap>
#include <QFile>
#include <QTimer>
#include <QQueue>
#include <QSound>
#include <QKeyEvent>

#include "tile.h"
#include "enemy.h"
#include "tower.h"
#include "pausemenudialog.h"

enum difficulty
{
    easy, medium, hard
};

enum map
{
    map1, map2, map3
};

class GameMapDialog : public QDialog
{
public:
    explicit GameMapDialog(QWidget *parent = nullptr);
    void setDifficulty(int);
    void setMap(int);
    void setMultiplayer(bool);
    int getDifficulty();
    int getMap();
    bool getMultiplayer();
    void drawMap();
    void updateBitcoinDisplay();
    enum difficulty gameDifficulty;
    enum map mapType;
    bool isMultiplayer;
    static const int tileSize = 32;
    static const int mapWidth = 15;
    static const int mapHeight = 30;
    static const int frameRate = 8; // Framerate in FPS.


protected:
    void keyPressEvent(QKeyEvent *event) override;

private slots:
    void onResumeGame();
    void onSaveGame();
    void onExitGame();
    void floodFill(); // Populate tiles with distance prameters and next tile pointers.
    void buildTower(towerType, int row, int col); // Build tower at tile that sent the signal.
    void sellTower(int row, int col); // Sell tower at tile that sent the signal.
    void upgradeTower(int row, int col); // Upgrade tower at tile that sent the signal.
    void killEnemy(Enemy*);

private:
    QGraphicsView *gameView;
    QGraphicsScene *gameScene;
    QFile *mapFile;

    QVector<Enemy*> enemies;
    QVector<Tower*> towers;

    int mapGrid[2*mapHeight][2*mapWidth];
    int barrierGrid[2*mapHeight][2*mapWidth];
    Tile *tileGrid[2*mapHeight][2*mapWidth];


//    QTimer *waveTimer;
    QTimer *updateTimer;

    int currentWave;
    int enemiesToSpawn;
    int enemiesPerWave;
    int bitcoinCount;
    int baseRow, baseCol; // Indices for base position.
    int spawnRow, spawnCol; // Indices for spawn position.

    QGraphicsTextItem *bitcoinText;
    QGraphicsRectItem *bitcoinBackground;
    QGraphicsPixmapItem *bitcoinIcon;
    QGraphicsItemGroup *bitcoinGroup;
    PauseMenuDialog *pauseMenu;

//    void spawnEnemy(EnemyType type, const QPointF& pos);
//    void startNextWave();
    void updateGame();
    void pauseGame();
    void resumeGame();
};

#endif // GAMEMAPDIALOG_H
