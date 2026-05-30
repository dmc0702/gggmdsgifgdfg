#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStackedWidget>
#include "gamecore.h"
#include "battlewidget.h"
#include "choosepanel.h"

class QPushButton;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onStartGame();
    void onRules();
    void onExit();
    void onChoiceCompleted();

private:
    QStackedWidget *stackedWidget;
    QWidget *menuWidget;
    ChoosePanel *choosePanel1;
    ChoosePanel *choosePanel2;
    BattleWidget *battleWidget;
    
    Player *player1;
    Player *player2;
    
    void setupMenu();
    void setupStyle();
};

#endif