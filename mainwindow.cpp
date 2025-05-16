#include "mainwindow.h"
#include "singleplayeroptionsdialog.h"
#include "multiplayeroptionsdialog.h"
#include <QPixmap>
#include <QFontDatabase>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setFixedSize(800, 600);
    setMouseTracking(true);

    // Load custom font (Press Start 2P)
    int fontId = QFontDatabase().addApplicationFont(":/fonts/PressStart2P-Regular.ttf");

    // Background QLabel
    Background = new QLabel(this);
    Background->setGeometry(0, 0, 800, 600);
    QPixmap pixmap(":/images/Logo.png");
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
}

void MainWindow::onSinglePlayerClicked()
{
    SinglePlayerOptionsDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        gameDifficulty = dialog.getSelectedDifficulty();
        gameMap = dialog.getSelectedMap();
        if (!gameDifficulty.isEmpty() && !gameMap.isEmpty()) {
            // Example switch case for difficulty
            qDebug() << "Starting new game with Difficulty:" << gameDifficulty << ", Map:" << gameMap;
            QString difficulty = gameDifficulty.toLower();
            switch (difficulty.at(0).toLatin1()) {
            case 'e': // Easy
                qDebug() << "Configuring game for Easy difficulty";
                // Set game parameters (e.g., enemy strength, lives)
                break;
            case 'm': // Medium
                qDebug() << "Configuring game for Medium difficulty";
                // Set game parameters
                break;
            case 'h': // Hard
                qDebug() << "Configuring game for Hard difficulty";
                // Set game parameters
                break;
            default:
                qDebug() << "Unknown difficulty";
            }

            // Example switch case for map
            QString map = gameMap.toLower();
            switch (map.at(0).toLatin1()) {
            case 'm': // Map1, Map2, Map3
                if (map == "map1") {
                    qDebug() << "Loading Map1";
                    // Load Map1 assets
                } else if (map == "map2") {
                    qDebug() << "Loading Map2";
                    // Load Map2 assets
                } else if (map == "map3") {
                    qDebug() << "Loading Map3";
                    // Load Map3 assets
                }
                break;
            case 'r': // Random
                qDebug() << "Generating random map";
                // Generate random map
                break;
            default:
                qDebug() << "Unknown map";
            }
        }
    }
}

void MainWindow::onMultiPlayerClicked()
{
    MultiPlayerOptionsDialog dialog(this);
    dialog.exec();
}

MainWindow::~MainWindow()
{
    delete Background;
    delete SinglePlayerBtn;
    delete MultiPlayerBtn;
}
