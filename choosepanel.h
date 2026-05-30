#ifndef CHOOSEPANEL_H
#define CHOOSEPANEL_H

#include <QWidget>
#include "gamecore.h"

class QListWidget;
class QPushButton;
class QLabel;

class ChoosePanel : public QWidget
{
    Q_OBJECT

public:
    explicit ChoosePanel(Player* player, int playerNum, QWidget *parent = nullptr);

signals:
    void choiceCompleted();

private slots:
    void addWeapon();
    void addSkill();
    void addEquipment();
    void onConfirm();

private:
    Player* targetPlayer;
    int playerNumber;
    int selectedCount;
    bool hasWeapon;
    
    QListWidget *selectedList;
    QLabel *statusLabel;
    QPushButton *weaponBtn;
    QPushButton *skillBtn;
    QPushButton *equipBtn;
    QPushButton *confirmBtn;
    
    void updateStatus();
    void showItemDialog(const QString& title, const QVector<QPair<int, QString>>& items, int type);
};

#endif