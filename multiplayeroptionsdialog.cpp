#include "multiplayeroptionsdialog.h"

#include <QVBoxLayout>
#include <QDebug>

MultiPlayerOptionsDialog::MultiPlayerOptionsDialog(QWidget *parent)
    : QDialog(parent)
{
    setFixedSize(400, 300);
    setWindowTitle("Multiplayer Options");

    // Set background color to match MainWindow
    setStyleSheet("QDialog { background-color: rgba(18, 28, 39, 255); }");

    // Create layout
    QVBoxLayout *layout = new QVBoxLayout(this);

    // Host Game button
    hostGameBtn = new QPushButton("Host Game", this);
    hostGameBtn->setFixedSize(300, 75);
    hostGameBtn->setStyleSheet(
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
    connect(hostGameBtn, &QPushButton::clicked, this, &MultiPlayerOptionsDialog::onHostGameClicked);
    layout->addWidget(hostGameBtn);

    // Join Game button
    joinGameBtn = new QPushButton("Join Game", this);
    joinGameBtn->setFixedSize(300, 75);
    joinGameBtn->setStyleSheet(
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
    connect(joinGameBtn, &QPushButton::clicked, this, &MultiPlayerOptionsDialog::onJoinGameClicked);
    layout->addWidget(joinGameBtn);

    // Center buttons vertically
    layout->setAlignment(Qt::AlignCenter);
    layout->setSpacing(20);

    setLayout(layout);
}

void MultiPlayerOptionsDialog::onHostGameClicked()
{
    qDebug() << "MultiPlayerOptionsDialog: Game selected";
    accept(); // Close dialog with "accepted" status
    // Add logic for hosting a game here
}

void MultiPlayerOptionsDialog::onJoinGameClicked()
{
    qDebug() << "MultiPlayerOptionsDialog: Join Game selected";
    accept(); // Close dialog with "accepted" status
    // Add logic for joining a game here
}
