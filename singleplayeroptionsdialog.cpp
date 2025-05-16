#include "singleplayeroptionsdialog.h"
#include "newgamesettingsdialog.h"
#include <QVBoxLayout>
#include <QDebug>

SinglePlayerOptionsDialog::SinglePlayerOptionsDialog(QWidget *parent)
    : QDialog(parent)
{
    setFixedSize(400, 300);
    setWindowTitle("Singleplayer Options");

    setStyleSheet("QDialog { background-color: rgba(18, 28, 39, 255); }");

    QVBoxLayout *layout = new QVBoxLayout(this);

    newGameBtn = new QPushButton("New Game", this);
    newGameBtn->setFixedSize(300, 75);
    newGameBtn->setStyleSheet(
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
    connect(newGameBtn, &QPushButton::clicked, this, &SinglePlayerOptionsDialog::onNewGameClicked);
    layout->addWidget(newGameBtn);

    loadGameBtn = new QPushButton("Load Game", this);
    loadGameBtn->setFixedSize(300, 75);
    loadGameBtn->setStyleSheet(
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
    connect(loadGameBtn, &QPushButton::clicked, this, &SinglePlayerOptionsDialog::onLoadGameClicked);
    layout->addWidget(loadGameBtn);

    layout->setAlignment(Qt::AlignCenter);
    layout->setSpacing(20);
    setLayout(layout);
}

void SinglePlayerOptionsDialog::onNewGameClicked()
{
    NewGameSettingsDialog settingsDialog(this);
    if (settingsDialog.exec() == QDialog::Accepted) {
        selectedDifficulty = settingsDialog.getSelectedDifficulty();
        selectedMap = settingsDialog.getSelectedMap();
        qDebug() << "New Game - Difficulty:" << selectedDifficulty << ", Map:" << selectedMap;
        accept(); // Close SinglePlayerOptionsDialog
    }
}

void SinglePlayerOptionsDialog::onLoadGameClicked()
{
    qDebug() << "Load Game selected";
    accept();
}
