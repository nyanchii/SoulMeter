#pragma once
#include ".\Damage Meter\Timer.h"
#include ".\Damage Meter\SWDamagePlayer.h"
using namespace std;

#define MAX_NAME_LEN 64
#define MAX_MAP_LEN 32

#define PLAYER_JOB_CANT_FIND 10
#define PLAYER_NAME_CANT_FIND u8"¥¼ª¾ª±®a"

#define DEBUG_DAMAGEMETER_OWNER_ID 0
#define DEBUG_DAMAGEMETER_DB 0
#define DEBUG_DAMAGEMETER_WORLD 0
#define DEBUG_DAMAGEMETER_MYID 0
#define DEBUG_DAMAGEMETER_PLAYERMETA 0

typedef struct _SW_OWNER_ID_STRUCT {
	UINT32 _id;
	UINT32 _owner_id;
}SW_OWNER_ID_STRUCT;

typedef struct _SW_DB2_STRUCT {
	UINT32 _id;
	USHORT _db1;
	UINT32 _db2;
}SW_DB2_STRUCT;

typedef struct _SW_PLAYER_METADATA {
	UINT32 _id;
	CHAR _name[MAX_NAME_LEN];
	BYTE _job;
}SW_PLAYER_METADATA;

#define DAMAGEMETER SWDamageMeter::getInstance()

class SWDamageMeter : public Singleton<SWDamageMeter> {
private:
	vector<SWDamagePlayer*> _playerInfo;
	vector<SW_OWNER_ID_STRUCT*> _ownerInfo;
	vector<SW_DB2_STRUCT*> _dbInfo;
	vector<SW_PLAYER_METADATA*> _playerMetadata;

	CHAR _mapName[MAX_MAP_LEN];
	UINT32 _myID;
	USHORT _worldID;
	
	USHORT _historyWorldID;
	FLOAT _historyTime;
	
	BOOL _mazeEnd;
	BOOL _historyMode;

	VOID InsertPlayerInfo(UINT32 id, UINT64 damage, UINT64 critDamage, USHORT hitCount, USHORT critHitCount, USHORT maxCombo, UINT32 monsterID, UINT32 skillID);
	VOID Sort();

	Timer _timer;

	mutex _mutex;

public:
	SWDamageMeter() : _myID(0), _worldID(0), _mazeEnd(0), _historyMode(0), _historyWorldID(0), _historyTime(0) {}
	~SWDamageMeter();

	VOID Restore();

	VOID GetLock();
	VOID FreeLock();

	VOID AddDamage(UINT32 id, UINT64 damage, UINT64 critDamage, USHORT hitCount, USHORT critHitCount, USHORT maxCombo, UINT32 monsterID, UINT32 skillID);
	
	VOID InsertOwnerID(UINT32 id, UINT32 owner_id);
	UINT32 GetOwnerID(UINT32 id);

	VOID InsertDB(UINT32 id, UINT32 db2);
	SW_DB2_STRUCT* GetMonsterDB(UINT32 id);

	VOID SetWorldID(USHORT worldID);
	USHORT GetWorldID();
	const CHAR* GetWorldName();

	VOID InsertPlayerMetadata(UINT32 id, CHAR* str, BYTE job);
	const CHAR* GetPlayerName(UINT32 id);
	BYTE GetPlayerJob(UINT32 id);

	VOID SetMyID(UINT32 id);

	BOOL CheckPlayer(UINT32 id); 

	vector<SWDamagePlayer*>::const_iterator GetPlayerInfo(UINT32 id);
	vector<SWDamagePlayer*>::const_iterator begin();
	vector<SWDamagePlayer*>::const_iterator end();
	const SIZE_T& size();

	UINT64 GetPlayerTotalDamage();

	BOOL isRun();
	BOOL isHistoryMode();

	VOID Suspend();
	VOID Start();
	VOID Clear();
	VOID Toggle();

	FLOAT GetTime();

	VOID SetMazeState(BOOL end);

	VOID SetHistory(INT index);
	VOID SaveHistory();
};