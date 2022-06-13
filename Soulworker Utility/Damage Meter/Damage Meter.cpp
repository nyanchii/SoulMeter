#include "pch.h"
#include ".\Damage Meter\Timer.h"
#include ".\Damage Meter\SWDamagePlayer.h"
#include ".\Damage Meter\Damage Meter.h"
#include ".\UI\PlotWindow.h"
#include ".\Damage Meter\History.h"
#include ".\Damage Meter\MySQLite.h"

SWDamageMeter::~SWDamageMeter() {

	if (_historyMode)
		Restore();

	for (auto itr = _playerInfo.begin(); itr != _playerInfo.end(); itr++)
		delete (*itr);

	for (auto itr = _ownerInfo.begin(); itr != _ownerInfo.end(); itr++)
		delete (*itr);

	for (auto itr = _dbInfo.begin(); itr != _dbInfo.end(); itr++)
		delete (*itr);

	for (auto itr = _playerMetadata.begin(); itr != _playerMetadata.end(); itr++)
		delete (itr->second);

	_playerInfo.clear();
	_ownerInfo.clear();
	_dbInfo.clear();
	_playerMetadata.clear();

	FreeLock();
}

VOID SWDamageMeter::GetLock() {
	_mutex.lock();
}
VOID SWDamageMeter::FreeLock() {
	_mutex.unlock();
}

VOID SWDamageMeter::InsertPlayerInfo(UINT32 id, UINT64 totalDMG, UINT64 soulstoneDMG, SWPACKETDAMAGE_DAMAGETYPE damageType, USHORT maxCombo, UINT32 monsterID, UINT32 skillID) {
	auto itr = _playerInfo.begin();

	for (; itr != _playerInfo.end(); itr++) {
		if (id == (*itr)->GetID()) {
			(*itr)->AddDamage(totalDMG, soulstoneDMG, damageType, maxCombo, monsterID, skillID);
			return;
		}
	}

	_playerInfo.push_back(new SWDamagePlayer(id, totalDMG, soulstoneDMG, damageType, maxCombo, monsterID, skillID));
}

VOID SWDamageMeter::AddDamage(UINT32 id, UINT64 totalDMG, UINT64 soulstoneDMG, SWPACKETDAMAGE_DAMAGETYPE damageType, USHORT maxCombo, UINT32 monsterID, UINT32 skillID) {
	if (_mazeEnd)
		return;

	// Meter must know mazeId to work correctly
	if (GetWorldID() == 0)
		return;

	SW_DB2_STRUCT* db = DAMAGEMETER.GetMonsterDB(monsterID);
	UINT32 monsterId = 0;
	if (db != nullptr) {
		monsterId = db->_db2;
	}

	if (!isRun()) {
		// Do not start/resume if it is damage to Puppet Orb,Ephnel Octagon, or Nabi Bomb wick
		if (resumeIgnoreIdList.find(monsterId) != resumeIgnoreIdList.end()) {
			return;
		}

		// If it is BS normal, only damage to Boss can start/resume timer.
		if (GetWorldID() == 21018) {
			if ((monsterId != 31310101) && (monsterId != 31310102)) {
				return;
			}
		}
	}

	Start();
	InsertPlayerInfo(id, totalDMG, soulstoneDMG, damageType, maxCombo, monsterID, skillID);
	Sort();
}

VOID SWDamageMeter::AddPlayerGetDamage(UINT32 playerId, UINT64 totalDMG, SWPACKETDAMAGE_DAMAGETYPE damageType, UINT32 monsterID, UINT32 skillID)
{
	if (!isRun() || GetTime() == 0)
		return;

	auto itr = _playerInfo.begin();

	for (; itr != _playerInfo.end(); itr++) {
		if (playerId == (*itr)->GetID()) {
			(*itr)->AddGetDamage(totalDMG, damageType, monsterID, skillID);
			return;
		}
	}

	if (CheckPlayer(playerId)) {
		SWDamagePlayer* newPlayer = new SWDamagePlayer(playerId);
		newPlayer->AddGetDamage(totalDMG, damageType, monsterID, skillID);
		_playerInfo.push_back(newPlayer);
	}
}

VOID SWDamageMeter::AddEnlighten(UINT32 playerId, FLOAT value)
{
	if (!isRun() || GetTime() == 0)
		return;

	auto itr = _playerInfo.begin();

	for (; itr != _playerInfo.end(); itr++) {
		if (playerId == (*itr)->GetID()) {
			(*itr)->AddEnlighten(value);
			return;
		}
	}

	if (CheckPlayer(playerId)) {
		SWDamagePlayer* newPlayer = new SWDamagePlayer(playerId);
		newPlayer->AddEnlighten(value);
		_playerInfo.push_back(newPlayer);
	}
}

