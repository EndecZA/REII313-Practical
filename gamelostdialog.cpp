#include "gamelostdialog.h"

GameLostDialog::GameLostDialog(int wave, QWidget *parent)
    : QDialog(parent)
{
    setModal(true);
    setFixedSize(400, 215);
    setWindowTitle("Game Lost");

    setStyleSheet("QDialog { background-color: rgba(18, 28, 39, 255); }");

    QVBoxLayout *layout = new QVBoxLayout(this);

    gameInfo = new QLabel(QString("You Lost!\nReached wave: %1").arg(wave));
    gameInfo->setFixedSize(350, 80);
    gameInfo->setStyleSheet(
        "QLabel {"
        "   background-color: #FFB347;"
        "   color: black;"
        "   border: 1px solid black;"
        "   border-radius: 5px;"
        "   font-family: 'Press Start 2P';"
        "   font-size: 20px;"
        "}");
    layout->addWidget(gameInfo);

    exitBtn = new QPushButton("Exit Game", this);
    exitBtn->setFixedSize(350, 75);
    exitBtn->setStyleSheet(
        "QPushButton {"
        "   background-color: #FFB347;"
        "   color: black;"
        "   border: 1px solid black;"
        "   border-radius: 5px;"
        "   font-family: 'Press Start 2P';"
        "   font-size: 20px;"
        "}"
        "QPushButton:hover {"
        "   background-color: #FF8C00;"
        "}");
    layout->addWidget(exitBtn);
    connect(exitBtn, &QPushButton::clicked, this, &GameLostDialog::onExitClicked);

    layout->setAlignment(Qt::AlignCenter);
    layout->setSpacing(20);
    setLayout(layout);
}

void GameLostDialog::onExitClicked()
{
    qDebug() << "GameLostDialog: Exit clicked";
    emit exitGame();
    reject();
}
