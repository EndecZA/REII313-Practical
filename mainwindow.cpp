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
    QFontDatabase().addApplicationFont(":/resources/fonts/PressStart2P-Regular.ttf");

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

}

void MainWindow::onSinglePlayerClicked()
{
    SinglePlayerOptionsDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        gameDifficulty = dialog.getSelectedDifficulty();
        gameMap = dialog.getSelectedMap();
        if (gameDifficulty != -1 && gameMap != -1) {
            // Example switch case for difficulty
            qDebug() << "MainWindow: Starting new game with Difficulty:" << gameDifficulty << ", Map:" << gameMap;
            switch (gameDifficulty) {
            case 0: // Easy
                qDebug() << "MainWindow: Configuring game for Easy difficulty";
                // Set game parameters (e.g., enemy strength, lives)
                break;
            case 1: // Medium
                qDebug() << "MainWindow: Configuring game for Medium difficulty";
                // Set game parameters
                break;
            case 2: // Hard
                qDebug() << "MainWindow: Configuring game for Hard difficulty";
                // Set game parameters
                break;
            default:
                qDebug() << "MainWindow: Unknown difficulty";
            }

            // Example switch case for map
            switch (gameMap) {
            case 0: // Map1
                qDebug() << "MainWindow: Loading Map1";
                // Load Map1 assets
                break;
            case 1: // Map2
                qDebug() << "MainWindow: Loading Map2";
                // Load Map2 assets
                break;
            case 2: // Map3
                qDebug() << "MainWindow: Loading Map3";
                // Load Map3 assets
                break;
            case 3: // Random
                qDebug() << "MainWindow: Generating random map";
                // Generate random map
                break;
            default:
                qDebug() << "MainWindow: Unknown map";
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
