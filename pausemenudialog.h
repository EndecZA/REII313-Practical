#ifndef PAUSEMENUDIALOG_H
#define PAUSEMENUDIALOG_H

#include <QDialog>
#include <QPushButton>

class PauseMenuDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PauseMenuDialog(QWidget *parent = nullptr);

signals:
    void resumeGame();
    void saveGame();
    void exitGame();

private slots:
    void onResumeClicked();
    void onSaveClicked();
    void onExitClicked();

private:
    QPushButton *resumeBtn;
    QPushButton *saveBtn;
    QPushButton *exitBtn;
};

#endif // PAUSEMENUDIALOG_H
