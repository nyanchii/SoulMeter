#pragma once

#define DEBUG_DAMAGE_SKILL 0
#define SKILL_NAME_LEN 64

class SWDamageSkill : public MemoryPool<SWDamageSkill, 100> {
private:
	UINT32 _id;
	UINT64 _damage;
	UINT64 _critDamage;
	USHORT _hitCount;
	USHORT _critHitCount;
	CHAR _name[SKILL_NAME_LEN];

protected:
	SWDamageSkill() : _id(0), _damage(0), _critDamage(0), _hitCount(0), _critHitCount(0) { }

public:
	SWDamageSkill(UINT32 id, UINT64 damage, UINT64 critDamage, USHORT hitCount, USHORT critHitCount);
	~SWDamageSkill() {}

	static BOOL SortFunction(SWDamageSkill* skillA, SWDamageSkill* skillB);

	UINT32 GetID();
	UINT64 GetDamage();
	UINT64 GetCritDamage();
	USHORT GetHitCount();
	USHORT GetCritHitCount();
	CHAR* GetName();
	VOID AddDamage(UINT64 damage, UINT64 critDamage, USHORT hitCount, USHORT critHitCount);
};