#ifndef GAMELOSTDIALOG_H
#define GAMELOSTDIALOG_H

#include <QDialog>
#include <QWidget>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QDebug>

class GameLostDialog : public QDialog
{
    Q_OBJECT

public:
    explicit GameLostDialog(int wave, QWidget *parent = nullptr);

private:
    QLabel *gameInfo;
    QPushButton *exitBtn;

private slots:
    void onExitClicked();

signals:
    void exitGame();

};

#endif // GAMELOSTDIALOG_H
