#ifndef SINGLEPLAYEROPTIONSDIALOG_H
#define SINGLEPLAYEROPTIONSDIALOG_H

#include <QDialog>
#include <QPushButton>

class SinglePlayerOptionsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SinglePlayerOptionsDialog(QWidget *parent = nullptr);
    int getSelectedDifficulty() const { return selectedDifficulty; }
    int getSelectedMap() const { return selectedMap; }
    int getSelectedSave() const { return selectedSave; }

private slots:
    void onNewGameClicked();
    void onLoadGameClicked();

private:
    QPushButton *newGameBtn;
    QPushButton *loadGameBtn;
    int selectedDifficulty;
    int selectedMap;
    int selectedSave;

};

#endif // SINGLEPLAYEROPTIONSDIALOG_H
