#include "mainwindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle("技能·武器·召唤 - 对决");
    setFixedSize(1200, 800);
    
    GameData::init();
    
    stackedWidget = new QStackedWidget(this);
    setCentralWidget(stackedWidget);
    
    setupStyle();
    setupMenu();
}

MainWindow::~MainWindow()
{
    delete player1;
    delete player2;
}

void MainWindow::setupStyle(){
    setStyleSheet(R"(
        QMainWindow {
            background-color: #1a1a2e;
        }
        QPushButton {
            background-color: #16213e;
            color: #c4a747;
            border: 2px solid #c4a747;
            border-radius: 10px;
            font-size: 18px;
            font-weight: bold;
            padding: 10px 20px;
        }
        QPushButton:hover {
            background-color: #c4a747;
            color: #16213e;
        }
        QLabel {
            color: #c4a747;
            font-size: 16px;
        }
    )");
}

void MainWindow::setupMenu(){
    menuWidget = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(menuWidget);
    
    QLabel *titleLabel = new QLabel("技能·武器·召唤");
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet("font-size: 48px; font-weight: bold; color: #c4a747; margin: 50px;");
    
    QPushButton *startBtn = new QPushButton("开始游戏");
    QPushButton *rulesBtn = new QPushButton("游戏规则");
    QPushButton *exitBtn = new QPushButton("退出游戏");
    
    startBtn->setFixedSize(200, 50);
    rulesBtn->setFixedSize(200, 50);
    exitBtn->setFixedSize(200, 50);
    
    connect(startBtn, &QPushButton::clicked, this, &MainWindow::onStartGame);
    connect(rulesBtn, &QPushButton::clicked, this, &MainWindow::onRules);
    connect(exitBtn, &QPushButton::clicked, this, &MainWindow::onExit);
    
    layout->addWidget(titleLabel);
    layout->addWidget(startBtn, 0, Qt::AlignCenter);
    layout->addWidget(rulesBtn, 0, Qt::AlignCenter);
    layout->addWidget(exitBtn, 0, Qt::AlignCenter);
    layout->addStretch();
    
    stackedWidget->addWidget(menuWidget);
}

void MainWindow::onStartGame(){
    player1 = new Player(1);
    player2 = new Player(2);
    
    choosePanel1 = new ChoosePanel(player1, 1);
    choosePanel2 = new ChoosePanel(player2, 2);
    
    connect(choosePanel1, &ChoosePanel::choiceCompleted, this, &MainWindow::onChoiceCompleted);
    connect(choosePanel2, &ChoosePanel::choiceCompleted, this, &MainWindow::onChoiceCompleted);
    
    stackedWidget->addWidget(choosePanel1);
    stackedWidget->addWidget(choosePanel2);
    stackedWidget->setCurrentWidget(choosePanel1);
}

void MainWindow::onRules(){
    QMessageBox::information(this, "游戏规则",
        "1. 普攻伤害由武器决定，花费2AP\n"
        "2. 技能花费2AP，造成单次主动效果\n"
        "3. 防具花费1AP，提供持续效果\n"
        "4. 一人仅能装备一件武器\n"
        "5. 每回合开始恢复5行动点\n"
        "6. 生命值降至0时失败");
}

void MainWindow::onExit(){
    close();
}

void MainWindow::onChoiceCompleted(){
    if(stackedWidget->currentWidget() == choosePanel1){
        stackedWidget->setCurrentWidget(choosePanel2);
    } else if(stackedWidget->currentWidget() == choosePanel2){
        battleWidget = new BattleWidget(player1, player2);
        stackedWidget->addWidget(battleWidget);
        stackedWidget->setCurrentWidget(battleWidget);
    }
}