VOID SWDamageMeter::AddSkillUsed(UINT32 playerId, UINT32 skillId)
{

	if (!isRun() || GetTime() == 0)
		return;

	// counts only skill that can proc enlighten (example : not right click)
	bool canProcEnlighten = false;
	if (skillId >= 10000000 && skillId < 1000000000) {
		if ((skillId % 100000) / 10000 == 0) {
			canProcEnlighten = true;
		}
	}
	if (!canProcEnlighten || !DAMAGEMETER.CheckPlayer(playerId)) {
		return;
	}

	auto itr = _playerInfo.begin();

	for (; itr != _playerInfo.end(); itr++) {
		if (playerId == (*itr)->GetID()) {
			(*itr)->AddSkillUsed(skillId);
			return;
		}
	}

	SWDamagePlayer* newPlayer = new SWDamagePlayer(playerId);
	newPlayer->AddSkillUsed(skillId);
	_playerInfo.push_back(newPlayer);

}

VOID SWDamageMeter::AddDodgeUsed(UINT32 playerId)
{
	if (!isRun() || GetTime() == 0)
		return;

	auto itr = _playerInfo.begin();

	for (; itr != _playerInfo.end(); itr++) {
		if (playerId == (*itr)->GetID()) {
			(*itr)->AddDodgeUsed();
			return;
		}
	}

	SWDamagePlayer* newPlayer = new SWDamagePlayer(playerId);
	newPlayer->AddDodgeUsed();
	_playerInfo.push_back(newPlayer);
}

VOID SWDamageMeter::AddDeath(UINT32 playerId)
{
	if (!isRun() || GetTime() == 0)
		return;

	auto itr = _playerInfo.begin();

	for (; itr != _playerInfo.end(); itr++) {
		if (playerId == (*itr)->GetID()) {
			(*itr)->AddDeathCount();
			return;
		}
	}

	SWDamagePlayer* newPlayer = new SWDamagePlayer(playerId);
	newPlayer->AddDeathCount();
	_playerInfo.push_back(newPlayer);
}

VOID SWDamageMeter::BuffIn(UINT32 playerId, USHORT buffId, BYTE stack, UINT32 giverId)
{
	if (_mazeEnd)
		return;
	if (buffId != 60228)
		return;

	auto itr = _playerInfo.begin();
	for (; itr != _playerInfo.end(); itr++) {
		if (playerId == (*itr)->GetID()) {
			(*itr)->SetJqStack(stack);
			return;
		}
	}
}

VOID SWDamageMeter::BuffOut(UINT32 playerId, USHORT buffId)
{
	if (_mazeEnd)
		return;
	if (buffId != 60228)
		return;

	auto itr = _playerInfo.begin();
	for (; itr != _playerInfo.end(); itr++) {
		if (playerId == (*itr)->GetID()) {
			(*itr)->SetJqStack(0);
			return;
		}
	}
}

VOID SWDamageMeter::InsertOwnerID(UINT32 id, UINT32 owner_id) {

	auto itr = _ownerInfo.begin();

	for (; itr != _ownerInfo.end(); itr++) {
		if ((*itr)->_id == id) {
#if DEBUG_DAMAGEMETER_OWNER_ID == 1
			Log::WriteLog(const_cast<LPTSTR>(_T("[DEBUG] [INSERT OWNER] [MODIFY] [ID = %08x] [OWNER = %08x]")), id, owner_id);
#endif
			(*itr)->_owner_id = owner_id;
			return;
		}
	}

	SW_OWNER_ID_STRUCT* obj = new SW_OWNER_ID_STRUCT;
	obj->_id = id;
	obj->_owner_id = owner_id;

#if DEBUG_DAMAGEMETER_OWNER_ID == 1
	Log::WriteLog(const_cast<LPTSTR>(_T("[DEBUG] [INSERT OWNER] [NEW] [ID = %08x] [OWNER = %08x]")), obj->_id, obj->_owner_id);
#endif

	_ownerInfo.push_back(obj);
}

UINT32 SWDamageMeter::GetOwnerID(UINT32 id) {

	auto itr = _ownerInfo.begin();

	for (; itr != _ownerInfo.end(); itr++) {
		if ((*itr)->_id == id)
			return (*itr)->_owner_id;
	}

	return 0xffffffff;
}

