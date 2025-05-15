#include "singleplayeroptionsdialog.h"
#include <QVBoxLayout>
#include <QDebug>

SinglePlayerOptionsDialog::SinglePlayerOptionsDialog(QWidget *parent)
    : QDialog(parent)
{
    setFixedSize(400, 300);
    setWindowTitle("Singleplayer Options");

    // Set background color to match MainWindow
    setStyleSheet("QDialog { background-color: rgba(18, 28, 39, 255); }");

    // Create layout
    QVBoxLayout *layout = new QVBoxLayout(this);

    // New Game button
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

    // Load Game button
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

    // Center buttons vertically
    layout->setAlignment(Qt::AlignCenter);
    layout->setSpacing(20);

    setLayout(layout);
}

void SinglePlayerOptionsDialog::onNewGameClicked()
{
    qDebug() << "New Game selected";
    accept(); // Close dialog with "accepted" status
    // Add logic for starting a new game here
}

void SinglePlayerOptionsDialog::onLoadGameClicked()
{
    qDebug() << "Load Game selected";
    accept(); // Close dialog with "accepted" status
    // Add logic for loading a game here
}
