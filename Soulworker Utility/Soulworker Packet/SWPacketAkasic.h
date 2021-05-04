#pragma once
#include ".\Soulworker Packet\SWPacket.h"
#include ".\Damage Meter\Damage Meter.h"

#pragma pack(push, 1)

typedef struct _SWPACKETAKASIC {
	UINT32 _id;
	UINT32 _ownder_id;
}SWPACKETAKASIC;

#pragma pack(pop)

class SWPacketAkasic : public SWPacket {
protected:
	SWPacketAkasic() {}

public:
	SWPacketAkasic(SWHEADER* swheader, BYTE* data);
	~SWPacketAkasic() {}

	VOID Do();
	VOID Log();
	VOID Debug();
};