VOID SWDamageMeter::InsertPlayerMetadata(UINT32 id, CHAR* str, BYTE job) {

	auto search = _playerMetadata.find(id);

	if (search == _playerMetadata.end()) {
		SW_PLAYER_METADATA* metaData = new SW_PLAYER_METADATA;
		metaData->_id = id;
		metaData->_job = job;
		strcpy_s(metaData->_name, MAX_NAME_LEN, str);
		_playerMetadata[id] = metaData;

#if DEBUG_DAMAGEMETER_PLAYERMETA == 1
		Log::WriteLogA(const_cast<CHAR*>("[DEBUG] [INSERT PLAYER META] [NEW] [ID = %08x] [NAME = %s] [JOB = %x]"), id, str, job);
#endif
		return;
	}
	SW_PLAYER_METADATA* metaData = search->second;
	metaData->_id = id;
	metaData->_job = job;
	strcpy_s(metaData->_name, MAX_NAME_LEN, str);
}

const CHAR* SWDamageMeter::GetPlayerName(UINT32 id) {
	if (id == _myID)
		return STR_TABLE_YOU;

	auto search = _playerMetadata.find(id);
	if (search == _playerMetadata.end()) {
		return PLAYER_NAME_CANT_FIND;
	}
	return search->second->_name;
}

BYTE SWDamageMeter::GetPlayerJob(UINT32 id) {

	auto search = _playerMetadata.find(id);
	if (search == _playerMetadata.end()) {
		return PLAYER_JOB_CANT_FIND;
	}
	return search->second->_job;
}

VOID SWDamageMeter::UpdateStat(UINT32 id, USHORT statType, FLOAT statValue)
{
	//if (_historyMode) {
	//	return;
	//}
	SW_PLAYER_METADATA* metaData;

	auto search = _playerMetadata.find(id);
	if (search == _playerMetadata.end()) {
		metaData = new SW_PLAYER_METADATA;
		metaData->_id = id;
		strcpy_s(metaData->_name, MAX_NAME_LEN, PLAYER_NAME_CANT_FIND);
		metaData->_job = PLAYER_JOB_CANT_FIND;
		_playerMetadata[id] = metaData;
	}
	else {
		metaData = search->second;
	}

	metaData->UpdateStat(statType, statValue);
	return;
}


VOID SWDamageMeter::Sort() {
	sort(_playerInfo.begin(), _playerInfo.end(), SWDamagePlayer::SortFunction);
}

VOID SWDamageMeter::InsertDB(UINT32 id, UINT32 db2) {
	auto itr = _dbInfo.begin();

	for (; itr != _dbInfo.end(); itr++) {
		if ((*itr)->_id == id) {
#if DEBUG_DAMAGEMETER_DB == 1
			Log::WriteLog(const_cast<LPTSTR>(_T("[DEBUG] [INSERT DB] [MODIFY] [ID = %04x] [DB1 = %d] [DB2 = %d]")), id, GetWorldID(), db2);
#endif
			(*itr)->_db2 = db2;
			return;
		}
	}


	SW_DB2_STRUCT* db = new SW_DB2_STRUCT;
	db->_id = id;
	db->_db2 = db2;

#if DEBUG_DAMAGEMETER_DB == 1
	Log::WriteLog(const_cast<LPTSTR>(_T("[DEBUG] [INSERT DB] [NEW] [ID = %04x] [DB2 = %d]")), db->_id, db->_db2);
#endif

	_dbInfo.push_back(db);
}

SW_DB2_STRUCT* SWDamageMeter::GetMonsterDB(UINT32 id) {

	for (auto itr = _dbInfo.begin(); itr != _dbInfo.end(); itr++) {
#if DEBUG_DAMAGEMETER_DB == 1
		Log::WriteLog(const_cast<LPTSTR>(_T("[DEBUG] [GetMonsterDB] [FIND ID = %04x] [ID = %04x] [DB2 = %d]")), id, (*itr)->_id, (*itr)->_db2);
#endif
		if ((*itr)->_id == id) {
			return *itr;
		}
	}

	return nullptr;
}

VOID SWDamageMeter::SetWorldID(USHORT worldID) {
	_worldID = worldID;

#if DEBUG_DAMAGEMETER_WORLD == 1
	Log::WriteLog(const_cast<LPTSTR>(_T("[DEBUG] [Set World] [World ID = %d]")), _worldID);
#endif
}

USHORT SWDamageMeter::GetWorldID() {

	if (!_historyMode)
		return _worldID;
	else
		return _historyWorldID;
}

const CHAR* SWDamageMeter::GetWorldName() {

	if (!_historyMode)
		SWDB.GetMapName(_worldID, _mapName, MAX_MAP_LEN);
	else
		SWDB.GetMapName(_historyWorldID, _mapName, MAX_MAP_LEN);

	return _mapName;
}

VOID SWDamageMeter::SetAggro(UINT32 id, UINT32 targetedId)
{
	if (CheckPlayer(id)) {
		return;
	}

	SW_DB2_STRUCT* db = DAMAGEMETER.GetMonsterDB(id);
	UINT32 db2 = 0;
	if (db != nullptr) {
		db2 = db->_db2;
	}
	//Log::MyLog("db2 : %u\n", db2);

	if (bossMonsterList.find(db2) != bossMonsterList.end()) {
		//Log::MyLog("id is registered");
		_aggroedId = targetedId;
	}
	else {
		//Log::MyLog("id is not registered;");
		//Log::WriteLog(const_cast<LPTSTR>(_T("[SetAggro] not registered [id = %08x] [targetedId = %08x]")), id, targetedId);
	}
}

