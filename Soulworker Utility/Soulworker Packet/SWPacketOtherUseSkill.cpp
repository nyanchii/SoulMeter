#include "pch.h"
#include ".\Soulworker Packet\SWPacket.h"
#include ".\Damage Meter\Damage Meter.h"
#include ".\Damage Meter\MySQLite.h"
#include ".\Soulworker Packet\SWPacketOtherUseSkill.h"
#include ".\UI\UtillWindow.h"

SWPacketOtherUseSkill::SWPacketOtherUseSkill(SWHEADER* swheader, BYTE* data) : SWPacket(swheader, data) {

}

VOID SWPacketOtherUseSkill::Do() {
	//	SWPACKETCHAT_HEADER* chat_header = (SWPACKETCHAT_HEADER*)(_data + sizeof(SWHEADER));

	//	BYTE* p_data = _data + sizeof(SWHEADER) + sizeof(SWPACKETCHAT_HEADER);

	SWPACKET_OTHERUSESKILL* otherSkill = (SWPACKET_OTHERUSESKILL*)(_data + sizeof(SWHEADER));

	DAMAGEMETER.AddSkillUsed(otherSkill->_playerId, otherSkill->_skillId);

	/*UINT32 userId = otherSkill->_playerId;
	if (!DAMAGEMETER.CheckPlayer(userId)) {
		UINT32 ownerId = DAMAGEMETER.GetOwnerID(userId);
		if (!DAMAGEMETER.CheckPlayer(ownerId)) {
			SW_DB2_STRUCT* db = DAMAGEMETER.GetMonsterDB(userId);
			UINT32 db2 = 0;
			if (db != nullptr) {
				db2 = db->_db2;
			}

			if (db2 == 31310101) {
				//Log::MyLog(_T("1"));
				ZeroMemory(_skillName, SKILL_NAME_LEN);
			//	Log::MyLog(_T("2"));
				SWDB.GetSkillName(otherSkill->_skillId, _skillName, SKILL_NAME_LEN);
				//Log::MyLog(_T("3"));
				string test = u8" [";
				test.append(_skillName);
				test.append(u8"]");
				UTILLWINDOW.AddLog(test);
				//Log::MyLog(test.c_str());
			}
		}
	}*/


}

VOID SWPacketOtherUseSkill::Log() {

}

VOID SWPacketOtherUseSkill::Debug() {

	//Log::MyLog(_T("UseSkill Packet\n"));
	//for (int i = sizeof(SWHEADER); i < _swheader->_size; i++)
	//	Log::MyLog(_T("%02x "), _data[i]);
	//Log::MyLog(_T("\n"));

	//SWPACKET_OTHERUSESKILL* otherSkill = (SWPACKET_OTHERUSESKILL*)(_data + sizeof(SWHEADER));

	//Log::MyLog(_T("id : %08x / skill : %u / x : %f / y : %f / z : %f\n"), otherSkill->_playerId, otherSkill->_skillId, otherSkill->_locationX, otherSkill->_locationY, otherSkill->_locationZ);

	//Log::MyLog(_T("Unknown01 : "));
	//for (int i = 0; i < sizeof(otherSkill->_unknown01); i++) {
	//	Log::MyLog(_T("%02x "), otherSkill->_unknown01[i]);
	//}
	//Log::MyLog(_T("\n"));

}