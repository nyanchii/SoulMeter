#pragma once
#include "pch.h"
#include ".\Language\Region.h"
#include ".\Damage Meter\Timer.h"
#include ".\Damage Meter\SWDamagePlayer.h"
#include ".\UI\PlotWindow.h"
using namespace std;
#include <unordered_map>

#define MAX_NAME_LEN 64
//#define MAX_MAP_LEN 32
#define MAX_MAP_LEN 64

#define PLAYER_JOB_CANT_FIND 10

#ifdef _DEBUG
#define DEBUG_DAMAGEMETER_OWNER_ID 0
#define DEBUG_DAMAGEMETER_DB 0
#define DEBUG_DAMAGEMETER_WORLD 0
#define DEBUG_DAMAGEMETER_MYID 0
#define DEBUG_DAMAGEMETER_PLAYERMETA 0
#endif

// 
#define DAMAGEMETER SWDamageMeter::getInstance()

//  forward declaration
// https://stackoverflow.com/questions/41502310/c-inclusion-of-typedef-struct-from-inside-a-class
// https://en.cppreference.com/w/cpp/language/class#Forward_declaration

typedef struct _SW_OWNER_ID_STRUCT {
	UINT32 _id;
	UINT32 _owner_id;
}SW_OWNER_ID_STRUCT;

typedef struct _SW_DB2_STRUCT {
	UINT32 _id;
	//	USHORT _db1;
	UINT32 _db2;
}SW_DB2_STRUCT;


static unordered_set<UINT32> resumeIgnoreIdList({
	
	14000101, 14000102, 14000103,
	1081102, 1081103, 1081104, 1081105, 1081106, 1081107, 1081108, 1081109,
	1091101 
	});

static unordered_set<UINT32> bossMonsterList({
	
	701, 702, 
	31308001, 
	//31309101, 31309102, 31309151, 
	31309151, 
	31301001, 31301002, 31301003, 
	31310101, 31310102, 
	31310103, 31310107 

	//90500009,90500010,90500011,90500012, 
	//90500014,90500015,90500016,90500017 
	});


