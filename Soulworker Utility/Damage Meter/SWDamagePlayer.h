#pragma once
#include ".\Damage Meter\SWDamageMonster.h"
using namespace std;

#define DEBUG_DAMAGE_PLAYER 0

class SWDamagePlayer : public MemoryPool<SWDamagePlayer, 30> {
private:
	UINT32 _id;
	UINT64 _damage;
	UINT64 _critDamage;
	USHORT _hitCount;
	USHORT _critHitCount;
	USHORT _maxCombo;

	vector<SWDamageMonster*> _monsterInfo;
	VOID InsertMonsterInfo(UINT32 monsterID, UINT64 damage, UINT64 critDamage, USHORT hitCount, USHORT critHitCount, UINT32 skillID);
	VOID Sort();

protected:
	SWDamagePlayer() : _id(0), _damage(0), _critDamage(0), _hitCount(0), _critHitCount(0), _maxCombo(0) { }

public:
	SWDamagePlayer(UINT32 id, UINT64 damage, UINT64 critDamage, USHORT hitCount, USHORT critHitCount, USHORT maxCombo, UINT32 monsterID, UINT32 skillID);
	~SWDamagePlayer();

	static BOOL SortFunction(SWDamagePlayer* playerA, SWDamagePlayer* playerB);

	UINT32 GetID();
	UINT64 GetDamage();
	UINT64 GetCritDamage();
	USHORT GetHitCount();
	USHORT GetCritHitCount();
	USHORT GetMaxCombo();
	UINT64 GetMonsterTotalDamage();

	vector<SWDamageMonster*>::const_iterator GetMonsterInfo(UINT id);
	vector<SWDamageMonster*>::const_iterator begin();
	vector<SWDamageMonster*>::const_iterator end();
	const SIZE_T& size();

	VOID AddDamage(UINT64 damage, UINT64 critDamage, USHORT hitCount, USHORT critHitCount, USHORT maxCombo, UINT32 monsterID, UINT32 skillID);
};