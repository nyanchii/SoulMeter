#pragma once
#include ".\Soulworker Packet\SWPacket.h"

class SWPacketMazeEnd : public SWPacket {
protected:
	SWPacketMazeEnd() {}

public:
	SWPacketMazeEnd(SWHEADER* swheader, BYTE* data);
	~SWPacketMazeEnd() {}

	VOID Do();
	VOID Log();
	VOID Debug();
};