class SWDamageMeter : public Singleton<SWDamageMeter> {
public: typedef struct _SW_PLAYER_METADATA {
private:
	FLOAT _armorBreak = 0.0;
	FLOAT _sg = 0.0;
	FLOAT _stamina = 0.0;
	FLOAT _sv = 0.0;
	FLOAT _attackSpeed = 0.0;
	FLOAT _partialDamage = 0.0;
	FLOAT _maxHP = 0.0;
	FLOAT _currentHP = 0.0;
	FLOAT _maxAttack = 0.0;
	FLOAT _critDamage = 0.0;
public:
	UINT32 _id = 0;
	CHAR _name[MAX_NAME_LEN];
	BYTE _job = 0;
	UINT64 _avgABSum = 0;
	UINT64 _avgABPreviousTime = 0;

	BOOL _gear90EffectStarted = false;
	UINT64 _gear90Sum = 0;
	UINT64 _gear90EffectStartedTime = 0;

	BOOL _gear50EffectStarted = false;
	UINT64 _gear50Sum = 0;
	UINT64 _gear50EffectStartedTime = 0;

	BOOL _acc01EffectStarted = false;
	UINT64 _acc01Sum = 0;
	UINT64 _acc01EffectStartedTime = 0;

	BOOL _acc02EffectStarted = false;
	UINT64 _acc02Sum = 0;
	UINT64 _acc02EffectStartedTime = 0;

	DOUBLE _losedHp = 0;


	_SW_PLAYER_METADATA() {
		_id = 0;
		_job = 0;
		_avgABSum = 0;
		_avgABPreviousTime = 0;
	}

	VOID UpdateStat(USHORT statType, FLOAT statValue) {
		switch (statType) {
		case StatType::SG:
			_sg = statValue;
			break;
		case StatType::Stamina:
			_stamina = statValue;
			break;
		case StatType::SV:
			_sv = statValue;
			break;
		case StatType::AttackSpeed:
			_attackSpeed = statValue;
			break;
		case StatType::PartialDamage:
			_partialDamage = statValue;
			break;
		case StatType::ArmorBreak:
			if (DAMAGEMETER.isRun()) {
				UINT64 time = (UINT64)((DOUBLE)DAMAGEMETER.GetTime());
				FLOAT correctedAB = (_armorBreak > 100) ? 100 : _armorBreak;
				_avgABSum += static_cast<UINT64>((time - _avgABPreviousTime) * correctedAB);
				_avgABPreviousTime = time;
			}
			_armorBreak = statValue;
			break;
		case StatType::CurrentHP:
			if (_currentHP > 0.0 && _currentHP > statValue) {
				_losedHp += ((DOUBLE)_currentHP - (DOUBLE)statValue);
			}
			_currentHP = statValue;
			break;
		case StatType::MaxHP:
			_maxHP = statValue;
			break;
		case StatType::MinAttack:
			_maxAttack = static_cast<FLOAT>(statValue * 1.25);
			break;
		case StatType::MaxAttack:
			_maxAttack = statValue;
			break;
		case StatType::CritDamage:
			_critDamage = statValue;
			break;
		default:
			break;
		}
	}
	VOID MeterSuspended() {
		UINT64 currentTime = (UINT64)DAMAGEMETER.GetTime();

		FLOAT correctedAB = (_armorBreak > 100) ? 100 : _armorBreak;
		_avgABSum += static_cast<UINT64>((currentTime - _avgABPreviousTime) * correctedAB);
		_avgABPreviousTime = currentTime;


	}
	VOID MeterReseted() {
		auto player = DAMAGEMETER.GetPlayerInfo(_id);
		if (player != DAMAGEMETER.end()) {
			// 
			(*player)->setHistoryLosedHP(_losedHp);

			if (_id == DAMAGEMETER.GetMyID()) {
				// 
				UINT64 currentTime = (UINT64)DAMAGEMETER.GetTime();

				UINT64 avgTimeDifference = currentTime - _avgABPreviousTime;
				DOUBLE currentAB = GetStat(StatType::ArmorBreak);
				currentAB = currentAB > 100.0 ? 100.0 : currentAB; // 
				UINT64 calculatedAvgAB = static_cast<UINT64>((_avgABSum + avgTimeDifference * currentAB));
				(*player)->SetHistoryAvgAB((DOUBLE)calculatedAvgAB / currentTime);

				// 
				if (_gear90EffectStarted) {
					UINT64 gear90TimeDiff = currentTime - _gear90EffectStartedTime;
					gear90TimeDiff = (gear90TimeDiff >= 5000) ? 5000 : gear90TimeDiff;
					_gear90Sum += gear90TimeDiff * 500;
					_gear90EffectStarted = false;
				}

				// 
				if (_gear50EffectStarted) {
					UINT64 gear50TimeDiff = currentTime - _gear50EffectStartedTime;
					gear50TimeDiff = (gear50TimeDiff >= 5000) ? 5000 : gear50TimeDiff;
					_gear50Sum += gear50TimeDiff * 1000;
					_gear50EffectStarted = false;
				}

				//
				if (_acc01EffectStarted) {
					UINT64 acc01TimeDiff = currentTime - _acc01EffectStartedTime;
					acc01TimeDiff = (acc01TimeDiff >= 2000) ? 2000 : acc01TimeDiff;
					_acc01Sum += acc01TimeDiff * 1200;
					_acc01EffectStarted = false;
				}

				// 
				if (_acc02EffectStarted)
				{
					UINT64 acc02TimeDiff = currentTime - _acc02EffectStartedTime;
					acc02TimeDiff = (acc02TimeDiff >= 10000) ? 10000 : acc02TimeDiff;
					_acc02Sum += acc02TimeDiff * 3000;
					_acc02EffectStarted = false;
				}

				// Save all to history
				(*player)->setHistoryBS(90, (DOUBLE)_gear90Sum / currentTime);
				(*player)->setHistoryBS(50, (DOUBLE)_gear50Sum / currentTime);
				(*player)->setHistoryBS(1, (DOUBLE)_acc01Sum / currentTime);
				(*player)->setHistoryBS(2, (DOUBLE)_acc02Sum / currentTime);
			}



		}

		_losedHp = 0.0;

		_avgABSum = 0;
		_avgABPreviousTime = 0;

		_gear90EffectStarted = false;
		_gear90Sum = 0;
		_gear90EffectStartedTime = 0;

		_gear50EffectStarted = false;
		_gear50Sum = 0;
		_gear50EffectStartedTime = 0;

		_acc01EffectStarted = false;
		_acc01Sum = 0;
		_acc01EffectStartedTime = 0;

		_acc02EffectStarted = false;
		_acc02Sum = 0;
		_acc02EffectStartedTime = 0;
	}

	VOID HitEnemy() {
		if (!DAMAGEMETER.isRun()) {
			return;
		}
		UINT64 currentTime = DAMAGEMETER.GetTime();
		CalBsGear3Set(true, currentTime);
		CalBsGear4Set(true, currentTime);
		CalBsAccSet1(true, currentTime);
		CalBsAccSet2(true, currentTime);
	}

	// Add to sum if duration of effect is finished or effect duration is updated
	// If duration is not finished, calculate it based on table time
	UINT64 CalBsGear3Set(bool hit, UINT64 currentTime) {
		const double HP_CONDITION = 90.0;
		const int DURATION = 5000;
		const int ATK = 500;

		const DOUBLE hpPercent = (DOUBLE)_currentHP / (DOUBLE)_maxHP * 100;

		if (currentTime - _gear90EffectStartedTime < 0) {
			return 0;
		}

		UINT64 _gear90TimeLapse = currentTime - _gear90EffectStartedTime;

		if ((hpPercent >= HP_CONDITION) && hit && !_gear90EffectStarted) { // new start
			_gear90EffectStarted = true;
			_gear90EffectStartedTime = currentTime;
		}
		else if ((hpPercent >= HP_CONDITION) && hit && _gear90EffectStarted) { // update effect
			if (_gear90TimeLapse >= DURATION) {
				_gear90TimeLapse = DURATION;
			}
			_gear90Sum += _gear90TimeLapse * ATK;
			_gear90EffectStartedTime = currentTime;
		}
		else if ((hpPercent < HP_CONDITION) && hit && _gear90EffectStarted) { // end effect
			if (_gear90TimeLapse >= DURATION) {
				_gear90TimeLapse = DURATION;
				_gear90Sum += _gear90TimeLapse * ATK;
				_gear90EffectStarted = false;
			}
		}
		else if (!hit && !_gear90EffectStarted) { // PlayerTable
			return 0;
		}
		else if (!hit && _gear90EffectStarted && (_gear90TimeLapse >= 0)) { // PlayerTable
			if (_gear90TimeLapse >= DURATION) {
				_gear90TimeLapse = DURATION;
			}
			return _gear90TimeLapse * ATK;
		}
		return 0;
	}

	UINT64 CalBsGear4Set(bool hit, UINT64 currentTime) {
		const double HP_CONDITION = 50.0;
		const int DURATION = 5000;
		const int ATK = 1000;

		const DOUBLE hpPercent = (DOUBLE)_currentHP / (DOUBLE)_maxHP * 100;

		if (currentTime - _gear50EffectStartedTime < 0) {
			return 0;
		}

		UINT64 _gear50TimeLapse = currentTime - _gear50EffectStartedTime;

		if ((hpPercent >= HP_CONDITION) && hit && !_gear50EffectStarted) { // new start
			_gear50EffectStarted = true;
			_gear50EffectStartedTime = currentTime;
		}
		else if ((hpPercent >= HP_CONDITION) && hit && _gear50EffectStarted) { // update effect
			if (_gear50TimeLapse >= DURATION) {
				_gear50TimeLapse = DURATION;
			}
			_gear50Sum += _gear50TimeLapse * ATK;
			_gear50EffectStartedTime = currentTime;
		}
		else if ((hpPercent < HP_CONDITION) && hit && _gear50EffectStarted) { // end effect
			if (_gear50TimeLapse >= DURATION) {
				_gear50TimeLapse = DURATION;
				_gear50Sum += _gear50TimeLapse * ATK;
				_gear50EffectStarted = false;
			}
		}
		else if (!hit && !_gear50EffectStarted) { // PlayerTable
			return 0;
		}
		else if (!hit && _gear50EffectStarted && (_gear50TimeLapse >= 0)) { // PlayerTable
			if (_gear50TimeLapse >= DURATION) {
				_gear50TimeLapse = DURATION;
			}
			return _gear50TimeLapse * ATK;
		}
		return 0;
	}

	UINT64 CalBsAccSet1(bool hit, UINT64 currentTime) {
		const double HP_CONDITION = 85.0;
		const int DURATION = 2000;
		const int ATK = 1200;

		const DOUBLE hpPercent = (DOUBLE)_currentHP / (DOUBLE)_maxHP * 100;

		if (currentTime - _acc01EffectStartedTime < 0) {
			return 0;
		}

		UINT64 _acc01TimeLapse = currentTime - _acc01EffectStartedTime;


		if ((hpPercent >= HP_CONDITION) && hit && !_acc01EffectStarted) { // new start
			_acc01EffectStarted = true;
			_acc01EffectStartedTime = currentTime;
		}
		else if ((hpPercent >= HP_CONDITION) && hit && _acc01EffectStarted) { // update effect
			if (_acc01TimeLapse >= DURATION) {
				_acc01TimeLapse = DURATION;
			}
			_acc01Sum += _acc01TimeLapse * ATK;
			_acc01EffectStartedTime = currentTime;
		}
		else if ((hpPercent < HP_CONDITION) && hit && _acc01EffectStarted) { // end effect
			if (_acc01TimeLapse >= DURATION) {
				_acc01TimeLapse = DURATION;
				_acc01Sum += _acc01TimeLapse * ATK;
				_acc01EffectStarted = false;
			}
		}
		else if (!hit && !_acc01EffectStarted) { // PlayerTable
			return 0;
		}
		else if (!hit && _acc01EffectStarted && (_acc01TimeLapse >= 0)) { // PlayerTable
			if (_acc01TimeLapse >= DURATION) {
				_acc01TimeLapse = DURATION;
			}
			return _acc01TimeLapse * ATK;
		}
		return 0;
	}

	UINT64 CalBsAccSet2(bool hit, UINT64 currentTime) {
		const double HP_CONDITION = 30.0;
		const int DURATION = 10000;
		const int ATK = 3000;

		const DOUBLE hpPercent = (DOUBLE)_currentHP / (DOUBLE)_maxHP * 100;
		if (currentTime - _acc02EffectStartedTime < 0) {
			return 0;
		}

		UINT64 _acc02TimeLapse = currentTime - _acc02EffectStartedTime;

		if ((hpPercent < HP_CONDITION) && hit && !_acc02EffectStarted) { // new start
			_acc02EffectStarted = true;
			_acc02EffectStartedTime = currentTime;
		}
		else if ((hpPercent < HP_CONDITION) && hit && _acc02EffectStarted) { // update effect
			if (_acc02TimeLapse >= DURATION) {
				_acc02TimeLapse = DURATION;
			}
			_acc02Sum += _acc02TimeLapse * ATK;
			_acc02EffectStartedTime = currentTime;
		}
		else if ((hpPercent >= HP_CONDITION) && hit && _acc02EffectStarted) { // end effect
			if (_acc02TimeLapse >= DURATION) {
				_acc02TimeLapse = DURATION;
				_acc02Sum += _acc02TimeLapse * ATK;
				_acc02EffectStarted = false;
			}
		}
		else if (!hit && !_acc02EffectStarted) { // PlayerTable
			return 0;
		}
		else if (!hit && _acc02EffectStarted && (_acc02TimeLapse >= 0)) { // PlayerTable
			if (_acc02TimeLapse >= DURATION) {
				_acc02TimeLapse = DURATION;
			}
			return _acc02TimeLapse * ATK;
		}
		return 0;
	}

	FLOAT GetStat(USHORT statType) {
		switch (statType) {
		case StatType::SG:
			return _sg;
		case StatType::Stamina:
			return _stamina;
		case StatType::SV:
			return _sv;
		case StatType::AttackSpeed:
			return _attackSpeed;
		case StatType::PartialDamage:
			return _partialDamage;
		case StatType::ArmorBreak:
			return _armorBreak;
		case StatType::MaxAttack:
			return _maxAttack;
		case StatType::CritDamage:
			return _critDamage;
		}

		return -1;
	}

}SW_PLAYER_METADATA;
private:
	vector<SWDamagePlayer*> _playerInfo;
	vector<SW_OWNER_ID_STRUCT*> _ownerInfo;
	vector<SW_DB2_STRUCT*> _dbInfo;
	unordered_map<UINT32, SW_PLAYER_METADATA*> _playerMetadata;

