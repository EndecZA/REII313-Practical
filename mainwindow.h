#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QPushButton>
#include <QSound>

class GameMapDialog;

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
    GameMapDialog *gameMap;
    QSound *backgroundSound;

    int gameSave; // Save slot (0 => new game).
    bool isMultiplayer; // Is game online.

};

#endif // MAINWINDOW_H
