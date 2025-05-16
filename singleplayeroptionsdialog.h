#ifndef SINGLEPLAYEROPTIONSDIALOG_H
#define SINGLEPLAYEROPTIONSDIALOG_H

#include <QDialog>
#include <QPushButton>

class SinglePlayerOptionsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SinglePlayerOptionsDialog(QWidget *parent = nullptr);
    QString getSelectedDifficulty() const { return selectedDifficulty; }
    QString getSelectedMap() const { return selectedMap; }

private slots:
    void onNewGameClicked();
    void onLoadGameClicked();

private:
    QPushButton *newGameBtn;
    QPushButton *loadGameBtn;
    QString selectedDifficulty;
    QString selectedMap;
};

#endif // SINGLEPLAYEROPTIONSDIALOG_H
