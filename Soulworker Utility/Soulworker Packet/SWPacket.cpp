#include "pch.h"
#include ".\Soulworker Packet\SWPacket.h"
#include ".\Soulworker Packet\PacketType.h"

SWPacket::SWPacket(SWHEADER* swheader, BYTE* data) {
	_swheader = swheader;
	_data = data;
}

VOID SWPacket::Do() {
	return;
}

VOID SWPacket::Log() {
	return;
}

VOID SWPacket::Debug() {
	
	/*if (_swheader->_op != OPcode::PARTY)
		return;

	Log::WriteLogA(const_cast<CHAR*>("[DEBUG] OP : %04x\tsize : %04x\n"), _swheader->_op, _swheader->_size);

	
	for (int i = 0; i < _swheader->_size; i++)
		Log::WriteLogA(const_cast<CHAR*>("%02x "), _data[i]);
	Log::WriteLogA(const_cast<CHAR*>("\n"));*/
	
	
	return;
}