#include "pch.h"
#include ".\Soulworker Packet\SWSPacketHeartbeat.h"
#include ".\Damage Meter\Damage Meter.h"
#include ".\UI\PlayerTable.h"

SWSPacketHeartbeat::SWSPacketHeartbeat(SWSHEADER* swheader, BYTE* data) {
	_swheader = swheader;
	_data = data;
}

VOID SWSPacketHeartbeat::Do() {
	PLAYERTABLE._lastSendTimestamp = GetCurrentTimeStamp();
	return;
}

VOID SWSPacketHeartbeat::Log() {
	return;
}

VOID SWSPacketHeartbeat::Debug() {
	return;
}