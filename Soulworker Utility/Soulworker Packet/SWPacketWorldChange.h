#pragma once
#include ".\Soulworker Packet\SWPacket.h"
#include ".\Damage Meter\Damage Meter.h"

#pragma pack(push, 1)

typedef struct _SWPACKETWORLDCHANGE {
	UINT32 _id;
	BYTE _unknwon1[20];
	USHORT _worldID;
	BYTE _unknwon2[68];
}SWPACKETWORLDCHANGE;

#pragma pack(pop)

class SWPacketWorldChange : public SWPacket {
protected:
	SWPacketWorldChange() {}

public:
	SWPacketWorldChange(SWHEADER* swheader, BYTE* data);
	~SWPacketWorldChange() {}

	VOID Do();
	VOID Log();
	VOID Debug();
};