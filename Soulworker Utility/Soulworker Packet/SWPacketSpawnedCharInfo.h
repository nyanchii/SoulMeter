#pragma once
#include ".\Soulworker Packet\SWPacket.h"
#include ".\Damage Meter\Damage Meter.h"

#pragma pack(push, 1)

typedef struct _SWPACKETSPAWNEDCHARINFO_HEADER {
	UINT32 _playerId;
	USHORT _nameSize;
}SWPACKETSPAWNEDCHARINFO_HEADER;

#pragma pack(pop)

class SWPacketSpawnedCharInfo : public SWPacket {
protected:
	SWPacketSpawnedCharInfo() {}

public:
	SWPacketSpawnedCharInfo(SWHEADER* swheader, BYTE* data);
	~SWPacketSpawnedCharInfo() {}

	VOID Do();
	VOID Log();
	VOID Debug();
};