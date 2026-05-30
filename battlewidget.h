#ifndef BATTLEWIDGET_H
#define BATTLEWIDGET_H

#include <QWidget>
#include "gamecore.h"

class QTextEdit;
class QProgressBar;
class QLabel;
class QPushButton;
class QListWidget;
class QListWidgetItem;

class BattleWidget : public QWidget
{
    Q_OBJECT

public:
    explicit BattleWidget(Player* p1, Player* p2, QWidget *parent = nullptr);

private slots:
    void onAttack();
    void onMove();
    void onSkill();
    void onEndTurn();
    void onSkillSelected(int row);
    void onMoveSelected(int row);
    void updateUI();

private:
    Player* player1;
    Player* player2;
    Player* currentPlayer;
    Player* opponent;
    int round;
    bool gameRunning;
    
    QTextEdit *logText;
    QProgressBar *hpBar1;
    QProgressBar *hpBar2;
    QLabel *apLabel1;
    QLabel *apLabel2;
    QLabel *posLabel1;
    QLabel *posLabel2;
    QLabel *weaponLabel1;
    QLabel *weaponLabel2;
    QLabel *buffLabel1;
    QLabel *buffLabel2;
    QPushButton *attackBtn;
    QPushButton *moveBtn;
    QPushButton *skillBtn;
    QPushButton *endBtn;
    QListWidget *skillList;
    QListWidget *moveList;
    
    void addLog(const QString& msg);
    void checkGameOver();
    void switchPlayer();
    void applyEquipmentEffects(Player& player);
    void applyOnAttackEffects(Player& attacker, Player& target, int dmg);
    void applyOnDamagedEffects(Player& target, Player& attacker, int dmg);
    QString getBuffText(Player& player);
};

#endif