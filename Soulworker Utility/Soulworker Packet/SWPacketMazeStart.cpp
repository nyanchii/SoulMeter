#include "pch.h"
#include ".\Soulworker Packet\SWPacketMazeStart.h"
#include ".\Damage Meter\Damage Meter.h"

SWPacketMazeStart::SWPacketMazeStart(SWHEADER* swheader, BYTE* data) : SWPacket(swheader, data) {

}

VOID SWPacketMazeStart::Do() {
	DAMAGEMETER.SetMazeState(FALSE);
}

VOID SWPacketMazeStart::Log() {

}

VOID SWPacketMazeStart::Debug() {
	Log::WriteLog(const_cast<LPTSTR>(_T("[TEST] [MAZE START]")));
}