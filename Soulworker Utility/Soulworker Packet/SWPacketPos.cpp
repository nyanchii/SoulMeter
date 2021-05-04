#include "pch.h"
#include ".\Soulworker Packet\SWPacket.h"
#include ".\Damage Meter\Damage Meter.h"
#include ".\Soulworker Packet\SWPacketPos.h"

SWPacketPos::SWPacketPos(SWHEADER* swheader, BYTE* data) : SWPacket(swheader, data) {

}

VOID SWPacketPos::Do() {

	SWPACKETPOS_HEADER* pos_header = (SWPACKETPOS_HEADER*)(_data + sizeof(SWHEADER));

	BYTE* p_data = _data;
	p_data += sizeof(SWHEADER) + sizeof(SWPACKETPOS_HEADER);

	for (int i = 0; i < pos_header->_posPlayerCount; i++) {
		SWPACKETPOS_DATA* pos_data = (SWPACKETPOS_DATA*)p_data;
		BYTE job = *(p_data + sizeof(SWPACKETPOS_DATA) + pos_data->_nickSize + 1);

		WCHAR utf16[MAX_NAME_LEN] = { 0 };
		memcpy_s(utf16, MAX_NAME_LEN * sizeof(WCHAR), p_data + sizeof(SWPACKETPOS_DATA), pos_data->_nickSize);

		CHAR utf8[MAX_NAME_LEN] = { 0 };
		if (!UTF16toUTF8(utf16, utf8, MAX_NAME_LEN)) {
			Log::WriteLog(const_cast<LPTSTR>(_T("Error in SWPacketPos : UTF16toUTF8 FAILED")));
			return;
		}

		DAMAGEMETER.InsertPlayerMetadata(pos_data->_playerID, utf8, job);

		p_data += sizeof(SWPACKETPOS_DATA) + pos_data->_nickSize + SWPACKETPOS_DUMMY;
	}
}

VOID SWPacketPos::Log() {

}

VOID SWPacketPos::Debug() {
}