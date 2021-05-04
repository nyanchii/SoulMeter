#pragma once
#include ".\Soulworker Packet\SWPacket.h"
#include ".\Damage Meter\Damage Meter.h"

#define SWPACKETPOS_DUMMY 36

#pragma pack(push, 1)

typedef struct _SWPACKETPOS_HEADER { // 19
	BYTE _unknown01[4];
	UINT32 _posHostID;
	BYTE _unknown02[10];
	BYTE _posPlayerCount;
}SWPACKETPOS_HEADER;

typedef struct _SWPACKETPOS_DATA { // 6
	UINT32 _playerID;
	USHORT _nickSize;
}SWPACKETPOS_DATA;

#pragma pack(pop)

class SWPacketPos : public SWPacket {
protected:
	SWPacketPos() {}

public:
	SWPacketPos(SWHEADER* swheader, BYTE* data);
	~SWPacketPos() {}

	VOID Do();
	VOID Log();
	VOID Debug();
};