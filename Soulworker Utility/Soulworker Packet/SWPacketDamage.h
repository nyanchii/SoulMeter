#pragma once
#include ".\Soulworker Packet\SWPacket.h"
#include ".\Damage Meter\Damage Meter.h"

#pragma pack(push, 1)

typedef struct _SWPACKETDAMAGE_MONSTER {
	UINT32 _monsterID;
	BYTE _unknown01;
	BYTE _damageType;
	UINT32 _totalDMG;
	UINT32 _criticalDMG;
	UINT32 _remainHP;
	BYTE _unknown02[22];
}SWPACKETDAMAGE_MONSTER;

typedef struct _SWPACKETDAMAGE_PLAYER {
	UINT32 _playerID;
	BYTE _unknown01[20];
	UINT32 _skillID;
	BYTE _unknown02[2];
	USHORT _maxCombo;
	BYTE _unknown03[2];
}SWPACKETDAMAGE_PLAYER;

#pragma pack(pop)

class SWPacketDamage : public SWPacket {
private:
	BOOL CheckCritical(BYTE damageType);

protected:
	SWPacketDamage() {}

public:
	SWPacketDamage(SWHEADER* swheader, BYTE* data);
	~SWPacketDamage() {}

	VOID Do();
	VOID Debug();
	VOID Log() {};
};