	CHAR _mapName[MAX_MAP_LEN];
	UINT32 _myID;
	USHORT _worldID;

	USHORT _historyWorldID;
	ULONG64 _historyTime;
	INT32 _historyID;

	UINT32 _aggroedId;

	BOOL _mazeEnd;
	BOOL _historyMode;

	VOID InsertPlayerInfo(UINT32 id, UINT64 totalDMG, UINT64 soulstoneDMG, SWPACKETDAMAGE_DAMAGETYPE damageType, USHORT maxCombo, UINT32 monsterID, UINT32 skillID);
	VOID Sort();
	VOID Restore();

	Timer _timer;

	mutex _mutex;

public:
	SWDamageMeter() :  _historyID(-1), _myID(0), _worldID(0), _mazeEnd(0), _historyMode(0), _historyWorldID(0), _historyTime(0) {}
	~SWDamageMeter();

	VOID GetLock();
	VOID FreeLock();

	VOID AddDamage(UINT32 id, UINT64 totalDMG, UINT64 soulstoneDMG, SWPACKETDAMAGE_DAMAGETYPE damageType, USHORT maxCombo, UINT32 monsterID, UINT32 skillID);
	VOID AddPlayerGetDamage(UINT32 playerId, UINT64 totalDMG, SWPACKETDAMAGE_DAMAGETYPE damageType, UINT32 monsterID, UINT32 skillID);
	VOID AddEnlighten(UINT32 playerId, FLOAT value);
	VOID AddSkillUsed(UINT32 playerId, UINT32 skillId);
	VOID AddDodgeUsed(UINT32 playerId);
	VOID AddDeath(UINT32 playerId);
	VOID BuffIn(UINT32 playerId, USHORT buffId, BYTE stack, UINT32 giverId);
	VOID BuffOut(UINT32 playerId, USHORT buffId);

