#ifndef GAMECORE_H
#define GAMECORE_H

#include <QString>
#include <QVector>
#include <QRandomGenerator>

struct Buff{
    int val,time;
    Buff(){val=time=0;}
    void apply(){
        if(time>0)time--;
        if(time<=0)val=0;
    }
};

class Player{
public:
    int health,action,pos;
    int maxHealth;
    int weaponId;
    QVector<int> equipIds;
    QVector<int> skillIds;
    
    Buff pohe,poac,poda,nehe,neac,neda;
    
    Player(int side);
    void startRound();
    void endRound();
    void move(int val);
    bool isAlive() const { return health > 0; }
    void addHeal(int val);
    void takeDamage(int dmg);
    QString getWeaponName() const;
    int getWeaponDamage();
};

class Weapon{
public:
    int id;
    QString name;
    QString desc;
    int baseDamage;
    Weapon(int i, const QString& n, const QString& d, int dmg);
    virtual int attack(){ return baseDamage; }
    virtual bool isDoubleHit() const { return false; }
};

class Hammer : public Weapon{
public:
    Hammer();
    int attack() override;
};

class Dagger : public Weapon{
public:
    Dagger();
    bool isDoubleHit() const override { return true; }
};

class Skill{
public:
    int id,reload;
    int currentCooldown;
    QString name,desc;
    Skill(int i, const QString& n, const QString& d, int cd);
    virtual void use(Player& from, Player& to) = 0;
    void tickCooldown();
    bool isReady() const { return currentCooldown == 0; }
    void resetCooldown() { currentCooldown = reload; }
};

class KnockbackSkill : public Skill{
public:
    KnockbackSkill();
    void use(Player& from, Player& to) override;
};

class HealSkill : public Skill{
public:
    HealSkill();
    void use(Player& from, Player& to) override;
};

class SlowSkill : public Skill{
public:
    SlowSkill();
    void use(Player& from, Player& to) override;
};

class FireballSkill : public Skill{
public:
    FireballSkill();
    void use(Player& from, Player& to) override;
};

class ShieldSkill : public Skill{
public:
    ShieldSkill();
    void use(Player& from, Player& to) override;
};

class BloodSkill : public Skill{
public:
    BloodSkill();
    void use(Player& from, Player& to) override;
};

class Equipment{
public:
    int id;
    QString name,desc;
    Equipment(int i, const QString& n, const QString& d);
    virtual void onRoundStart(Player& user) {}
    virtual void onAttack(Player& user, Player& target, int dmg) {}
    virtual void onDamaged(Player& user, Player& attacker, int dmg) {}
};

class IronWill : public Equipment{
public:
    IronWill();
    void onRoundStart(Player& user) override;
};

class SwiftBoots : public Equipment{
public:
    SwiftBoots();
    void onRoundStart(Player& user) override;
};

class LifeFountain : public Equipment{
public:
    LifeFountain();
    void onRoundStart(Player& user) override;
};

class ThornArmor : public Equipment{
public:
    ThornArmor();
    void onDamaged(Player& user, Player& attacker, int dmg) override;
};

class VampireMask : public Equipment{
public:
    VampireMask();
    void onAttack(Player& user, Player& target, int dmg) override;
};

class GameData{
public:
    static QVector<Weapon*> weapons;
    static QVector<Skill*> allSkills;
    static QVector<Equipment*> equipments;
    
    static void init();
    static Weapon* getWeapon(int id);
    static Skill* getSkill(int id);
    static Equipment* getEquipment(int id);
    static QString getWeaponName(int id);
    static QString getSkillName(int id);
    static QString getEquipName(int id);
};

#endif