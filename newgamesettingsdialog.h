#ifndef NEWGAMESETTINGSDIALOG_H
#define NEWGAMESETTINGSDIALOG_H

#include <QDialog>
#include <QComboBox>
#include <QPushButton>

class NewGameSettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit NewGameSettingsDialog(QWidget *parent = nullptr);
    QString getSelectedDifficulty() const { return selectedDifficulty; }
    QString getSelectedMap() const { return selectedMap; }

private slots:
    void onConfirmClicked();
    void onBackClicked();

private:
    QComboBox *difficultyCombo;
    QComboBox *mapCombo;
    QPushButton *confirmBtn;
    QPushButton *backBtn;
    QString selectedDifficulty;
    QString selectedMap;
};

#endif // NEWGAMESETTINGSDIALOG_H
