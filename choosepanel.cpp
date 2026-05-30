#include "choosepanel.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QListWidget>
#include <QPushButton>
#include <QLabel>
#include <QMessageBox>
#include <QDialog>
#include <QDialogButtonBox>

ChoosePanel::ChoosePanel(Player* player, int playerNum, QWidget *parent)
    : QWidget(parent), targetPlayer(player), playerNumber(playerNum), 
      selectedCount(0), hasWeapon(false)
{
    setStyleSheet(R"(
        QListWidget {
            background-color: #16213e;
            color: #c4a747;
            border: 2px solid #c4a747;
            border-radius: 5px;
        }
        QListWidget::item:selected {
            background-color: #c4a747;
            color: #16213e;
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
    
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    
    QLabel *titleLabel = new QLabel(QString("玩家%1 选择6件物品").arg(playerNumber));
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet("font-size: 24px; font-weight: bold; margin: 20px;");
    
    statusLabel = new QLabel("已选 0/6 件 (武器未选)");
    statusLabel->setAlignment(Qt::AlignCenter);
    
    selectedList = new QListWidget();
    
    QHBoxLayout *btnLayout = new QHBoxLayout();
    weaponBtn = new QPushButton("选择武器");
    skillBtn = new QPushButton("选择技能");
    equipBtn = new QPushButton("选择防具");
    confirmBtn = new QPushButton("确认");
    confirmBtn->setEnabled(false);
    
    btnLayout->addWidget(weaponBtn);
    btnLayout->addWidget(skillBtn);
    btnLayout->addWidget(equipBtn);
    btnLayout->addWidget(confirmBtn);
    
    mainLayout->addWidget(titleLabel);
    mainLayout->addWidget(statusLabel);
    mainLayout->addWidget(selectedList);
    mainLayout->addLayout(btnLayout);
    
    connect(weaponBtn, &QPushButton::clicked, this, &ChoosePanel::addWeapon);
    connect(skillBtn, &QPushButton::clicked, this, &ChoosePanel::addSkill);
    connect(equipBtn, &QPushButton::clicked, this, &ChoosePanel::addEquipment);
    connect(confirmBtn, &QPushButton::clicked, this, &ChoosePanel::onConfirm);
}

void ChoosePanel::addWeapon(){
    if(hasWeapon){
        QMessageBox::warning(this, "提示", "已经选择过武器了！");
        return;
    }
    
    QVector<QPair<int, QString>> weapons;
    weapons.append({1, "短刀 - 普攻伤害20"});
    weapons.append({2, "战锤 - 普攻伤害25~35"});
    weapons.append({3, "影击匕首 - 普攻伤害12，连击两次"});
    
    showItemDialog("选择武器", weapons, 1);
}

void ChoosePanel::addSkill(){
    QVector<QPair<int, QString>> skills;
    skills.append({4, "随波逐流 - 击退目标7距离，冷却4回合"});
    skills.append({5, "治疗 - 恢复15生命，冷却3回合"});
    skills.append({6, "抑制 - 目标行动力减少2持续3回合，冷却2回合"});
    skills.append({7, "炎爆术 - 造成25伤害并附加灼烧3回合，冷却5回合"});
    skills.append({8, "神圣壁垒 - 减伤50%持续2回合，冷却4回合"});
    skills.append({9, "燃血 - 伤害+30%但每回合扣5血持续3回合，冷却6回合"});
    
    showItemDialog("选择技能", skills, 2);
}

void ChoosePanel::addEquipment(){
    QVector<QPair<int, QString>> equips;
    equips.append({16, "不屈意志 - 生命低于50%时减伤30%"});
    equips.append({17, "疾风战靴 - 每回合+1行动点"});
    equips.append({18, "生命之泉 - 每回合回复5生命"});
    equips.append({19, "荆棘之甲 - 受到攻击反弹25%伤害"});
    equips.append({23, "吸血面罩 - 攻击回复20%伤害的生命"});
    
    showItemDialog("选择防具", equips, 3);
}

void ChoosePanel::showItemDialog(const QString& title, const QVector<QPair<int, QString>>& items, int type){
    QDialog dialog(this);
    dialog.setWindowTitle(title);
    dialog.setMinimumSize(400, 300);
    
    QVBoxLayout *layout = new QVBoxLayout(&dialog);
    
    QListWidget *listWidget = new QListWidget();
    for(const auto& item : items){
        listWidget->addItem(item.second);
    }
    
    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    
    layout->addWidget(listWidget);
    layout->addWidget(buttonBox);
    
    connect(buttonBox, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);
    
    if(dialog.exec() == QDialog::Accepted){
        int index = listWidget->currentRow();
        if(index >= 0 && index < items.size()){
            int id = items[index].first;
            QString name = items[index].second;
            
            if(type == 1){
                targetPlayer->weaponId = id;
                hasWeapon = true;
                selectedList->addItem(QString("武器: %1").arg(name));
                selectedCount++;
            } else if(type == 2){
                targetPlayer->skillIds.append(id);
                selectedList->addItem(QString("技能: %1").arg(name));
                selectedCount++;
            } else if(type == 3){
                targetPlayer->equipIds.append(id);
                selectedList->addItem(QString("防具: %1").arg(name));
                selectedCount++;
            }
            
            updateStatus();
        }
    }
}

void ChoosePanel::updateStatus(){
    QString weaponStatus = hasWeapon ? "已选" : "未选";
    statusLabel->setText(QString("已选 %1/6 件 (武器%2)").arg(selectedCount).arg(weaponStatus));
    
    if(selectedCount == 6){
        confirmBtn->setEnabled(true);
        weaponBtn->setEnabled(false);
        skillBtn->setEnabled(false);
        equipBtn->setEnabled(false);
    }
}

void ChoosePanel::onConfirm(){
    if(selectedCount == 6){
        emit choiceCompleted();
    }
}