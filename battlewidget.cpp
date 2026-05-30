#include "battlewidget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTextEdit>
#include <QProgressBar>
#include <QLabel>
#include <QPushButton>
#include <QListWidget>
#include <QMessageBox>

BattleWidget::BattleWidget(Player* p1, Player* p2, QWidget *parent)
    : QWidget(parent), player1(p1), player2(p2), round(1), gameRunning(true)
{
    currentPlayer = player1;
    opponent = player2;
    
    setStyleSheet(R"(
        QTextEdit {
            background-color: #0a0a1a;
            color: #c4a747;
            border: 2px solid #c4a747;
            border-radius: 5px;
            font-family: monospace;
        }
        QProgressBar {
            border: 2px solid #c4a747;
            border-radius: 5px;
            text-align: center;
        }
        QProgressBar::chunk {
            background-color: #e94560;
            border-radius: 3px;
        }
        QListWidget {
            background-color: #16213e;
            color: #c4a747;
            border: 2px solid #c4a747;
            border-radius: 5px;
        }
        QPushButton {
            background-color: #16213e;
            color: #c4a747;
            border: 2px solid #c4a747;
            border-radius: 5px;
            font-size: 14px;
            padding: 5px;
        }
        QPushButton:hover {
            background-color: #c4a747;
            color: #16213e;
        }
    )");
    
    QHBoxLayout *mainLayout = new QHBoxLayout(this);
    
    // 左侧：状态面板
    QWidget *leftPanel = new QWidget();
    QVBoxLayout *leftLayout = new QVBoxLayout(leftPanel);
    
    QLabel *p1Label = new QLabel("玩家1");
    p1Label->setStyleSheet("font-size: 20px; font-weight: bold;");
    hpBar1 = new QProgressBar();
    hpBar1->setRange(0, player1->maxHealth);
    apLabel1 = new QLabel(QString("行动点: %1").arg(player1->action));
    posLabel1 = new QLabel(QString("位置: %1").arg(player1->pos));
    weaponLabel1 = new QLabel(QString("武器: %1").arg(player1->getWeaponName()));
    buffLabel1 = new QLabel();
    buffLabel1->setWordWrap(true);
    
    leftLayout->addWidget(p1Label);
    leftLayout->addWidget(hpBar1);
    leftLayout->addWidget(apLabel1);
    leftLayout->addWidget(posLabel1);
    leftLayout->addWidget(weaponLabel1);
    leftLayout->addWidget(buffLabel1);
    leftLayout->addStretch();
    
    QLabel *p2Label = new QLabel("玩家2");
    p2Label->setStyleSheet("font-size: 20px; font-weight: bold;");
    hpBar2 = new QProgressBar();
    hpBar2->setRange(0, player2->maxHealth);
    apLabel2 = new QLabel(QString("行动点: %1").arg(player2->action));
    posLabel2 = new QLabel(QString("位置: %1").arg(player2->pos));
    weaponLabel2 = new QLabel(QString("武器: %1").arg(player2->getWeaponName()));
    buffLabel2 = new QLabel();
    buffLabel2->setWordWrap(true);
    
    leftLayout->addWidget(p2Label);
    leftLayout->addWidget(hpBar2);
    leftLayout->addWidget(apLabel2);
    leftLayout->addWidget(posLabel2);
    leftLayout->addWidget(weaponLabel2);
    leftLayout->addWidget(buffLabel2);
    
    // 中间：战斗日志
    logText = new QTextEdit();
    logText->setReadOnly(true);
    logText->setMaximumWidth(400);
    
    // 右侧：操作面板
    QWidget *rightPanel = new QWidget();
    QVBoxLayout *rightLayout = new QVBoxLayout(rightPanel);
    
    attackBtn = new QPushButton("攻击 (-2AP)");
    moveBtn = new QPushButton("移动 (-1AP)");
    skillBtn = new QPushButton("技能 (-2AP)");
    endBtn = new QPushButton("结束回合");
    
    skillList = new QListWidget();
    moveList = new QListWidget();
    moveList->addItem("向左移动5");
    moveList->addItem("向右移动5");
    
    rightLayout->addWidget(attackBtn);
    rightLayout->addWidget(moveBtn);
    rightLayout->addWidget(skillBtn);
    rightLayout->addWidget(skillList);
    rightLayout->addWidget(moveList);
    rightLayout->addWidget(endBtn);
    rightLayout->addStretch();
    
    mainLayout->addWidget(leftPanel, 1);
    mainLayout->addWidget(logText, 2);
    mainLayout->addWidget(rightPanel, 1);
    
    connect(attackBtn, &QPushButton::clicked, this, &BattleWidget::onAttack);
    connect(moveBtn, &QPushButton::clicked, this, &BattleWidget::onMove);
    connect(skillBtn, &QPushButton::clicked, this, &BattleWidget::onSkill);
    connect(endBtn, &QPushButton::clicked, this, &BattleWidget::onEndTurn);
    connect(skillList, &QListWidget::currentRowChanged, this, &BattleWidget::onSkillSelected);
    connect(moveList, &QListWidget::currentRowChanged, this, &BattleWidget::onMoveSelected);
    
    moveList->setVisible(false);
    skillList->setVisible(false);
    
    addLog("========== 游戏开始 ==========");
    addLog(QString("第%1回合").arg(round));
    addLog("玩家1回合开始");
    updateUI();
}

