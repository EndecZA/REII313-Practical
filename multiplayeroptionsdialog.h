// multiplayeroptionsdialog.h
#ifndef MULTIPLAYEROPTIONSDIALOG_H
#define MULTIPLAYEROPTIONSDIALOG_H

#include <QDialog>
#include <QPushButton>
#include <QUdpSocket>
#include <QTcpSocket>
#include <QListWidget>

class MultiPlayerOptionsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit MultiPlayerOptionsDialog(QWidget *parent = nullptr);

signals:
    void gameHosted();
    void gameJoined(const QString& hostAddress);

private slots:
    void onHostGameClicked();
    void onJoinGameClicked();
    void readPendingDatagrams();
    void onGameSelected(QListWidgetItem* item);

private:
    QPushButton *hostGameBtn;
    QPushButton *joinGameBtn;
    QListWidget *gamesList;
    QUdpSocket *discoverySocket;
    QMap<QString, QString> availableGames;
};

#endif // MULTIPLAYEROPTIONSDIALOG_H
