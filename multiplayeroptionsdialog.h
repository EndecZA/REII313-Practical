#ifndef MULTIPLAYEROPTIONSDIALOG_H
#define MULTIPLAYEROPTIONSDIALOG_H

#include <QDialog>
#include <QPushButton>
#include <QUdpSocket>
#include <QTcpSocket>
#include <QListWidget>
#include <QTimer>

class MultiPlayerOptionsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit MultiPlayerOptionsDialog(QWidget *parent = nullptr);
    ~MultiPlayerOptionsDialog();

signals:
    void gameHosted();
    void gameJoined(const QString& hostAddress);

private slots:
    void onHostGameClicked();
    void onJoinGameClicked();
    void readPendingDatagrams();
    void onGameSelected(QListWidgetItem* item);
    void broadcastGame();

private:
    QString getLocalIp();
    QPushButton *hostGameBtn;
    QPushButton *joinGameBtn;
    QListWidget *gamesList;
    QUdpSocket *discoverySocket;
    QTimer *broadcastTimer;
    QMap<QString, QString> availableGames;
};

#endif // MULTIPLAYEROPTIONSDIALOG_H
