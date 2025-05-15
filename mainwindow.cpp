#include "mainwindow.h"

#include <QPixmap>
#include <QDebug>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setFixedSize(800,600);
    setMouseTracking(true);

    Background = new QLabel(this);
        Background->setGeometry(0, 0, 800, 600);
        Background->setPixmap(QPixmap(":/images/Logo.png"));
        Background->setScaledContents(true); // Scale image to fit label
        Background->lower(); // Keep behind other widgets

    SinglePlayerBtn = new QPushButton(this);
    SinglePlayerBtn->setFixedSize(200,75);
    SinglePlayerBtn->setText("Singleplayer");
    SinglePlayerBtn->move(475,300);
    SinglePlayerBtn->setStyleSheet(
            "QPushButton {"
            "   background-color: #FFB347;" // Orange-Yellow
            "   color: black;" // Dark text for contrast
            "   border: 1px solid black;" // Soft white border
            "   border-radius: 5px;"
            "   font-size: 24px;"
            "}"
            "QPushButton:hover {"
            "   background-color: #FF8C00;" // Brighter orange on hover
            "}");
  //  connect(SinglePlayerBtn, SIGNAL(clicked()),this,SLOT(SinglePlayerClicked()));

    MultiPlayerBtn = new QPushButton(this);
    MultiPlayerBtn->setFixedSize(200,75);
    MultiPlayerBtn->setText("Multiplayer");
    MultiPlayerBtn->move(475,400);
    MultiPlayerBtn->setStyleSheet(
            "QPushButton {"
            "   background-color: #FFB347;"
            "   color: black;"
            "   border: 1px solid black;"
            "   border-radius: 5px;"
            "   font-size: 24px;"
            "}"
            "QPushButton:hover {"
            "   background-color: #FF8C00;" // Brighter orange on hover
            "}");
  //  connect(MultiPlayerBtn, SIGNAL(clicked()),this,SLOT(MultiPlayerClicked()));

}

MainWindow::~MainWindow()
{
}


