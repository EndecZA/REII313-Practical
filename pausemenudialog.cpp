#include "pausemenudialog.h"
#include <QVBoxLayout>
#include <QDebug>

PauseMenuDialog::PauseMenuDialog(QWidget *parent)
    : QDialog(parent)
{
    setModal(true);
    setFixedSize(400, 400);
    setWindowTitle("Pause Menu");

    setStyleSheet("QDialog { background-color: rgba(18, 28, 39, 255); }");

    QVBoxLayout *layout = new QVBoxLayout(this);

    resumeBtn = new QPushButton("Resume", this);
    resumeBtn->setFixedSize(300, 75);
    resumeBtn->setStyleSheet(
        "QPushButton {"
        "   background-color: #FFB347;"
        "   color: black;"
        "   border: 1px solid black;"
        "   border-radius: 5px;"
        "   font-family: 'Press Start 2P';"
        "   font-size: 20px;"
        "}"
        "QPushButton:hover {"
        "   background-color: #FF8C00;"
        "}");
    connect(resumeBtn, &QPushButton::clicked, this, &PauseMenuDialog::onResumeClicked);
    layout->addWidget(resumeBtn);

    saveBtn = new QPushButton("Save Game", this);
    saveBtn->setFixedSize(300, 75);
    saveBtn->setStyleSheet(
        "QPushButton {"
        "   background-color: #FFB347;"
        "   color: black;"
        "   border: 1px solid black;"
        "   border-radius: 5px;"
        "   font-family: 'Press Start 2P';"
        "   font-size: 20px;"
        "}"
        "QPushButton:hover {"
        "   background-color: #FF8C00;"
        "}");
    connect(saveBtn, &QPushButton::clicked, this, &PauseMenuDialog::onSaveClicked);
    layout->addWidget(saveBtn);

    exitBtn = new QPushButton("Exit Game", this);
    exitBtn->setFixedSize(300, 75);
    exitBtn->setStyleSheet(
        "QPushButton {"
        "   background-color: #FFB347;"
        "   color: black;"
        "   border: 1px solid black;"
        "   border-radius: 5px;"
        "   font-family: 'Press Start 2P';"
        "   font-size: 20px;"
        "}"
        "QPushButton:hover {"
        "   background-color: #FF8C00;"
        "}");
    connect(exitBtn, &QPushButton::clicked, this, &PauseMenuDialog::onExitClicked);
    layout->addWidget(exitBtn);

    layout->setAlignment(Qt::AlignCenter);
    layout->setSpacing(20);
    setLayout(layout);
}

void PauseMenuDialog::onResumeClicked()
{
    qDebug() << "PauseMenuDialog: Resume clicked";
    emit resumeGame();
    accept();
}

void PauseMenuDialog::onSaveClicked()
{
    qDebug() << "PauseMenuDialog: Save clicked";
    emit saveGame();
    accept();
}

void PauseMenuDialog::onExitClicked()
{
    qDebug() << "PauseMenuDialog: Exit clicked";
    emit exitGame();
    reject();
}
