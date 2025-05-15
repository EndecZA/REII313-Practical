#ifndef MULTIPLAYEROPTIONSDIALOG_H
#define MULTIPLAYEROPTIONSDIALOG_H

#include <QDialog>
#include <QPushButton>

class MultiPlayerOptionsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit MultiPlayerOptionsDialog(QWidget *parent = nullptr);

private slots:
    void onHostGameClicked();
    void onJoinGameClicked();

private:
    QPushButton *hostGameBtn;
    QPushButton *joinGameBtn;
};

#endif // MULTIPLAYEROPTIONSDIALOG_H