UINT32 SWDamageMeter::GetAggro()
{
	return _aggroedId;
}

VOID SWDamageMeter::SetMyID(UINT32 id) {
	_myID = id;

#if DEBUG_DAMAGEMETER_MYID == 1
	Log::WriteLog(const_cast<LPTSTR>(_T("[DEBUG] [Set MyID] [My ID = %08x]")), _myID);
#endif
}

UINT32 SWDamageMeter::GetMyID() {
	return _myID;
}

BOOL SWDamageMeter::CheckPlayer(UINT32 id) {
	if (id < 10000000)
		return TRUE;
	else
		return FALSE;
}

vector<SWDamagePlayer*>::const_iterator SWDamageMeter::GetPlayerInfo(UINT32 id) {

	auto itr = _playerInfo.begin();

	for (; itr != _playerInfo.end(); itr++) {
		if ((*itr)->GetID() == id) {
			return itr;
		}
	}

	return itr;
}

UINT64 SWDamageMeter::GetPlayerTotalDamage() {

	UINT64 playerTotalDamage = 0;

	for (auto itr = _playerInfo.begin(); itr != _playerInfo.end(); itr++) {
		playerTotalDamage += (*itr)->GetDamage();
	}

	return playerTotalDamage;
}

vector<SWDamagePlayer*>::const_iterator SWDamageMeter::begin() {
	return _playerInfo.begin();
}

vector<SWDamagePlayer*>::const_iterator SWDamageMeter::end() {
	return _playerInfo.end();
}

const SIZE_T SWDamageMeter::size() {
	return _playerInfo.size();
}

SWDamageMeter::SW_PLAYER_METADATA* SWDamageMeter::GetPlayerMetaData(UINT32 id)
{
	auto search = _playerMetadata.find(id);
	if (search == _playerMetadata.end()) {
		return nullptr;
	}
	return search->second;
}

BOOL SWDamageMeter::isRun() {
	return _timer.isRun();
}

VOID SWDamageMeter::Suspend() {

	if (_historyMode) {
		Restore();
		_historyMode = FALSE;
	}
	_timer.Suspend();
	for (auto itr = _playerMetadata.begin(); itr != _playerMetadata.end(); itr++) {
		itr->second->MeterSuspended();
	}

	//auto itr = _playerInfo.begin();
	//for (; itr != _playerInfo.end(); itr++) {
	//	(*itr)->MeterSuspended();
	//}
}

VOID SWDamageMeter::Start() {

	if (_historyMode) {
		Restore();
		_historyMode = FALSE;
	}
	_timer.Run();
}

VOID SWDamageMeter::Clear() {

	if (!_historyMode) {
		if (_playerInfo.size() > 0) {
			HISTORY.push_back(_playerInfo, BUFFMETER.GetPlayerInfo(), PLOTWINDOW.GetPlotInfo());
			for (auto itr = _playerMetadata.begin(); itr != _playerMetadata.end(); itr++) {
				itr->second->MeterReseted();
			}
			_playerInfo.clear();
		}
		_mazeEnd = FALSE;
	}
	else {
		Restore();
		_historyMode = FALSE;
	}
	_timer.Stop();
	PLOTWINDOW.Clear();
	BUFFMETER.Clear();
	_aggroedId = 0;
}

VOID SWDamageMeter::Toggle() {

	if (isRun())
		Suspend();
	else
		Start();
}

ULONG64 SWDamageMeter::GetTime() {

	if (!_historyMode)
		return _timer.GetTime();
	else
		return _historyTime;
}

VOID SWDamageMeter::SetMazeState(BOOL end) {
	_mazeEnd = end;
	if (end) {
		PLOTWINDOW.End();
	}
}

VOID SWDamageMeter::Restore() {

	if (!_historyMode)
		return;

	_playerInfo.clear();
	_historyID = -1;
}

VOID SWDamageMeter::SetHistory(INT index) {

	if (!_historyMode) {
		Clear();
	}

	auto history = HISTORY[index];
	_playerInfo = history._history;
	_historyWorldID = history._worldID;
	_historyTime = history._time;
	_historyID = index;
	BUFFMETER.SetPlayerInfo(history._buffHistory);
	PLOTWINDOW.SetPlotInfo(history._plotHistory);

	_historyMode = TRUE;
}

BOOL SWDamageMeter::isHistoryMode()
{
	return _historyMode;
}