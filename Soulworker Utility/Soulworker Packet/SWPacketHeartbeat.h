#pragma once
#include ".\Soulworker Packet\SWPacket.h"

#pragma pack(push, 1)
typedef struct _SWPACKETHEARTBEAT {
	BYTE _unknown01; //0430new
	UINT32 _randomNumber; // random
	BYTE _unknown02[4];
}SWPACKETHEARTBEAT;

#pragma pack(pop)

class SWPacketHeartbeat : public SWPacket {
protected:
	SWPacketHeartbeat() {}

public:
	SWPacketHeartbeat(SWHEADER* swheader, BYTE* data);
	~SWPacketHeartbeat() {}

	VOID Do();
	VOID Log();
	VOID Debug();
};