#include "pch.h"
#include ".\Damage Meter\SWDamageMonster.h"
#include ".\Damage Meter\SWDamagePlayer.h"
#include ".\Damage Meter\Damage Meter.h"
#include ".\Soulworker Packet\PacketInfo.h"
#include ".\MySQLite.h"
#include ".\UI\Option.h"

SWDamagePlayer::SWDamagePlayer(UINT32 id, UINT64 totalDMG, UINT64 soulstoneDMG, SWPACKETDAMAGE_DAMAGETYPE damageType, USHORT maxCombo, UINT32 monsterID, UINT32 skillID) {
	_id = id;
	AddDamage(totalDMG, soulstoneDMG, damageType, maxCombo, monsterID, skillID);
}

SWDamagePlayer::SWDamagePlayer(UINT32 id) {
	_id = id;
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

	SW_DB2_STRUCT* db = DAMAGEMETER.GetMonsterDB(monsterID);
	//Log::WriteLog(const_cast<LPTSTR>(_T("[Monster] [MonsterID = %d] [DB2 = %d]")), monsterID, db->_db2);

	UINT32 db2 = 0;

	if (db != nullptr) {
		db2 = db->_db2;
	}

	for (; itr != _monsterInfo.end(); itr++) {
		if (db2 == (*itr)->GetDB2()) {
			(*itr)->AddDamage(damage, critDamage, hitCount, critHitCount, skillID);
			return;
		}
	}

	_monsterInfo.push_back(new SWDamageMonster(monsterID, db2, damage, critDamage, hitCount, critHitCount, skillID));
}

VOID SWDamagePlayer::Sort() {
	sort(_monsterInfo.begin(), _monsterInfo.end(), SWDamageMonster::SortFunction);
}

VOID SWDamagePlayer::AddDamage(UINT64 totalDMG, UINT64 soulstoneDMG, SWPACKETDAMAGE_DAMAGETYPE damageType, USHORT maxCombo, UINT32 monsterID, UINT32 skillID)
{
	//Log::WriteLog(const_cast<LPTSTR>(_T("[PLAYER] [DamageType = %d]")), damageType.CRIT);


	// 
	SW_DB2_STRUCT* db = DAMAGEMETER.GetMonsterDB(monsterID);
	UINT32 db2 = 0;
	if (db != nullptr) {
		db2 = db->_db2;
	}

	// 
	if (!damageType.MISS && _id == DAMAGEMETER.GetMyID()) {
		auto metadata = DAMAGEMETER.GetPlayerMetaData(_id);
		if (metadata != nullptr) {
			metadata->HitEnemy();
		}
	}

	_hitCount += 1;
	_critHitCount += damageType.CRIT;
	// 
	if (totalDMG >= 200) {
		_hitCountForCritRate += 1;
		_critHitCountForCritRate += damageType.CRIT;
		_missCount += damageType.MISS;
		/*
		if (damageType.soulstoneType) {
			_soulstoneCount++;
		}
		*/
		if (soulstoneDMG != 0) {
			_soulstoneCount++;
		}

	}
	if (maxCombo > _maxCombo) {
		_maxCombo = maxCombo;
	}

	USHORT worldID = DAMAGEMETER.GetWorldID();

	// 
	// 
	if (worldID == 21018) {
		if (db2 == 31310101 || db2 == 31310102) {
			_damage += totalDMG;
			_soulstoneDamage += soulstoneDMG;
			_damageForSoulstone += totalDMG;
			_soulstoneDamageForSoulstone += soulstoneDMG;
		}
	}
	else if (dpsIgnoreIdList.find(db2) == dpsIgnoreIdList.end()) {
		_damage += totalDMG;
		_soulstoneDamage += soulstoneDMG;

		if (totalDMG >= 200 && (damageType.soulstoneType != 0)) {
			_damageForSoulstone += totalDMG;
			_soulstoneDamageForSoulstone += soulstoneDMG;
		}
	}

#if DEBUG_DAMAGE_PLAYER == 1
	Log::WriteLog(const_cast<LPTSTR>(_T("[PLAYER] [ID = %d] [MonsterID = %04x] [DMG = %llu] [hitCount = %d] [cirtHitCount = %d] [maxCombo = %d]")), _id, monsterID, _damage, _hitCount, _critHitCount, _maxCombo);
#endif

	InsertMonsterInfo(monsterID, totalDMG, soulstoneDMG, 1, damageType.CRIT, skillID);
	Sort();
}

UINT32 SWDamagePlayer::GetID() {
	return _id;
}

UINT64 SWDamagePlayer::GetDamage() {
	return _damage;
}

