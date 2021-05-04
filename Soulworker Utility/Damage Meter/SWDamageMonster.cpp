#include "pch.h"
#include ".\Damage Meter\SWDamageSkill.h"
#include ".\Damage Meter\SWDamageMonster.h"
#include ".\Damage Meter\MySQLite.h"

SWDamageMonster::SWDamageMonster(UINT32 id, USHORT db1, UINT32 db2, UINT64 damage, UINT64 critDamage, USHORT hitCount, USHORT critHitCount, UINT32 skillID) {
	_id = id;
	_db1 = db1;
	_db2 = db2;
	_damage = damage;
	_critDamage = critDamage;
	_hitCount = hitCount;
	_critHitCount = critHitCount;

	ZeroMemory(_name, MONSTER_NAME_LEN);
	SWDB.GetMonsterName(db1, db2, _name, MONSTER_NAME_LEN);

#if DEBUG_DAMAGE_MONSTER == 1
	Log::WriteLog(const_cast<LPTSTR>(_T("\t[MONSTER] [ID = %d] [DB1 = %d] [DB2 = %d] [NAME = %s] [DMG = %llu] [cirDMG = %llu] [hitCount = %d] [cirtHitCount = %d]")), _id, _db1, _db2, _name, _damage, _critDamage, _hitCount, _critHitCount);
#endif
	

	InsertSkillInfo(skillID, damage, critDamage, hitCount, critHitCount);
}

SWDamageMonster::~SWDamageMonster() {

	for (auto itr = _skillinfo.begin(); itr != _skillinfo.end(); itr++) {
		delete (*itr);
	}

	_skillinfo.clear();
}

BOOL SWDamageMonster::SortFunction(SWDamageMonster* monsterA, SWDamageMonster* monsterB) {
	return monsterA->GetDamage() > monsterB->GetDamage();
}

VOID SWDamageMonster::AddDamage(UINT64 damage, UINT64 critDamage, USHORT hitCount, USHORT critHitCount, UINT32 skillID) {
	_damage += damage;
	_critDamage += critDamage;
	_hitCount += hitCount;
	_critHitCount += critHitCount;

#if DEBUG_DAMAGE_MONSTER == 1
	Log::WriteLog(const_cast<LPTSTR>(_T("\t[MONSTER] [ID = %d] [DB1 = %d] [DB2 = %d] [NAME = %s] [DMG = %llu] [cirDMG = %llu] [hitCount = %d] [cirtHitCount = %d]")), _id, _db1, _db2, _name, _damage, _critDamage, _hitCount, _critHitCount);
#endif

	InsertSkillInfo(skillID, damage, critDamage, hitCount, critHitCount);
	Sort();
}

VOID SWDamageMonster::InsertSkillInfo(UINT32 skillID, UINT64 damage, UINT64 critDamage, USHORT hitCount, USHORT critHitCount) {

	auto itr = _skillinfo.begin();

	for (; itr != _skillinfo.end(); itr++) {
		if (skillID == (*itr)->GetID()) {
			(*itr)->AddDamage(damage, critDamage, hitCount, critHitCount);
			return;
		}
	}

	_skillinfo.push_back(new SWDamageSkill(skillID, damage, critDamage, hitCount, critHitCount));
}

VOID SWDamageMonster::Sort() {
	sort(_skillinfo.begin(), _skillinfo.end(), SWDamageSkill::SortFunction);
}

UINT32 SWDamageMonster::GetID() {
	return _id;
}

USHORT SWDamageMonster::GetDB1() {
	return _db1;
}

UINT32 SWDamageMonster::GetDB2() {
	return _db2;
}

UINT64 SWDamageMonster::GetDamage() {
	return _damage;
}

UINT64 SWDamageMonster::GetCritDamage() {
	return _critDamage;
}

USHORT SWDamageMonster::GetHitCount() {
	return _hitCount;
}

USHORT SWDamageMonster::GetCritHitCount() {
	return _critHitCount;
}

UINT64 SWDamageMonster::GetSkillTotalDamage() {
	UINT64 skillTotalDamage = 0;

	for (auto itr = _skillinfo.begin(); itr != _skillinfo.end(); itr++)
		skillTotalDamage += (*itr)->GetDamage();

	return skillTotalDamage;
}

const CHAR* SWDamageMonster::GetName() {
	return _name;
}

vector<SWDamageSkill*>::const_iterator SWDamageMonster::begin() {
	return _skillinfo.begin();
}

vector<SWDamageSkill*>::const_iterator SWDamageMonster::end() {
	return _skillinfo.end();
}

const SIZE_T& SWDamageMonster::size() {
	return _skillinfo.size();
}