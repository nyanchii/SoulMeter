#include "pch.h"
#include ".\Soulworker Packet\SWPacketMazeEnd.h"
#include ".\Damage Meter\Damage Meter.h"

SWPacketMazeEnd::SWPacketMazeEnd(SWHEADER* swheader, BYTE* data) : SWPacket(swheader, data) {

}

VOID SWPacketMazeEnd::Do() {
	DAMAGEMETER.SetMazeState(TRUE);
	DAMAGEMETER.Suspend();
	DAMAGEMETER.SaveHistory();
}

VOID SWPacketMazeEnd::Log() {

}

VOID SWPacketMazeEnd::Debug() {
	Log::WriteLog(const_cast<LPTSTR>(_T("[TEST] [Maze End]")));
}