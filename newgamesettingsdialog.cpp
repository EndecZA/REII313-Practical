#include "newgamesettingsdialog.h"

#include <QVBoxLayout>
#include <QLabel>
#include <QDebug>

NewGameSettingsDialog::NewGameSettingsDialog(QWidget *parent)
    : QDialog(parent)
{
    setFixedSize(400, 400);
    setWindowTitle("New Game Settings");

    // Set background color
    setStyleSheet("QDialog { background-color: rgba(18, 28, 39, 255); }");

    // Create layout
    QVBoxLayout *layout = new QVBoxLayout(this);

    // Difficulty label
    QLabel *difficultyLabel = new QLabel("Difficulty:", this);
    difficultyLabel->setStyleSheet(
        "QLabel {"
        "   color: #E0E0E0;"
        "   font-family: 'Press Start 2P';"
        "   font-size: 16px;"
        "}");
    layout->addWidget(difficultyLabel);

    // Difficulty combo box
    difficultyCombo = new QComboBox(this);
    difficultyCombo->addItems({"Easy", "Medium", "Hard"});
    difficultyCombo->setStyleSheet(
        "QComboBox {"
        "   background-color: #FFB347;"
        "   color: black;"
        "   border: 1px solid black;"
        "   font-family: 'Press Start 2P';"
        "   font-size: 16px;"
        "   padding: 5px;"
        "}"
        "QComboBox QAbstractItemView {"
        "   background-color: #FFB347;"
        "   color: black;"
        "   selection-background-color: #FF8C00;"
        "}");
    layout->addWidget(difficultyCombo);

    // Map label
    QLabel *mapLabel = new QLabel("Map:", this);
    mapLabel->setStyleSheet(
        "QLabel {"
        "   color: #E0E0E0;"
        "   font-family: 'Press Start 2P';"
        "   font-size: 16px;"
        "}");
    layout->addWidget(mapLabel);

    // Map combo box
    mapCombo = new QComboBox(this);
    mapCombo->addItems({"Map 1", "Map 2", "Map 3", "Random"});
    mapCombo->setStyleSheet(
        "QComboBox {"
        "   background-color: #FFB347;"
        "   color: black;"
        "   border: 1px solid black;"
        "   font-family: 'Press Start 2P';"
        "   font-size: 16px;"
        "   padding: 5px;"
        "}"
        "QComboBox QAbstractItemView {"
        "   background-color: #FFB347;"
        "   color: black;"
        "   selection-background-color: #FF8C00;"
        "}");
    layout->addWidget(mapCombo);

    // Confirm button
    confirmBtn = new QPushButton("Start Game", this);
    confirmBtn->setFixedSize(300, 75);
    confirmBtn->setStyleSheet(
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
    connect(confirmBtn, &QPushButton::clicked, this, &NewGameSettingsDialog::onConfirmClicked);
    layout->addWidget(confirmBtn);

    // Back button
    backBtn = new QPushButton("Back", this);
    backBtn->setFixedSize(300, 75);
    backBtn->setStyleSheet(
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
    connect(backBtn, &QPushButton::clicked, this, &NewGameSettingsDialog::onBackClicked);
    layout->addWidget(backBtn);

    // Center layout
    layout->setAlignment(Qt::AlignCenter);
    layout->setSpacing(15);
    setLayout(layout);

}

void NewGameSettingsDialog::onConfirmClicked()
{
    selectedDifficulty = difficultyCombo->currentIndex();
    selectedMap = mapCombo->currentIndex();
    qDebug() << "NewGameSettingsDialog: Selected Difficulty:" << selectedDifficulty << ", Map:" << selectedMap;
    accept(); // Close dialog with "accepted" status
}

void NewGameSettingsDialog::onBackClicked()
{
    reject(); // Close dialog with "rejected" status
}
