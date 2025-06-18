#include "mainwindow.h"
#include "singleplayeroptionsdialog.h"
#include "multiplayeroptionsdialog.h"
#include "gamemapdialog.h"

#include <QPixmap>
#include <QFontDatabase>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setFixedSize(800, 600);
    setMouseTracking(true);

    // Load custom font (Press Start 2P)
    QFontDatabase().addApplicationFont(":/resources/fonts/PressStart2P-Regular.ttf");

    // Initialize background music with QSound
    backgroundSound = new QSound(":/resources/audio/audio.wav", this);
    backgroundSound->setLoops(-1); // Infinite looping
    backgroundSound->play();

    // Background QLabel
    Background = new QLabel(this);
    Background->setGeometry(0, 0, 800, 600);
    QPixmap pixmap(":/resources/images/Logo.png");
    Background->setPixmap(pixmap);
    Background->setScaledContents(true);
    Background->lower();

    // Singleplayer button
    SinglePlayerBtn = new QPushButton(this);
    SinglePlayerBtn->setFixedSize(200, 75);
    SinglePlayerBtn->setText("Singleplayer");
    SinglePlayerBtn->move(475, 300);
    SinglePlayerBtn->setStyleSheet(
        "QPushButton {"
        "   background-color: #FFB347;"
        "   color: black;"
        "   border: 1px solid black;"
        "   border-radius: 5px;"
        "   font-family: 'Press Start 2P';"
        "   font-size: 15px;"
        "}"
        "QPushButton:hover {"
        "   background-color: #FF8C00;"
        "}");
    connect(SinglePlayerBtn, &QPushButton::clicked, this, &MainWindow::onSinglePlayerClicked);

    // Multiplayer button
    MultiPlayerBtn = new QPushButton(this);
    MultiPlayerBtn->setFixedSize(200, 75);
    MultiPlayerBtn->setText("Multiplayer");
    MultiPlayerBtn->move(475, 400);
    MultiPlayerBtn->setStyleSheet(
        "QPushButton {"
        "   background-color: #FFB347;"
        "   color: black;"
        "   border: 1px solid black;"
        "   border-radius: 5px;"
        "   font-family: 'Press Start 2P';"
        "   font-size: 15px;"
        "}"
        "QPushButton:hover {"
        "   background-color: #FF8C00;"
        "}");
    connect(MultiPlayerBtn, &QPushButton::clicked, this, &MainWindow::onMultiPlayerClicked);

    // Initialize Public Variables used during game creation and runtime:
    gameSave = false;
    isMultiplayer = false;
}

void MainWindow::onSinglePlayerClicked() {

    isMultiplayer = false;
    SinglePlayerOptionsDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        gameSave = dialog.getSelectedSave(); // 0 for new game, 1 for load game

        gameMap = new GameMapDialog(this);
        if (gameSave == 1) {
            if (!gameMap->loadGameFromFile("savegame.txt")) {
                qDebug() << "Failed to load saved game.";
            }
        } else {
            gameMap->setDifficulty(dialog.getSelectedDifficulty());
            gameMap->setMap(dialog.getSelectedMap());
        }
        gameMap->setMultiplayer(isMultiplayer);
        if (gameSave == 1) {
            if (!gameMap->loadGameFromFile("savegame.txt")) {
                qDebug() << "Failed to load saved game.";
                return; // Prevent further execution
            }
        } else {
            gameMap->drawMap();
        }
        gameMap->exec();
    }
}



void MainWindow::onMultiPlayerClicked()
{
    isMultiplayer = true;
    MultiPlayerOptionsDialog dialog(this);
    dialog.exec();

    // CODE HERE
}

MainWindow::~MainWindow()
{
    delete Background;
    delete SinglePlayerBtn;
    delete MultiPlayerBtn;
}


