QT       += core gui
QT += core gui widgets multimedia
QT += network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    enemy.cpp \
    gamemapdialog.cpp \
    main.cpp \
    mainwindow.cpp \
    multiplayeroptionsdialog.cpp \
    newgamesettingsdialog.cpp \
    pausemenudialog.cpp \
    singleplayeroptionsdialog.cpp \
    tile.cpp \
    tower.cpp

HEADERS += \
    enemy.h \
    gamemapdialog.h \
    mainwindow.h \
    multiplayeroptionsdialog.h \
    newgamesettingsdialog.h \
    pausemenudialog.h \
    singleplayeroptionsdialog.h \
    tile.h \
    tower.h

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    ../../Logo.png

RESOURCES += \
    resources.qrc
