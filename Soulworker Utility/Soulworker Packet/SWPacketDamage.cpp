#include "pch.h"
#include ".\Soulworker Packet\SWPacket.h"
#include ".\Damage Meter\Damage Meter.h"
#include ".\Soulworker Packet\SWPacketDamage.h"

SWPacketDamage::SWPacketDamage(SWHEADER* swheader, BYTE* data) : SWPacket(swheader, data) {

}

BOOL SWPacketDamage::CheckCritical(BYTE damageType) {
	if (damageType & 0x04)
		return TRUE;
	else
		return FALSE;
}

VOID SWPacketDamage::Do() {
	BYTE monsterNum = *(_data + sizeof(SWHEADER));
	SWPACKETDAMAGE_PLAYER* player = (SWPACKETDAMAGE_PLAYER*)((_data + sizeof(_SWPACKETDAMAGE_MONSTER) * monsterNum) + sizeof(SWHEADER) + 1);

	if (!DAMAGEMETER.CheckPlayer(player->_playerID)) {
		UINT32 owner_id = 0xffffffff;
		if ((owner_id = DAMAGEMETER.GetOwnerID(player->_playerID)) == 0xffffffff) {
			return;
		}
		else {if (!DAMAGEMETER.CheckPlayer(owner_id)) {
				return;
			}
			else {
				player->_playerID = owner_id;
			}
		}
	}

	for (int i = 0; i < monsterNum; i++) {
		SWPACKETDAMAGE_MONSTER* monster = (SWPACKETDAMAGE_MONSTER*)((_data + sizeof(SWPACKETDAMAGE_MONSTER) * i) + sizeof(SWHEADER) + 1);

		DAMAGEMETER.AddDamage(player->_playerID, monster->_totalDMG, monster->_criticalDMG, player->_maxCombo != 0 ? 1 : 0, player->_maxCombo != 0 ? CheckCritical(monster->_damageType) : 0, player->_maxCombo, monster->_monsterID, player->_skillID);
	}
}

VOID SWPacketDamage::Debug() {
	BYTE monsterNum = *(_data + sizeof(SWHEADER));
	SWPACKETDAMAGE_PLAYER* player = (SWPACKETDAMAGE_PLAYER*)((_data + sizeof(_SWPACKETDAMAGE_MONSTER) * monsterNum) + sizeof(SWHEADER) + 1);

	Log::WriteLogA(const_cast<CHAR*>("[DEBUG] [MonsterNum = %d] [CheckPlayer = %d] [GetOwnerID = %08x] [OwnerCheckPlayer = %d]"), monsterNum, DAMAGEMETER.CheckPlayer(player->_playerID), DAMAGEMETER.GetOwnerID(player->_playerID), DAMAGEMETER.CheckPlayer(DAMAGEMETER.GetOwnerID(player->_playerID)));

	if (!DAMAGEMETER.CheckPlayer(player->_playerID)) {
		UINT32 owner_id = 0xffffffff;
		if ((owner_id = DAMAGEMETER.GetOwnerID(player->_playerID)) == 0xffffffff) {
			return;
		}
		else {
			if (!DAMAGEMETER.CheckPlayer(owner_id)) {
				return;
			}
			else {
				player->_playerID = owner_id;
			}
		}
	}

	for (int i = 0; i < monsterNum; i++) {
		SWPACKETDAMAGE_MONSTER* monster = (SWPACKETDAMAGE_MONSTER*)((_data + sizeof(SWPACKETDAMAGE_MONSTER) * i) + sizeof(SWHEADER) + 1);

		Log::WriteLogA(const_cast<CHAR*>("\t[PlayerID = %08x] [MonsterID = %08x] [SkillID = %08x] [Damage = %d] [CritDamage = %d] [MaxCombo = %d] [HIT = %d] [CritHit = %d]"),
			player->_playerID, monster->_monsterID, player->_skillID, monster->_totalDMG, monster->_criticalDMG, player->_maxCombo, player->_maxCombo != 0 ? 1 : 0, CheckCritical(monster->_damageType));
	}
}