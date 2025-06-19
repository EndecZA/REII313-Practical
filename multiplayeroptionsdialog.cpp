#include "multiplayeroptionsdialog.h"

#include <QVBoxLayout>
#include <QDebug>

const quint16 DISCOVERY_PORT = 45454;

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

    gamesList = new QListWidget(this);
    gamesList->setVisible(false);
    connect(gamesList, &QListWidget::itemClicked, this, &MultiPlayerOptionsDialog::onGameSelected);

    // Setup network
    discoverySocket = new QUdpSocket(this);
    discoverySocket->bind(DISCOVERY_PORT, QUdpSocket::ShareAddress);
    connect(discoverySocket, &QUdpSocket::readyRead, this, &MultiPlayerOptionsDialog::readPendingDatagrams);

}

void MultiPlayerOptionsDialog::onHostGameClicked()
{
    // Start broadcasting game availability
    QByteArray datagram = "HOST:" + QHostAddress(QHostAddress::LocalHost).toString().toUtf8();
    discoverySocket->writeDatagram(datagram, QHostAddress::Broadcast, DISCOVERY_PORT);

    emit gameHosted();
    accept();
}

void MultiPlayerOptionsDialog::onJoinGameClicked()
{
    hostGameBtn->setVisible(false);
    joinGameBtn->setVisible(false);
    gamesList->setVisible(true);
}

void MultiPlayerOptionsDialog::readPendingDatagrams()
{
    while (discoverySocket->hasPendingDatagrams()) {
        QByteArray datagram;
        datagram.resize(discoverySocket->pendingDatagramSize());
        QHostAddress sender;
        quint16 senderPort;
        discoverySocket->readDatagram(datagram.data(), datagram.size(), &sender, &senderPort);
        if (datagram.startsWith("HOST:")) {
            QString hostAddress = QString(datagram.mid(5));
            if (!availableGames.contains(hostAddress)) {
                availableGames.insert(hostAddress, "Game at " + hostAddress);
                new QListWidgetItem("Game at " + hostAddress, gamesList);
            }
        }
    }
}

void MultiPlayerOptionsDialog::onGameSelected(QListWidgetItem* item)
{
    QString gameInfo = item->text();
    QString hostAddress = availableGames.key(gameInfo);

    if (!hostAddress.isEmpty()) {
        emit gameJoined(hostAddress);
        accept();
    }
}
