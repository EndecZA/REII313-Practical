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
#include <QVector>
#include <QFile>
#include <QTimer>
#include <QPointF>
#include <QPair>
#include <QQueue>
#include <QMap>
#include <QSound> // Added for QSound
#include "enemy.h"
#include "tower.h"

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
    QVector<QPointF> findPath(const QPointF& start, const QPointF& target);

private:
    enum difficulty gameDifficulty;
    enum map mapType;
    bool isMultiplayer;
    static const int tileSize = 32;
    static const int mapWidth = 15;
    static const int mapHeight = 30;
    int mapGrid[2*mapHeight][2*mapWidth];
    int barrierGrid[2*mapHeight][2*mapWidth];

    QGraphicsView *gameView;
    QGraphicsScene *gameScene;
    QPixmap *tileset;
    QFile *mapFile;
    QVector<Enemy*> enemies;
    QTimer *waveTimer;
    QTimer *updateTimer;
    int currentWave;
    int enemiesToSpawn;
    int enemiesPerWave;
    int bitcoinCount;
    QGraphicsTextItem *bitcoinText;
    QGraphicsRectItem *bitcoinBackground;
    QGraphicsPixmapItem *bitcoinIcon;
    QGraphicsItemGroup *bitcoinGroup;
    Tower *tower;
    QSound *backgroundSound;

    void spawnEnemy(EnemyType type, const QPointF& pos);
    QVector<QPointF> getSpawnPoints();
    void startNextWave();
    void updateGame();
};

#endif // GAMEMAPDIALOG_H
