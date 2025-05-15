#ifndef SINGLEPLAYEROPTIONSDIALOG_H
#define SINGLEPLAYEROPTIONSDIALOG_H

#include <QDialog>
#include <QPushButton>

class SinglePlayerOptionsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SinglePlayerOptionsDialog(QWidget *parent = nullptr);

private slots:
    void onNewGameClicked();
    void onLoadGameClicked();

private:
    QPushButton *newGameBtn;
    QPushButton *loadGameBtn;
};

#endif // SINGLEPLAYEROPTIONSDIALOG_H
