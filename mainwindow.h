#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    QPushButton *SinglePlayerBtn, *MultiPlayerBtn;

private slots:
// void SinglePlayerClicked();
// void MultiPLayerClicked();

};
#endif // MAINWINDOW_H