UINT64 SWDamagePlayer::GetSoulstoneDamage() {
	return _soulstoneDamage;
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

USHORT SWDamagePlayer::GetHitCountForCritRate()
{
	return _hitCountForCritRate;
}

USHORT SWDamagePlayer::GetCritHitCountForCritRate()
{
	return _critHitCountForCritRate;
}

USHORT SWDamagePlayer::GetSoulstoneCount()
{
	return _soulstoneCount;
}

UINT64 SWDamagePlayer::GetDamageForSoulstone()
{
	return _damageForSoulstone;
}

UINT64 SWDamagePlayer::GetSoulStoneDamageForSoulstone()
{
	return _soulstoneDamageForSoulstone;
}

USHORT SWDamagePlayer::GetGetHitAll()
{
	return _getHitAll;
}

USHORT SWDamagePlayer::GetGetHit()
{
	return _getHit;
}

USHORT SWDamagePlayer::GetGetHitBS()
{
	return _getHitBS;
}

USHORT SWDamagePlayer::GetGetHitMissed()
{
	return _getHitMissed;
}

USHORT SWDamagePlayer::GetGetHitMissedReal()
{
	return _getHitMissedReal;
}

USHORT SWDamagePlayer::GetMissCount()
{
	return _missCount;
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

VOID SWDamagePlayer::SetHistoryAvgAB(DOUBLE historyAvgAB)
{
	_historyAvgAB = historyAvgAB;
}

DOUBLE SWDamagePlayer::GetHistoryAvgAB()
{
	return _historyAvgAB;
}

FLOAT SWDamagePlayer::GetEnlightenSum()
{
	return _enlightenSum;
}

USHORT SWDamagePlayer::GetGigaEnlighten()
{
	return _gigaEnlightenProc;
}

USHORT SWDamagePlayer::GetTeraEnlighten()
{
	return _teraEnlightenProc;
}

VOID SWDamagePlayer::setHistoryBS(int type, DOUBLE value)
{
	switch (type) {
	case 90:
		_historygear90 = value;
		break;
	case 50:
		_historygear50 = value;
		break;
	case 1:
		_historyacc01 = value;
		break;
	case 2:
		_historyacc02 = value;
		break;

	}
}

DOUBLE SWDamagePlayer::GetHistoryBS(int type)
{
	switch (type) {
	case 90:
		return _historygear90;
	case 50:
		return _historygear50;
	case 1:
		return _historyacc01;
	case 2:
		return _historyacc02;
	}
	return 0;
}

VOID SWDamagePlayer::setHistoryLosedHP(DOUBLE losedHP)
{
	_historyLosedHP = losedHP;
}

DOUBLE SWDamagePlayer::GetHistoryLosedHP()
{
	return _historyLosedHP;
}

VOID SWDamagePlayer::AddSkillUsed(UINT32 skillId)
{
	_skillCounts++;

	if (skillCounts.find(skillId) == skillCounts.end()) {
		skillCounts.insert(std::make_pair(skillId, 1));
		return;
	}

	skillCounts[skillId] = skillCounts[skillId] + 1;
}

VOID SWDamagePlayer::AddDodgeUsed()
{
	_dodgeCounts++;
}

VOID SWDamagePlayer::AddDeathCount()
{
	_deathCounts++;
}

VOID SWDamagePlayer::AddGetDamage(UINT64 totalDMG, SWPACKETDAMAGE_DAMAGETYPE damageType, UINT32 monsterID, UINT32 skillID)
{
	_getHitAll++;
	if (totalDMG > 0) {
		_getHit++;
		// 
		if (skillID != 1313101016 && skillID != 1313101113) {
			_getHitBS++;
		}

		if (damageType.MISS) {
			_getHitMissedReal++;
		}
	}

	if (damageType.MISS) {
		_getHitMissed++;
	}
}

VOID SWDamagePlayer::AddEnlighten(FLOAT value)
{
	_enlightenSum += value;
	if (value == 5.0) {
		_gigaEnlightenProc++;
	}
	else if (value == 10.0) {
		_teraEnlightenProc++;
	}
}

USHORT SWDamagePlayer::GetSkillUsed()
{
	return _skillCounts;
}

USHORT SWDamagePlayer::GetDodgeUsed()
{
	return _dodgeCounts;
}

USHORT SWDamagePlayer::GetDeathCount()
{
	return _deathCounts;
}

VOID SWDamagePlayer::SetJqStack(BYTE stack)
{
	_JqStack = stack;
}

BYTE SWDamagePlayer::GetJqStack() {
	return _JqStack;
}


vector<SWDamageMonster*>::const_iterator SWDamagePlayer::begin() {
	return _monsterInfo.begin();
}

vector<SWDamageMonster*>::const_iterator SWDamagePlayer::end() {
	return _monsterInfo.end();
}

const SIZE_T SWDamagePlayer::size() {
	return _monsterInfo.size();
}