void BattleWidget::addLog(const QString& msg){
    logText->append(msg);
}

void BattleWidget::updateUI(){
    hpBar1->setValue(player1->health);
    hpBar1->setFormat(QString("%1/%2").arg(player1->health).arg(player1->maxHealth));
    hpBar2->setValue(player2->health);
    hpBar2->setFormat(QString("%1/%2").arg(player2->health).arg(player2->maxHealth));
    
    apLabel1->setText(QString("行动点: %1").arg(player1->action));
    apLabel2->setText(QString("行动点: %1").arg(player2->action));
    posLabel1->setText(QString("位置: %1").arg(player1->pos));
    posLabel2->setText(QString("位置: %1").arg(player2->pos));
    weaponLabel1->setText(QString("武器: %1").arg(player1->getWeaponName()));
    weaponLabel2->setText(QString("武器: %1").arg(player2->getWeaponName()));
    
    buffLabel1->setText(getBuffText(*player1));
    buffLabel2->setText(getBuffText(*player2));
    
    if(currentPlayer == player1){
        attackBtn->setEnabled(currentPlayer->action >= 2);
        moveBtn->setEnabled(currentPlayer->action >= 1);
        skillBtn->setEnabled(currentPlayer->action >= 2 && !currentPlayer->skillIds.isEmpty());
    } else {
        attackBtn->setEnabled(currentPlayer->action >= 2);
        moveBtn->setEnabled(currentPlayer->action >= 1);
        skillBtn->setEnabled(currentPlayer->action >= 2 && !currentPlayer->skillIds.isEmpty());
    }
}

QString BattleWidget::getBuffText(Player& player){
    QStringList buffs;
    if(player.pohe.val > 0) buffs << QString("回血+%1").arg(player.pohe.val);
    if(player.nehe.val > 0) buffs << QString("流血-%1").arg(player.nehe.val);
    if(player.poac.val > 0) buffs << QString("行动+%1").arg(player.poac.val);
    if(player.neac.val > 0) buffs << QString("行动-%1").arg(player.neac.val);
    if(player.poda.val > 0) buffs << QString("伤害+%1%").arg(player.poda.val);
    if(player.neda.val > 0) buffs << QString("减伤+%1%").arg(player.neda.val);
    return buffs.join(" ");
}

void BattleWidget::onAttack(){
    if(currentPlayer->action < 2){
        addLog("行动点不足！");
        return;
    }
    
    currentPlayer->action -= 2;
    int dmg = currentPlayer->getWeaponDamage();
    
    addLog(QString("%1 发起攻击！").arg(currentPlayer == player1 ? "玩家1" : "玩家2"));
    
    if(currentPlayer->weaponId == 3){
        int dmg1 = 12 * (100 + currentPlayer->poda.val - opponent->neda.val) / 100;
        opponent->takeDamage(dmg1);
        addLog(QString("第一次攻击造成 %1 伤害").arg(dmg1));
        applyOnAttackEffects(*currentPlayer, *opponent, dmg1);
        
        if(opponent->isAlive()){
            int dmg2 = 12 * (100 + currentPlayer->poda.val - opponent->neda.val) / 100;
            opponent->takeDamage(dmg2);
            addLog(QString("第二次攻击造成 %1 伤害").arg(dmg2));
            applyOnAttackEffects(*currentPlayer, *opponent, dmg2);
        }
    } else {
        dmg = dmg * (100 + currentPlayer->poda.val - opponent->neda.val) / 100;
        opponent->takeDamage(dmg);
        addLog(QString("造成 %1 伤害").arg(dmg));
        applyOnAttackEffects(*currentPlayer, *opponent, dmg);
    }
    
    applyOnDamagedEffects(*opponent, *currentPlayer, dmg);
    
    updateUI();
    checkGameOver();
}