	VOID InsertOwnerID(UINT32 id, UINT32 owner_id);
	UINT32 GetOwnerID(UINT32 id);

	VOID InsertDB(UINT32 id, UINT32 db2);
	SW_DB2_STRUCT* GetMonsterDB(UINT32 id);

	VOID SetWorldID(USHORT worldID);
	USHORT GetWorldID();
	const CHAR* GetWorldName();

	VOID SetAggro(UINT32 id, UINT32 targetedId);
	UINT32 GetAggro();

	VOID InsertPlayerMetadata(UINT32 id, CHAR* str, BYTE job);
	const CHAR* GetPlayerName(UINT32 id);
	BYTE GetPlayerJob(UINT32 id);

	VOID UpdateStat(UINT32 id, USHORT statType, FLOAT statValue);

	VOID SetMyID(UINT32 id);

	UINT32 GetMyID();

	BOOL CheckPlayer(UINT32 id);

	vector<SWDamagePlayer*>::const_iterator GetPlayerInfo(UINT32 id);
	vector<SWDamagePlayer*>::const_iterator begin();
	vector<SWDamagePlayer*>::const_iterator end();
	const SIZE_T size();

	SW_PLAYER_METADATA* GetPlayerMetaData(UINT32 id);

	UINT64 GetPlayerTotalDamage();

	BOOL isRun();

	VOID Suspend();
	VOID Start();
	VOID Clear();
	VOID Toggle();

	ULONG64 GetTime();

	VOID SetMazeState(BOOL end);

	VOID SetHistory(INT index);
	BOOL isHistoryMode();

	INT32 GetCurrentHistoryID()
	{
		return _historyID;
	}
};