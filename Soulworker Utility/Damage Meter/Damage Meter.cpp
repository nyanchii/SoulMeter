#include "pch.h"
#include ".\Damage Meter\Timer.h"
#include ".\Damage Meter\SWDamagePlayer.h"
#include ".\Damage Meter\Damage Meter.h"
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
		delete (*itr);

	_playerInfo.clear();
	_ownerInfo.clear();
	_dbInfo.clear();
	_playerMetadata.clear();

	_mutex.unlock();
}

VOID SWDamageMeter::GetLock() {
	_mutex.lock();
}
VOID SWDamageMeter::FreeLock() {
	_mutex.unlock();
}

VOID SWDamageMeter::InsertPlayerInfo(UINT32 id, UINT64 damage, UINT64 critDamage, USHORT hitCount, USHORT critHitCount, USHORT maxCombo, UINT32 monsterID, UINT32 skillID) {
	auto itr = _playerInfo.begin();

	for (; itr != _playerInfo.end(); itr++) {
		if (id == (*itr)->GetID()) {
			(*itr)->AddDamage(damage, critDamage, hitCount, critHitCount, maxCombo, monsterID, skillID);
			return;
		}
	}

	_playerInfo.push_back(new SWDamagePlayer(id, damage, critDamage, hitCount, critHitCount, maxCombo, monsterID, skillID));
}

VOID SWDamageMeter::AddDamage(UINT32 id, UINT64 damage, UINT64 critDamage, USHORT hitCount, USHORT critHitCount, USHORT maxCombo, UINT32 monsterID, UINT32 skillID) {
	if (_mazeEnd)
		return;

	Start();
	InsertPlayerInfo(id, damage, critDamage, hitCount, critHitCount, maxCombo, monsterID, skillID);
	Sort();
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

	auto itr = _playerMetadata.begin();

	for (; itr != _playerMetadata.end(); itr++) {
		if ((*itr)->_id == id) {
			(*itr)->_job = job;
			strcpy_s((*itr)->_name, MAX_NAME_LEN, str);
#if DEBUG_DAMAGEMETER_PLAYERMETA == 1
			Log::WriteLogA(const_cast<CHAR*>("[DEBUG] [INSERT PLAYER META] [MODIFY] [ID = %08x] [NAME = %s] [JOB = %d]"), (*itr)->_id, (*itr)->_name, (*itr)->_job);
#endif
			return;
		}
	}

	SW_PLAYER_METADATA* metadata = new SW_PLAYER_METADATA;
	metadata->_id = id;
	metadata->_job = job;
	strcpy_s(metadata->_name, MAX_NAME_LEN, str);

#if DEBUG_DAMAGEMETER_PLAYERMETA == 1
	Log::WriteLogA(const_cast<CHAR*>("[DEBUG] [INSERT PLAYER META] [NEW] [ID = %08x] [NAME = %s] [JOB = %x]"), metadata->_id, metadata->_name, metadata->_job);
#endif

	_playerMetadata.push_back(metadata);

}

const CHAR* SWDamageMeter::GetPlayerName(UINT32 id) {

	for (auto itr = _playerMetadata.begin(); itr != _playerMetadata.end(); itr++) {
		if ((*itr)->_id == id) {
			if ((*itr)->_id == _myID) {
				return u8"自己";
			}
			return (*itr)->_name;
		}
	}

	if (id == _myID)
		return u8"自己";

	return PLAYER_NAME_CANT_FIND;
}

BYTE SWDamageMeter::GetPlayerJob(UINT32 id) {

	for (auto itr = _playerMetadata.begin(); itr != _playerMetadata.end(); itr++) {
		if ((*itr)->_id == id)
			return (*itr)->_job;
	}

	return PLAYER_JOB_CANT_FIND;
}

VOID SWDamageMeter::Sort() {
	sort(_playerInfo.begin(), _playerInfo.end(), SWDamagePlayer::SortFunction);
}

VOID SWDamageMeter::InsertDB(UINT32 id, UINT32 db2) {

	auto itr = _dbInfo.begin();

	for (; itr != _dbInfo.end(); itr++) {
		if ((*itr)->_id == id) {
#if DEBUG_DAMAGEMETER_DB == 1
			Log::WriteLog(const_cast<LPTSTR>(_T("[DEBUG] [INSERT DB] [MODIFY] [ID = %08x] [DB1 = %d] [DB2 = %d]")), id, GetWorldID(), db2);
#endif
			(*itr)->_db1 = GetWorldID();
			(*itr)->_db2 = db2;
			return;
		}
	}

	SW_DB2_STRUCT* db = new SW_DB2_STRUCT;
	db->_id = id;
	db->_db1 = GetWorldID();
	db->_db2 = db2;

#if DEBUG_DAMAGEMETER_DB == 1
	Log::WriteLog(const_cast<LPTSTR>(_T("[DEBUG] [INSERT DB] [NEW] [ID = %08x] [DB1 = %d] [DB2 = %d]")), db->_id, db->_db1, db->_db2);
#endif

	_dbInfo.push_back(db);
}

SW_DB2_STRUCT* SWDamageMeter::GetMonsterDB(UINT32 id) {

	for (auto itr = _dbInfo.begin(); itr != _dbInfo.end(); itr++) {
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

VOID SWDamageMeter::SetMyID(UINT32 id) {
	_myID = id;

#if DEBUG_DAMAGEMETER_MYID == 1
	Log::WriteLog(const_cast<LPTSTR>(_T("[DEBUG] [Set MyID] [My ID = %08x]")), _myID);
#endif
}

BOOL SWDamageMeter::CheckPlayer(UINT32 id) {
	if (id < 1073741824)
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

const SIZE_T& SWDamageMeter::size() {
	return _playerInfo.size();
}

BOOL SWDamageMeter::isRun() {
	return _timer.isRun();
}

BOOL SWDamageMeter::isHistoryMode() {
	return _historyMode;
}

VOID SWDamageMeter::Suspend() {

	if (_historyMode) {
		Restore();
		_historyMode = FALSE;
	}
	_timer.Suspend();
}

VOID SWDamageMeter::SaveHistory() {

	if (!_historyMode && _playerInfo.size() > 0) {
		HISTORY.push_back(_playerInfo);
	}
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
			//HISTORY.push_back(_playerInfo);
			_playerInfo.clear();
		}
		_mazeEnd = FALSE;
	}
	else {
		Restore();
		_historyMode = FALSE;
	}

	_timer.Stop();
}

VOID SWDamageMeter::Toggle() {

	if (isRun())
		Suspend();
	else
		Start();
}

FLOAT SWDamageMeter::GetTime() {

	if (!_historyMode)
		return _timer.GetTime();
	else
		return _historyTime;
}

VOID SWDamageMeter::SetMazeState(BOOL end) {
	_mazeEnd = end;
}

VOID SWDamageMeter::Restore() {

	if (!_historyMode)
		return;

	_playerInfo.clear();
}

VOID SWDamageMeter::SetHistory(INT index) {

	//if (!_historyMode) {
	//	Clear();
	//}

	auto history = HISTORY[index];
	_playerInfo = history._history;
	_historyWorldID = history._worldID;
	_historyTime = history._time;
	
	_historyMode = TRUE;
}