void BattleWidget::onMove(){
    moveList->setVisible(true);
    skillList->setVisible(false);
}

void BattleWidget::onSkill(){
    if(currentPlayer->skillIds.isEmpty()){
        addLog("没有可用技能！");
        return;
    }
    
    skillList->clear();
    for(int id : currentPlayer->skillIds){
        QString name = GameData::getSkillName(id);
        if(!name.isEmpty()){
            skillList->addItem(name);
        }
    }
    skillList->setVisible(true);
    moveList->setVisible(false);
}

void BattleWidget::onSkillSelected(int row){
    if(row < 0 || row >= currentPlayer->skillIds.size()) return;
    
    if(currentPlayer->action < 2){
        addLog("行动点不足！");
        return;
    }
    
    int skillId = currentPlayer->skillIds[row];
    
    currentPlayer->action -= 2;
    
    if(skillId == 4){
        KnockbackSkill().use(*currentPlayer, *opponent);
        addLog("使用技能: 随波逐流 - 击退目标");
    } else if(skillId == 5){
        HealSkill().use(*currentPlayer, *opponent);
        addLog("使用技能: 治疗 - 恢复15生命");
    } else if(skillId == 6){
        SlowSkill().use(*currentPlayer, *opponent);
        addLog("使用技能: 抑制 - 目标行动力降低");
    } else if(skillId == 7){
        FireballSkill().use(*currentPlayer, *opponent);
        addLog("使用技能: 炎爆术");
    } else if(skillId == 8){
        ShieldSkill().use(*currentPlayer, *opponent);
        addLog("使用技能: 神圣壁垒 - 减伤50%");
    } else if(skillId == 9){
        BloodSkill().use(*currentPlayer, *opponent);
        addLog("使用技能: 燃血 - 伤害增加但每回合扣血");
    }
    
    skillList->setVisible(false);
    updateUI();
    checkGameOver();
}

void BattleWidget::onMoveSelected(int row){
    if(row < 0 || row >= moveList->count()) return;
    
    if(currentPlayer->action < 1){
        addLog("行动点不足！");
        return;
    }
    
    currentPlayer->action -= 1;
    int moveDist = (row == 0) ? -5 : 5;
    currentPlayer->move(moveDist);
    addLog(QString("移动到位置 %1").arg(currentPlayer->pos));
    
    moveList->setVisible(false);
    updateUI();
}

void BattleWidget::onEndTurn(){
    addLog("-------- 回合结束 --------");
    switchPlayer();
}

void BattleWidget::switchPlayer(){
    if(currentPlayer == player1){
        currentPlayer = player2;
        opponent = player1;
        addLog("玩家2回合开始");
    } else {
        currentPlayer = player1;
        opponent = player2;
        round++;
        addLog(QString("\n========== 第%1回合 ==========").arg(round));
        addLog("玩家1回合开始");
        
        player1->endRound();
        player2->endRound();
        
        player1->startRound();
        player2->startRound();
        
        applyEquipmentEffects(*player1);
        applyEquipmentEffects(*player2);
    }
    
    updateUI();
}

void BattleWidget::applyEquipmentEffects(Player& player){
    for(int id : player.equipIds){
        Equipment* equip = GameData::getEquipment(id);
        if(equip) equip->onRoundStart(player);
    }
}

void BattleWidget::applyOnAttackEffects(Player& attacker, Player& target, int dmg){
    for(int id : attacker.equipIds){
        Equipment* equip = GameData::getEquipment(id);
        if(equip) equip->onAttack(attacker, target, dmg);
    }
}

void BattleWidget::applyOnDamagedEffects(Player& target, Player& attacker, int dmg){
    for(int id : target.equipIds){
        Equipment* equip = GameData::getEquipment(id);
        if(equip) equip->onDamaged(target, attacker, dmg);
    }
}

void BattleWidget::checkGameOver(){
    if(!player1->isAlive()){
        addLog("\n========== 游戏结束 ==========");
        addLog("玩家2 胜利！");
        gameRunning = false;
        attackBtn->setEnabled(false);
        moveBtn->setEnabled(false);
        skillBtn->setEnabled(false);
        endBtn->setEnabled(false);
        QMessageBox::information(this, "游戏结束", "玩家2 胜利！");
    } else if(!player2->isAlive()){
        addLog("\n========== 游戏结束 ==========");
        addLog("玩家1 胜利！");
        gameRunning = false;
        attackBtn->setEnabled(false);
        moveBtn->setEnabled(false);
        skillBtn->setEnabled(false);
        endBtn->setEnabled(false);
        QMessageBox::information(this, "游戏结束", "玩家1 胜利！");
    }
}