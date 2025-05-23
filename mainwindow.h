#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QPushButton>
#include <QWindow>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onSinglePlayerClicked();
    void onMultiPlayerClicked();

private:
    QLabel *Background;
    QPushButton *SinglePlayerBtn;
    QPushButton *MultiPlayerBtn;
    QWindow gameWindow;
    int gameDifficulty; // Store difficulty
    int gameMap; // Store map

};

#endif // MAINWINDOW_H
