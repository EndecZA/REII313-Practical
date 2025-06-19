#include "multiplayeroptionsdialog.h"

#include <QVBoxLayout>
#include <QDebug>
#include <QHostAddress>

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

    // Games list
    gamesList = new QListWidget(this);
    gamesList->setFixedSize(300, 100);
    gamesList->setStyleSheet(
        "QListWidget {"
        "   background-color: #2E2E2E;"
        "   color: white;"
        "   border: 1px solid #FFB347;"
        "   border-radius: 5px;"
        "   font-family: 'Press Start 2P';"
        "   font-size: 14px;"
        "}"
        "QListWidget::item:selected {"
        "   background-color: #FF8C00;"
        "}");
    gamesList->setVisible(false);
    connect(gamesList, &QListWidget::itemClicked, this, &MultiPlayerOptionsDialog::onGameSelected);
    layout->addWidget(gamesList);

    // Center widgets vertically
    layout->setAlignment(Qt::AlignCenter);
    layout->setSpacing(20);

    setLayout(layout);

    // Setup network
    discoverySocket = new QUdpSocket(this);
    bool bindSuccess = discoverySocket->bind(DISCOVERY_PORT, QUdpSocket::ShareAddress | QUdpSocket::ReuseAddressHint);
    if (!bindSuccess) {
        qDebug() << "Failed to bind discovery socket to port" << DISCOVERY_PORT;
    }
    connect(discoverySocket, &QUdpSocket::readyRead, this, &MultiPlayerOptionsDialog::readPendingDatagrams);

    // Setup broadcast timer
    broadcastTimer = new QTimer(this);
    connect(broadcastTimer, &QTimer::timeout, this, &MultiPlayerOptionsDialog::broadcastGame);
}

MultiPlayerOptionsDialog::~MultiPlayerOptionsDialog()
{
    broadcastTimer->stop();
}

void MultiPlayerOptionsDialog::onHostGameClicked()
{
    // Start broadcasting game availability every 2 seconds
    broadcastGame();
    broadcastTimer->start(2000); // Broadcast every 2 seconds

    emit gameHosted();
    accept();
}

void MultiPlayerOptionsDialog::onJoinGameClicked()
{
    hostGameBtn->setVisible(false);
    joinGameBtn->setVisible(false);
    gamesList->setVisible(true);
    gamesList->clear();
    availableGames.clear();
    // Send a discovery request to prompt hosts to respond
    QByteArray datagram = "DISCOVER";
    discoverySocket->writeDatagram(datagram, QHostAddress::Broadcast, DISCOVERY_PORT);
}

void MultiPlayerOptionsDialog::broadcastGame()
{
    // Broadcast the host's IP address
    QByteArray datagram = "HOST:" + QHostAddress(QHostAddress::LocalHost).toString().toUtf8();
    discoverySocket->writeDatagram(datagram, QHostAddress::Broadcast, DISCOVERY_PORT);
    qDebug() << "Broadcasting host availability";
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
                qDebug() << "Discovered game at" << hostAddress;
            }
        } else if (datagram == "DISCOVER") {
            // Respond to discovery requests if this instance is hosting
            if (broadcastTimer->isActive()) {
                broadcastGame();
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
