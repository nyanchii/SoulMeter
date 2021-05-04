#include "pch.h"
#include ".\Damage Meter\SWDamageSkill.h"
#include ".\Damage Meter\MySQLite.h"

SWDamageSkill::SWDamageSkill(UINT32 id, UINT64 damage, UINT64 critDamage, USHORT hitCount, USHORT critHitCount){
	_id = id;
	_damage = damage;
	_critDamage = critDamage;
	_hitCount = hitCount;
	_critHitCount = critHitCount;

	ZeroMemory(_name, SKILL_NAME_LEN);
	SWDB.GetSkillName(id, _name, SKILL_NAME_LEN);

#if DEBUG_DAMAGE_SKILL == 1
	Log::WriteLog(const_cast<LPTSTR>(_T("\t\t[SKILL] [ID = %d] [NAME = %s] [DMG = %llu] [cirDMG = %llu] [hitCount = %d] [cirtHitCount = %d]")), _id, _name,_damage, _critDamage, _hitCount, _critHitCount);
#endif
}

BOOL SWDamageSkill::SortFunction(SWDamageSkill* skillA, SWDamageSkill* skillB) {
	return skillA->GetDamage() > skillB->GetDamage();
}

VOID SWDamageSkill::AddDamage(UINT64 damage, UINT64 critDamage, USHORT hitCount, USHORT critHitCount) {
	_damage += damage;
	_critDamage += critDamage;
	_hitCount += hitCount;
	_critHitCount += critHitCount;

#if DEBUG_DAMAGE_SKILL == 1
	Log::WriteLog(const_cast<LPTSTR>(_T("\t\t[SKILL] [ID = %d] [NAME = %s] [DMG = %llu] [cirDMG = %llu] [hitCount = %d] [cirtHitCount = %d]")), _id, _name, _damage, _critDamage, _hitCount, _critHitCount);
#endif
}

UINT32 SWDamageSkill::GetID() {
	return _id;
}

UINT64 SWDamageSkill::GetDamage() {
	return _damage;
}

UINT64 SWDamageSkill::GetCritDamage() {
	return _critDamage;
}

USHORT SWDamageSkill::GetHitCount() {
	return _hitCount;
}

USHORT SWDamageSkill::GetCritHitCount() {
	return _critHitCount;
}

CHAR* SWDamageSkill::GetName() {
	return _name;
}
