#include "pch.h"
#include ".\Soulworker Packet\PacketType.h"
#include ".\Soulworker Packet\SWPacketMaker.h"
#include ".\Soulworker Packet\SWCrypt.h"

SWPacketMaker::SWPacketMaker() {
	_isSegmentation = FALSE;
}

SWPacketMaker::~SWPacketMaker() {

}

SWHEADER* SWPacketMaker::GetSWHeader(IPv4Packet* packet) {

	if (packet == nullptr || packet->_datalength < sizeof(SWHEADER)) {
		return nullptr;
	}
		

	SWHEADER* swheader = (SWHEADER*)(packet->_data);

	if (swheader->_magic != _SWMAGIC || swheader->_const_value01 != SWCONSTVALUE) {
		return nullptr;
	}
		

	return swheader;

}

BYTE* SWPacketMaker::GetSWData(IPv4Packet* packet) {

	return (BYTE*)(packet->_data);
}

VOID SWPacketMaker::Decrypt(BYTE* data, const UINT size, const UINT start, const INT32 keyIndex) {

	if (data == nullptr || size < 0 || start < 0)
		return;
	UINT32 _size = size;

	if (_SWMAGIC == 5) {
		SWCRYPT.DecryptPacket(data + start, size - start, keyIndex);
	}
	else if (_SWMAGIC == 3) {
		_size -= sizeof(SWHEADER) + 3;
		BYTE ecx = keyIndex;
		for (UINT i = 0; i < _size; i++) {
			data[i + start] ^= _keyTable[16 * (ecx % 16) + (i & 0xF)];
		}
	}
}

VOID SWPacketMaker::ResizePacket(IPv4Packet* packet) {

	SWHEADER* swheader = GetSWHeader(packet);

#if DEBUG_RESIZEPACKET == 1
	Log::WriteLogA(const_cast<CHAR*>("[ResizePacket Before] [Packet Length = %d] [SWPacket Size = %u]"), packet->_datalength, swheader->_size);
#endif

	if (swheader == nullptr)
		return;

	if (packet->_datalength >= swheader->_size) {
		packet->_datalength -= swheader->_size;
		packet->_data += swheader->_size;
	}
	else {
		packet->_datalength = 0;
	}

#if DEBUG_RESIZEPACKET == 1
	Log::WriteLogA(const_cast<CHAR*>("[ResizePacket After] [Packet Length = %d] [SWPacket Size = %u]"), packet->_datalength, swheader->_size);
#endif
}

VOID SWPacketMaker::ResizePacket(SIZE_T remainsize, IPv4Packet* packet) {

#if DEBUG_RESIZEPACKET == 1
	Log::WriteLogA(const_cast<CHAR*>("[ResizePacket Before] [Packet Length = %d] [remainSize = %d]"), packet->_datalength, remainsize);
#endif

	if (packet->_datalength >= remainsize) {
		packet->_datalength -= remainsize;
		packet->_data += remainsize;
	}
	else {
		packet->_datalength = 0;
	}

#if DEBUG_RESIZEPACKET == 1
	Log::WriteLogA(const_cast<CHAR*>("[ResizePacket After] [Packet Length = %d] [remainSize = %d]"), packet->_datalength, remainsize);
#endif
}

DWORD SWPacketMaker::CheckSegmentation(IPv4Packet* packet) {
	if (_isSegmentation) {
		return REASSAMBLY;
	}
	else {
		SWHEADER* swheader = GetSWHeader(packet);

		if (swheader == nullptr) {
			return NO_SWHEADER;
		}
			
		if (packet->_datalength < swheader->_size) {
			_isSegmentation = TRUE;
			return YES_SEGMENTATION;
		}
	}

	return NO_SEGMENTATION;
}

VOID SWPacketMaker::StartReassambly(IPv4Packet* packet) {

	SWHEADER* swheader = GetSWHeader(packet);
	BYTE* data = GetSWData(packet);

	if (swheader == nullptr || data == nullptr)
		return;

	_segmentationPacket.Init(swheader->_size);
	_segmentationPacket.AppendData(data, packet->_datalength);

	ResizePacket(packet);
}

VOID SWPacketMaker::Reassambly(IPv4Packet* packet) {

	BYTE* data = GetSWData(packet);

#if DEBUG_SEGMENTATION == 1
	Log::WriteLogA(const_cast<CHAR*>("[Reassambly] [datalength = %d]"), packet->_datalength);
#endif

	SIZE_T remainsize = _segmentationPacket.GetRemainSize();

	DWORD result = SEGMENTATION_SUCCESS;

	if (packet->_datalength >= remainsize) {
		result = _segmentationPacket.AppendData(data, remainsize);
	}
	else if (packet->_datalength < remainsize) {
		result = _segmentationPacket.AppendData(data, packet->_datalength);
	}

	ResizePacket(remainsize, packet);

	switch (result) {
	case SEGMENTATION_SUCCESS: {
#if DEBUG_SEGMENTATION == 1
		IPv4Packet* test = _segmentationPacket.GetData();
		Log::WriteLogA(const_cast<CHAR*>("[Reassambly Success] [datalength = %d]"), test->_datalength);

		for (int i = 0; i < test->_datalength; i++)
			printf("%02x ", test->_data[i]);
		printf("\n");
#endif
		CreateSWPacket(_segmentationPacket.GetData());
		_isSegmentation = FALSE;
	}
		break;
	case SEGMENTATION_FAILED:
		break;
	}
}

