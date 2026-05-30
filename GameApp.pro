QT += core gui widgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    gamecore.cpp \
    battlewidget.cpp \
    choosepanel.cpp

HEADERS += \
    mainwindow.h \
    gamecore.h \
    battlewidget.h \
    choosepanel.h