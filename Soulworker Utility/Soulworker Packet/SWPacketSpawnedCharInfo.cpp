#include "pch.h"
#include ".\Soulworker Packet\SWPacket.h"
#include ".\Damage Meter\Damage Meter.h"
#include ".\Soulworker Packet\SWPacketSpawnedCharInfo.h"

SWPacketSpawnedCharInfo::SWPacketSpawnedCharInfo(SWHEADER* swheader, BYTE* data) : SWPacket(swheader, data) {

}

VOID SWPacketSpawnedCharInfo::Do() {

	SWPACKETSPAWNEDCHARINFO_HEADER* spawned_header = (SWPACKETSPAWNEDCHARINFO_HEADER*)(_data + sizeof(SWHEADER));

	BYTE* p_data = _data;
	p_data += sizeof(SWHEADER) + sizeof(SWPACKETSPAWNEDCHARINFO_HEADER);

	BYTE job = *(p_data + spawned_header->_nameSize + 1);

	WCHAR utf16[MAX_NAME_LEN] = { 0 };
	memcpy_s(utf16, MAX_NAME_LEN * sizeof(WCHAR), p_data, spawned_header->_nameSize);

	CHAR utf8[MAX_NAME_LEN] = { 0 };
	if (!UTF16toUTF8(utf16, utf8, MAX_NAME_LEN)) {
		//Log::WriteLog(const_cast<LPTSTR>(_T("Error in SWPacketPos : UTF16toUTF8 FAILED")));
		return;
	}


	//DAMAGEMETER.InsertPlayerMetadata(spawned_header->_playerId, utf8, job);

	//Log::MyLog(_T("id : %d / nameSize : %u / name : %s / job : %u\n"), spawned_header->_playerId, spawned_header->_nameSize, utf16, job);


}

VOID SWPacketSpawnedCharInfo::Log() {

}


VOID SWPacketSpawnedCharInfo::Debug() {


	//SWPACKETOBJECTCREATE* obj_create = (SWPACKETOBJECTCREATE*)(_data + sizeof(SWHEADER));

	//Log::MyLog(_T("unknown02\n"));
	//for (int i = 0; i < 17; i++)
	//	Log::MyLog(_T("%02x "), obj_create->_unknown02[i]);
	//Log::MyLog(_T("\n"));

	//Log::MyLog(_T("unknown03\n"));
	//for (int i = 0; i < 54; i++)
	//	Log::MyLog(_T("%02x "), obj_create->_unknown03[i]);
	//Log::MyLog(_T("\n"));

	//Log::WriteLogA(const_cast<CHAR*>("[DEBUG] [Created Object] [X = %f] [Y = %f] [Z = %f]"), obj_create->_locationX, obj_create->_locationY, obj_create->_locationZ);
	//Log::WriteLogA(const_cast<CHAR*>("[DEBUG] [Created Object] [X = %f] [Y = %f] [Z = %f]"), obj_create->_unknownFloat1, obj_create->_unknownFloat2, obj_create->_unknownFloat3);


	//Log::WriteLogA(const_cast<CHAR*>("[DEBUG] [Created Object] [ID = %08x] [Owner ID = %08x] [DB2 = %u] [RealDB2 = %u]"), obj_create->_id, obj_create->_owner_id, obj_create->_db2, obj_create->_realDB2);
}