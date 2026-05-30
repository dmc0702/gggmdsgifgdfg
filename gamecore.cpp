#include "gamecore.h"

Player::Player(int side){
    health = maxHealth = 100;
    action = 5;
    pos = side * 5;
    weaponId = 0;
}

void Player::startRound(){
    health += pohe.val - nehe.val;
    if(health > maxHealth) health = maxHealth;
    action = 5 + poac.val - neac.val;
    if(action < 0) action = 0;
}

void Player::endRound(){
    pohe.apply();
    poac.apply();
    poda.apply();
    nehe.apply();
    neac.apply();
    neda.apply();
}

void Player::move(int val){
    pos += val;
    if(pos < 0) pos = 0;
    if(pos > 100) pos = 100;
}

void Player::addHeal(int val){
    health += val;
    if(health > maxHealth) health = maxHealth;
}

void Player::takeDamage(int dmg){
    int finalDmg = dmg * (100 + poda.val - neda.val) / 100;
    health -= finalDmg;
}

QString Player::getWeaponName() const{
    if(weaponId == 1) return "短刀";
    if(weaponId == 2) return "战锤";
    if(weaponId == 3) return "影击匕首";
    return "无";
}

int Player::getWeaponDamage(){
    if(weaponId == 1) return 20;
    if(weaponId == 2){
        return 30 + (QRandomGenerator::global()->bounded(11) - 5);
    }
    if(weaponId == 3) return 12;
    return 15;
}

Weapon::Weapon(int i, const QString& n, const QString& d, int dmg)
    : id(i), name(n), desc(d), baseDamage(dmg) {}

Hammer::Hammer() : Weapon(2, "战锤", "普攻伤害25~35", 30) {}
int Hammer::attack(){ return 30 + (QRandomGenerator::global()->bounded(11) - 5); }

Dagger::Dagger() : Weapon(3, "影击匕首", "普攻伤害12，连击两次", 12) {}

Skill::Skill(int i, const QString& n, const QString& d, int cd)
    : id(i), name(n), desc(d), reload(cd), currentCooldown(0) {}

void Skill::tickCooldown(){
    if(currentCooldown > 0) currentCooldown--;
}

KnockbackSkill::KnockbackSkill() : Skill(4, "随波逐流", "击退目标7距离", 4) {}
void KnockbackSkill::use(Player& from, Player& to){
    if(to.pos > from.pos) to.move(7);
    else if(to.pos < from.pos) to.move(-7);
}

HealSkill::HealSkill() : Skill(5, "治疗", "恢复15生命", 3) {}
void HealSkill::use(Player& from, Player& to){
    from.addHeal(15);
}

SlowSkill::SlowSkill() : Skill(6, "抑制", "目标行动力减少2持续3回合", 2) {}
void SlowSkill::use(Player& from, Player& to){
    to.neac.val = 2;
    to.neac.time = 3;
}

FireballSkill::FireballSkill() : Skill(7, "炎爆术", "造成25伤害并附加灼烧3回合", 5) {}
void FireballSkill::use(Player& from, Player& to){
    int dmg = 25 * (100 + from.poda.val - to.neda.val) / 100;
    to.takeDamage(dmg);
    to.nehe.val = 5;
    to.nehe.time = 3;
}

ShieldSkill::ShieldSkill() : Skill(8, "神圣壁垒", "减伤50%持续2回合", 4) {}
void ShieldSkill::use(Player& from, Player& to){
    from.neda.val = 50;
    from.neda.time = 2;
}

BloodSkill::BloodSkill() : Skill(9, "燃血", "伤害+30%但每回合扣5血持续3回合", 6) {}
void BloodSkill::use(Player& from, Player& to){
    from.poda.val = 30;
    from.poda.time = 3;
    from.nehe.val = 5;
    from.nehe.time = 3;
}

Equipment::Equipment(int i, const QString& n, const QString& d)
    : id(i), name(n), desc(d) {}

IronWill::IronWill() : Equipment(16, "不屈意志", "生命低于50%时减伤30%") {}
void IronWill::onRoundStart(Player& user){
    if(user.health <= 50){
        user.neda.val = 30;
        user.neda.time = 5;
    }
}

SwiftBoots::SwiftBoots() : Equipment(17, "疾风战靴", "每回合+1行动点") {}
void SwiftBoots::onRoundStart(Player& user){
    user.poac.val = 1;
    user.poac.time = 999;
}

LifeFountain::LifeFountain() : Equipment(18, "生命之泉", "每回合回复5生命") {}
void LifeFountain::onRoundStart(Player& user){
    user.pohe.val = 5;
    user.pohe.time = 999;
}

ThornArmor::ThornArmor() : Equipment(19, "荆棘之甲", "受到攻击反弹25%伤害") {}
void ThornArmor::onDamaged(Player& user, Player& attacker, int dmg){
    int reflect = dmg * 0.25;
    attacker.takeDamage(reflect);
}

VampireMask::VampireMask() : Equipment(23, "吸血面罩", "攻击回复20%伤害的生命") {}
void VampireMask::onAttack(Player& user, Player& target, int dmg){
    int heal = dmg * 0.2;
    user.addHeal(heal);
}

QVector<Weapon*> GameData::weapons;
QVector<Skill*> GameData::allSkills;
QVector<Equipment*> GameData::equipments;

void GameData::init(){
    weapons.clear();
    weapons.append(new Weapon(1, "短刀", "普攻伤害20", 20));
    weapons.append(new Hammer());
    weapons.append(new Dagger());
    
    allSkills.clear();
    allSkills.append(new KnockbackSkill());
    allSkills.append(new HealSkill());
    allSkills.append(new SlowSkill());
    allSkills.append(new FireballSkill());
    allSkills.append(new ShieldSkill());
    allSkills.append(new BloodSkill());
    
    equipments.clear();
    equipments.append(new IronWill());
    equipments.append(new SwiftBoots());
    equipments.append(new LifeFountain());
    equipments.append(new ThornArmor());
    equipments.append(new VampireMask());
}

Weapon* GameData::getWeapon(int id){
    for(auto w : weapons) if(w->id == id) return w;
    return nullptr;
}

Skill* GameData::getSkill(int id){
    for(auto s : allSkills) if(s->id == id) return s;
    return nullptr;
}

Equipment* GameData::getEquipment(int id){
    for(auto e : equipments) if(e->id == id) return e;
    return nullptr;
}

QString GameData::getWeaponName(int id){
    if(id == 1) return "短刀";
    if(id == 2) return "战锤";
    if(id == 3) return "影击匕首";
    return "无";
}

QString GameData::getSkillName(int id){
    if(id == 4) return "随波逐流";
    if(id == 5) return "治疗";
    if(id == 6) return "抑制";
    if(id == 7) return "炎爆术";
    if(id == 8) return "神圣壁垒";
    if(id == 9) return "燃血";
    return "";
}

QString GameData::getEquipName(int id){
    if(id == 16) return "不屈意志";
    if(id == 17) return "疾风战靴";
    if(id == 18) return "生命之泉";
    if(id == 19) return "荆棘之甲";
    if(id == 23) return "吸血面罩";
    return "";
}