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
#include <QGraphicsProxyWidget>
#include <QPushButton>
#include <QPixmap>
#include <QFile>
#include <QTimer>
#include <QQueue>
#include <QSound>
#include <QKeyEvent>
#include <QTcpServer>
#include <QTcpSocket>

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
    void updateWaveDisplay();

    enum difficulty gameDifficulty;
    enum map mapType;
    bool isMultiplayer;

    QString getLocalIp();

    void setupNetworkAsHost();
    void setupNetworkAsClient(const QString& hostAddress);

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

    void onNewConnection();
    void readClientData();
    void sendGameUpdate();

private slots:
    void startWave();
    void spawnEnemy();

private:
    static const int tileSize = 32;
    static const int mapWidth = 15;
    static const int mapHeight = 30;
    static const int frameRate = 8; // Framerate in FPS.
    static const int startingBitcoins = 300;

    QGraphicsView *gameView;
    QGraphicsScene *gameScene;
    QFile *mapFile;

    QVector<Enemy*> enemies;
    QVector<Tower*> towers;
    QQueue<Tile*> spawnPoints; // Queue used to cycle the spawning of enemies.
    QQueue<EnemyType> waveEnemies; // Queue used to cycle over the types of enemies in each wave.

    int mapGrid[2*mapHeight][2*mapWidth];
    int barrierGrid[2*mapHeight][2*mapWidth];
    Tile *tileGrid[2*mapHeight][2*mapWidth];
    int baseRow, baseCol; // Indices for base position.

    QTimer *gameTick;
    QTimer *enemyTick;
    QTimer *enemySpawnTimer;

    int currentEnemyIndex;

    int currentWave;
    int enemiesToSpawn;
    int totalEnemiesPerWave;
    int bitcoinCount;

    QGraphicsTextItem *bitcoinText;
    QGraphicsTextItem *waveText;
    QGraphicsRectItem *bitcoinBackground;
    QGraphicsPixmapItem *bitcoinIcon;
    QGraphicsItemGroup *bitcoinGroup;
    QGraphicsProxyWidget *buttonProxy;
    QPushButton *nextWaveButton;
    PauseMenuDialog *pauseMenu;

    int savedBitcoinCount; // Added to keep track of saved bitcoin

    void updateGame();
    void tickEnemies();
    void incrementWave();
    void pauseGame();
    void resumeGame();

    QTcpServer *tcpServer;
    QTcpSocket *clientSocket;
    QTimer *networkUpdateTimer;

};

#endif // GAMEMAPDIALOG_H
