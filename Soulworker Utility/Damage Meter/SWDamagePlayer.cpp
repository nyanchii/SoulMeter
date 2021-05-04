#include "pch.h"
#include ".\Damage Meter\SWDamageMonster.h"
#include ".\Damage Meter\SWDamagePlayer.h"
#include ".\Damage Meter\Damage Meter.h"

SWDamagePlayer::SWDamagePlayer(UINT32 id, UINT64 damage, UINT64 critDamage, USHORT hitCount, USHORT critHitCount, USHORT maxCombo, UINT32 monsterID, UINT32 skillID) {
	_id = id;
	_damage = damage;
	_critDamage = critDamage;
	_hitCount = hitCount;
	_critHitCount = critHitCount;
	_maxCombo = maxCombo;

#if DEBUG_DAMAGE_PLAYER == 1
	Log::WriteLog(const_cast<LPTSTR>(_T("[PLAYER] [ID = %d] [DMG = %llu] [cirDMG = %llu] [hitCount = %d] [cirtHitCount = %d] [maxCombo = %d]")), _id, _damage, _critDamage, _hitCount, _critHitCount, _maxCombo);
#endif

	InsertMonsterInfo(monsterID, damage, critDamage, hitCount, critHitCount, skillID);
}

SWDamagePlayer::~SWDamagePlayer() {

	for (auto itr = _monsterInfo.begin(); itr != _monsterInfo.end(); itr++) {
		delete (*itr);
	}

	_monsterInfo.clear();
}

BOOL SWDamagePlayer::SortFunction(SWDamagePlayer* playerA, SWDamagePlayer* playerB) {
	return playerA->GetDamage() > playerB->GetDamage();
}

VOID SWDamagePlayer::InsertMonsterInfo(UINT32 monsterID, UINT64 damage, UINT64 critDamage, USHORT hitCount, USHORT critHitCount, UINT32 skillID) {

	auto itr = _monsterInfo.begin();

	for (; itr != _monsterInfo.end(); itr++) {
		if (monsterID == (*itr)->GetID()) {
			(*itr)->AddDamage(damage, critDamage, hitCount, critHitCount, skillID);
			return;
		}
	}

	SW_DB2_STRUCT* db = DAMAGEMETER.GetMonsterDB(monsterID);
	USHORT db1 = 0;
	UINT32 db2 = 0;

	if (db != nullptr) {
		db1 = db->_db1;
		db2 = db->_db2;
	}
	
	_monsterInfo.push_back(new SWDamageMonster(monsterID, db1, db2, damage, critDamage, hitCount, critHitCount, skillID));
}

VOID SWDamagePlayer::Sort() {
	sort(_monsterInfo.begin(), _monsterInfo.end(), SWDamageMonster::SortFunction);
}

VOID SWDamagePlayer::AddDamage(UINT64 damage, UINT64 critDamage, USHORT hitCount, USHORT critHitCount, USHORT maxCombo, UINT32 monsterID, UINT32 skillID) {
	_damage += damage;
	_critDamage += critDamage;
	_hitCount += hitCount;
	_critHitCount += critHitCount;
	
	if (maxCombo > _maxCombo)
		_maxCombo = maxCombo;

#if DEBUG_DAMAGE_PLAYER == 1
	Log::WriteLog(const_cast<LPTSTR>(_T("[PLAYER] [ID = %d] [DMG = %llu] [cirDMG = %llu] [hitCount = %d] [cirtHitCount = %d] [maxCombo = %d]")), _id, _damage, _critDamage, _hitCount, _critHitCount, _maxCombo);
#endif

	InsertMonsterInfo(monsterID, damage, critDamage, hitCount, critHitCount, skillID);
	Sort();
}

UINT32 SWDamagePlayer::GetID() {
	return _id;
}

UINT64 SWDamagePlayer::GetDamage() {
	return _damage;
}

UINT64 SWDamagePlayer::GetCritDamage() {
	return _critDamage;
}

USHORT SWDamagePlayer::GetHitCount() {
	return _hitCount;
}

USHORT SWDamagePlayer::GetCritHitCount() {
	return _critHitCount;
}

USHORT SWDamagePlayer::GetMaxCombo() {
	return _maxCombo;
}

UINT64 SWDamagePlayer::GetMonsterTotalDamage() {
	
	UINT64 monsterTotalDamage = 0;

	for (auto itr = _monsterInfo.begin(); itr != _monsterInfo.end(); itr++)
		monsterTotalDamage += (*itr)->GetDamage();

	return monsterTotalDamage;
}

vector<SWDamageMonster*>::const_iterator SWDamagePlayer::GetMonsterInfo(UINT id) {

	auto itr = _monsterInfo.begin();

	for (; itr != _monsterInfo.end(); itr++) {
		if ((*itr)->GetID() == id) {
			return itr;
		}
	}

	return itr;
}

vector<SWDamageMonster*>::const_iterator SWDamagePlayer::begin() {
	return _monsterInfo.begin();
}

vector<SWDamageMonster*>::const_iterator SWDamagePlayer::end() {
	return _monsterInfo.end();
}

const SIZE_T& SWDamagePlayer::size() {
	return _monsterInfo.size();
}