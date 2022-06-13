#include "pch.h"
#include ".\Soulworker Packet\SWPacket.h"
#include ".\Soulworker Packet\SWPacketPartyListInfo.h"
#include ".\Damage Meter\Damage Meter.h"

SWPacketPartyListInfo::SWPacketPartyListInfo(SWHEADER* swheader, BYTE* data) : SWPacket(swheader, data) {

}

VOID SWPacketPartyListInfo::Do() {


	//Log::MyLog(_T("Party Info\n"));
	//for (int i = sizeof(SWHEADER); i < _swheader->_size; i++)
	//	Log::MyLog(_T("%02x "), _data[i]);
	//Log::MyLog(_T("\n"));

	BYTE* p_data = _data;
	p_data += sizeof(SWHEADER);
	BYTE partyCount = *p_data;
	//Log::MyLog("Party Counts : %u\n", partyCount);
	p_data += 1;

	//Log::MyLog("Unknown01 : %04x\n", *((USHORT*)p_data));
	p_data += 2;
	//Log::MyLog("\n");

	for (int i = 0; i < partyCount; i++) {
		USHORT unknown01 = *((USHORT*)p_data);
		p_data += 2;

		BYTE unknown02 = *p_data;
		p_data += 1;

		USHORT partyNameLength = *((USHORT*)p_data);
		p_data += 2;

		WCHAR partyName_utf16[MAX_NAME_LEN] = { 0 };
		memcpy_s(partyName_utf16, MAX_NAME_LEN * sizeof(WCHAR), p_data, partyNameLength);
		CHAR partyName_utf8[MAX_NAME_LEN] = { 0 };
		if (!UTF16toUTF8(partyName_utf16, partyName_utf8, MAX_NAME_LEN)) {
			//Log::WriteLog(const_cast<LPTSTR>(_T("Error in SWPacketParty : UTF16toUTF8 FAILED")));
			return;
		}
		p_data += partyNameLength;

		USHORT minLv = *((USHORT*)p_data);
		p_data += 2;
		USHORT maxLv = *((USHORT*)p_data);
		p_data += 2;

		BYTE unknown03 = *p_data;
		p_data += 1;

		USHORT leaderNameLength = *((USHORT*)p_data);
		p_data += 2;

		WCHAR leaderName_utf16[MAX_NAME_LEN] = { 0 };
		memcpy_s(leaderName_utf16, MAX_NAME_LEN * sizeof(WCHAR), p_data, leaderNameLength);
		CHAR leaderName_utf8[MAX_NAME_LEN] = { 0 };
		if (!UTF16toUTF8(leaderName_utf16, leaderName_utf8, MAX_NAME_LEN)) {
			//Log::WriteLog(const_cast<LPTSTR>(_T("Error in SWPacketParty : UTF16toUTF8 FAILED")));
			return;
		}
		p_data += leaderNameLength;

		BYTE partyMemberCount = *p_data;
		p_data += 1;

		UINT32 remainedTime = *((UINT32*)p_data);
		p_data += 4;

		USHORT mapId = *((USHORT*)p_data);
		p_data += 2;

		USHORT unknown04 = *((USHORT*)p_data);
		p_data += 2;

		UINT32 partyId = *((UINT32*)p_data);
		p_data += 4;

		UINT32 leaderId = *((UINT32*)p_data);
		p_data += 4;

		BYTE unknown05 = *p_data; // 1=teammates 2=enemy
		p_data += 1;
		BYTE unknown06 = *p_data;
		p_data += 1;

		//Log::MyLog(_T("unknown 01 : %04x / 2: %02x / 3: %02x / 4 : %04x / 5 : %02x / 6 : %02x\n")
		//	, unknown01, unknown02, unknown03, unknown04, unknown05, unknown06);
		//Log::MyLog(_T("Party Name : %s\n"), partyName_utf16);
		//Log::MyLog(_T("memberCounts : %u / minLvl : %u, maxLv : %u \n"), partyMemberCount, minLv, maxLv);
		//Log::MyLog(_T("Leader Name : %s / LeaderId : %u\n"), leaderName_utf16, leaderId);
		//Log::MyLog(_T("PartyId : %u / mapId : %u / remaindTime : %u \n"), partyId, mapId, remainedTime);
		//Log::MyLog("\n");
	}
}

VOID SWPacketPartyListInfo::Log() {

}

VOID SWPacketPartyListInfo::Debug() {

	//SWPACKETPARTY_HEADER* party_header = (SWPACKETPARTY_HEADER*)(_data + sizeof(SWHEADER));

	//BYTE* p_data = _data;
	//p_data += sizeof(SWHEADER) + sizeof(SWPACKETPARTY_HEADER);

	//for (int i = 0; i < party_header->_partyPlayerCount; i++) {
	//	SWPACKETPARTY_DATA* party_data = (SWPACKETPARTY_DATA*)p_data;
	//	BYTE job = *(p_data + sizeof(SWPACKETPARTY_DATA) + party_data->_nickSize + 1);

	//	WCHAR utf16[MAX_NAME_LEN] = { 0 };
	//	memcpy_s(utf16, MAX_NAME_LEN * sizeof(WCHAR), p_data + sizeof(SWPACKETPARTY_DATA), party_data->_nickSize);

	//	CHAR utf8[MAX_NAME_LEN] = { 0 };
	//	if (!UTF16toUTF8(utf16, utf8, MAX_NAME_LEN)) {
	//		//Log::WriteLog(const_cast<LPTSTR>(_T("Error in SWPacketParty : UTF16toUTF8 FAILED")));
	//		return;
	//	}

	//	//Log::WriteLog(const_cast<LPTSTR>(_T("[DEBUG] [PARTY DATA] [PARTY PLAYER %d] [ID %08x] [NAME = %s] [NICKMEMSIZE = %d] [JOB = %d]")), party_data->_playerID, utf16, party_data->_nickSize, job);

	//	p_data += sizeof(SWPACKETPARTY_DATA) + party_data->_nickSize + SWPACKETPARTY_DUMMY;
	//}
}