#pragma once
#include ".\Soulworker Packet\SWPacket.h"
#include ".\Damage Meter\Damage Meter.h"

#pragma pack(push, 1)

typedef struct _SWPACKETOBJECTCREATE {
	BYTE _unknown01;
	UINT32 _id;
	BYTE _unknown02[33];
	UINT32 _owner_id;
	BYTE _unknown03[54];
	UINT32 _db2;
}SWPACKETOBJECTCREATE;

#pragma pack(pop)

class SWPacketObjectCreate : public SWPacket {
protected:
	SWPacketObjectCreate() {}

public:
	SWPacketObjectCreate(SWHEADER* swheader, BYTE* data);
	~SWPacketObjectCreate() {}

	VOID Do();
	VOID Log();
	VOID Debug();
};