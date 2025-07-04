#ifndef GAMEMAPDIALOG_H
#define GAMEMAPDIALOG_H

#include <QDialog>
#include <QWidget>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsTextItem>
#include <QGraphicsRectItem>
#include <QGraphicsLineItem>
#include <QPen>
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
    ~GameMapDialog();

    void clearGameState();
    bool saveGameToFile(const QString& filename);
    bool loadGameFromFile(const QString& filename);
    void setDifficulty(int);
    void setMap(int);
    void setMultiplayer(bool);
    int getDifficulty();
    int getMap();
    bool getMultiplayer();
    void drawMap();

    void updateBitcoinDisplay();
    void updateWaveDisplay();


    void spawnWave();

    enum difficulty gameDifficulty;
    enum map mapType;
    bool isMultiplayer;

protected:
    void keyPressEvent(QKeyEvent *event) override;

public slots:
    void onResumeGame();
    void onSaveGame();
    void onExitGame();
    void floodFill(); // Populate tiles with distance parameters and next tile pointers.
    void buildTower(towerType, int row, int col); // Build tower at tile that sent the signal.
    void sellTower(int row, int col); // Sell tower at tile that sent the signal.
    void upgradeTower(int row, int col); // Upgrade tower at tile that sent the signal.
    void destroyTower(int row, int col);
    void killEnemy(Enemy*);
    void attackAnimation(Tower*, Enemy*);
    bool loadGameFromFile(const QString& filename);
    bool saveGameToFile(const QString& filename);
    void cleanState();


private:
    static const int tileSize = 32;
    static const int mapWidth = 15;
    static const int mapHeight = 30;
    static const int frameRate = 8; // Framerate in FPS.

    QGraphicsView *gameView;
    QGraphicsScene *gameScene;
    QFile *mapFile;

    QVector<Enemy*> enemies;
    QVector<Tower*> towers;
    QVector<EnemyType> waveEnemies;

    int mapGrid[2*mapHeight][2*mapWidth];
    int barrierGrid[2*mapHeight][2*mapWidth];
    Tile *tileGrid[2*mapHeight][2*mapWidth];
    QQueue<Tile*> spawnPoints; // Queue used to cycle the spawning of enemies.

    QTimer *gameTick;
    QTimer *enemyTick;
    QTimer *waveTimer;
    QTimer *enemySpawnTimer;

    int currentEnemyIndex;

    int currentWave;
    int enemiesToSpawn;
    int totalEnemiesPerWave;
    int bitcoinCount;
    int baseRow, baseCol; // Indices for base position.

    QGraphicsTextItem *bitcoinText;
    QGraphicsTextItem *waveText;
    QGraphicsRectItem *bitcoinBackground;
    QGraphicsPixmapItem *bitcoinIcon;
    QGraphicsItemGroup *bitcoinGroup;
    PauseMenuDialog *pauseMenu;

    int savedBitcoinCount; // Added to keep track of saved bitcoin

    void updateGame();
    void tickEnemies();
    void pauseGame();
    void resumeGame();
};

#endif // GAMEMAPDIALOG_H