VOID SWPacketMaker::CreateSWPacket(IPv4Packet* packet) {

	SWHEADER* swheader = GetSWHeader(packet);
	BYTE* data = GetSWData(packet);

	if (swheader == nullptr || data == nullptr)
		return;

	Decrypt(data, swheader->_size, sizeof(SWHEADER) - 3, swheader->key);

	SWPacket* swpacket = nullptr;
	DAMAGEMETER.GetLock();
	{
		switch (_byteswap_ushort(swheader->_op)) {
			/* 0x01*/
		case OPcode::HEARTBEAT:
			swpacket = new SWPacketHeartbeat(swheader, data);
			break;

			/*0x03*/
		case OPcode::STATCHANGE: //0430
			swpacket = new SWPacketStatChange(swheader, data);
			break;
		case OPcode::DEAD:
			swpacket = new SWPacketDead(swheader, data);
			break;

			/*0x04*/
		case OPcode::WORLDCHANGE: //0430
			swpacket = new SWPacketWorldChange(swheader, data);
			break;
		case OPcode::MAZESTART:
			swpacket = new SWPacketMazeStart(swheader, data);
			break;
		case OPcode::SPAWNED_CHARINFO:
			//swpacket = new SWPacketSpawnedCharInfo(swheader, data);
			break;
		case OPcode::IN_INFO_MONSTER: //0605
			swpacket = new SWPacketInInfoMonster(swheader, data);
			break;
		case OPcode::OTHER_INFOS_MONSTER:
			swpacket = new SWPacketOtherInfosMonster(swheader, data);
			break;

			/*0x05*/
		case OPcode::STARTMOVE:
			//swpacket = new SWPacketStartMove(swheader, data);
			break;
		case OPcode::STOPMOVE:
			break;
		case OPcode::JUMP:
			break;
		case OPcode::CANCEL_WITHMOVE:
			break;

			/*0x06 Combat*/
		//case OPcode::EVADE:
		//	break;
		case OPcode::USESKILL:
			//swpacket = new SWPacketUseSkill(swheader, data);
			break;
		case OPcode::OTHER_USESKILL: //0430
			swpacket = new SWPacketOtherUseSkill(swheader, data);
			break;
		case OPcode::DAMAGE: //0430
			swpacket = new SWPacketDamage(swheader, data);
			break;
		case OPcode::BUFFIN: //0430
			swpacket = new SWPacketBuffIn(swheader, data);
			break;
		case OPcode::BUFFOUT: //0430
			swpacket = new SWPacketBuffOut(swheader, data);
			break;
		case OPcode::PROJECTILE:
			break;
		case OPcode::AKASIC:
			swpacket = new SWPacketAkasic(swheader, data);
			break;
		case OPcode::COOLDOWN: //0430
			swpacket = new SWPacketCooldown(swheader, data);
			break;

			/*0x07*/
		case OPcode::CHAT:
			//swpacket = new SWPacketChat(swheader, data);
			break;

			/* 0x11 */
		case OPcode::MAZEEND:
			swpacket = new SWPacketMazeEnd(swheader, data);
			break;

			/* 0x12 Party */
		case OPcode::PARTY: //0430
			swpacket = new SWPacketParty(swheader, data);
			break;
		case OPcode::PARTY_LIST_INFO:
			//swpacket = new SWPacketPartyListInfo(swheader, data);
			break;

			/* 0x17 Monster */
		case OPcode::MONSTER_STAT_UPDATE:
			swpacket = new SWPacketMonsterStatUpdate(swheader, data);
			break;
		case OPcode::MONSTER_KILLED: //0430
			swpacket = new SWPacketMonsterKilled(swheader, data);
			break;
		case OPcode::AGGRO_CHANGED: //0430
			swpacket = new SWPacketAggroChanged(swheader, data);
			break;

			/* 0x23 Gesture*/
		case OPcode::GESTURE_USED:
			//swpacket = new SWPacketGestureUsed(swheader, data);
			break;

			/* 0x2e Force*/
			// 8 players party
		case OPcode::POS: //0430
			swpacket = new SWPacketPos(swheader, data);
			break;

		default:
#if DEBUG_DISPLAYPKT == 1
			Log::WriteLogA("OP : %04x\tsize : %04x", swheader->_op, swheader->_size);
			for (int i = 0; i < swheader->_size; i++)
				Log::WriteLogNoDate(L"%02x ", data[i]);
			Log::WriteLogNoDate(L"\n\n");
#endif
			break;
		}

		if (swpacket != nullptr) {
#if DEBUG_CREATEPACKET == 1 // && defined(_DEBUG)
			swpacket->Debug();
#endif
			// Todo
			swpacket->Do();

			delete swpacket;
		}
	}
	DAMAGEMETER.FreeLock();

	ResizePacket(packet);
}

VOID SWPacketMaker::CheckRemainPacket(IPv4Packet* packet) {
	
	if (packet->_datalength > 0)
		Parse(packet);
}

DWORD SWPacketMaker::Parse(IPv4Packet* packet) {

	if (packet == nullptr)
		return ERROR_INVALID_PARAMETER;

	switch (CheckSegmentation(packet)) {
	case NO_SEGMENTATION:
		CreateSWPacket(packet);
		break;
	case YES_SEGMENTATION:
		StartReassambly(packet);
		break;
	case REASSAMBLY:
		Reassambly(packet);
		break;
	default:
		return ERROR_FUNCTION_FAILED;
	}

	CheckRemainPacket(packet);

	return ERROR_SUCCESS;
}

char* SWPacketMaker::GetKeyInfo() {
	return _keyInfo;

}