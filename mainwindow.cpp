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

    // Load custom font
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
    dialog.exec(); // Modal dialog
}

void MainWindow::onMultiPlayerClicked()
{
    MultiPlayerOptionsDialog dialog(this);
    dialog.exec(); // Modal dialog
}

MainWindow::~MainWindow()
{
    delete Background;
    delete SinglePlayerBtn;
    delete MultiPlayerBtn;
}
