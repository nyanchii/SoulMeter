#include "pch.h"
#include ".\Soulworker Packet\PacketType.h"
#include ".\Soulworker Packet\SWSPacketMaker.h"
#include ".\Soulworker Packet\SWCrypt.h"

SWSPacketMaker::SWSPacketMaker() {
	_isSegmentation = FALSE;
}

SWSPacketMaker::~SWSPacketMaker() {

}

SWSHEADER* SWSPacketMaker::GetSWSHeader(IPv4Packet* packet) {

	if (packet == nullptr || packet->_datalength < sizeof(SWSHEADER))
		return nullptr;

	SWSHEADER* swheader = (SWSHEADER*)(packet->_data);

	if (swheader->_magic != _SWMAGIC || swheader->_const_value01 != SWCONSTVALUE)
		return nullptr;

	return swheader;

}

BYTE* SWSPacketMaker::GetSWData(IPv4Packet* packet) {

	return (BYTE*)(packet->_data);
}

VOID SWSPacketMaker::Decrypt(BYTE* data, const UINT size, const UINT start, const UINT keyIndex) {
	if (data == nullptr || size < 0 || start < 0)
		return;
	UINT _size = size;

	if (_SWMAGIC == 5) {
		SWCRYPT.DecryptPacket(data + start, size - start, keyIndex);
	}
	else if (_SWMAGIC == 3) {
		_size -= sizeof(SWHEADER) + 3;
		BYTE ecx = data[5];
		for (UINT i = 0; i < _size; i++) {
			data[i + start] ^= _keyTable[16 * (ecx % 16) + (i & 16)];
		}
	}
}

VOID SWSPacketMaker::ResizePacket(IPv4Packet* packet) {

	SWSHEADER* swheader = GetSWSHeader(packet);

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

VOID SWSPacketMaker::ResizePacket(SIZE_T remainsize, IPv4Packet* packet) {

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

DWORD SWSPacketMaker::CheckSegmentation(IPv4Packet* packet) {

	if (_isSegmentation) {
		return REASSAMBLY;
	}
	else {
		SWSHEADER* swheader = GetSWSHeader(packet);

		if (swheader == nullptr)
			return NO_SWHEADER;

		if (packet->_datalength < swheader->_size) {
			_isSegmentation = TRUE;
			return YES_SEGMENTATION;
		}
	}

	return NO_SEGMENTATION;
}

VOID SWSPacketMaker::StartReassambly(IPv4Packet* packet) {

	SWSHEADER* swheader = GetSWSHeader(packet);
	BYTE* data = GetSWData(packet);

	if (swheader == nullptr || data == nullptr)
		return;

	_segmentationPacket.Init(swheader->_size);
	_segmentationPacket.AppendData(data, packet->_datalength);

	ResizePacket(packet);
}

VOID SWSPacketMaker::Reassambly(IPv4Packet* packet) {

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

VOID SWSPacketMaker::CreateSWPacket(IPv4Packet* packet) {

	SWSHEADER* swheader = GetSWSHeader(packet);
	BYTE* data = GetSWData(packet);

	if (swheader == nullptr || data == nullptr)
		return;

	Decrypt(data, swheader->_size, sizeof(SWSHEADER) - 3, swheader->key);

	SWSPacket* swpacket = nullptr;
	//int iRand = (rand() % 9) + 1;
	DAMAGEMETER.GetLock();
	{
		switch (_byteswap_ushort(swheader->_op)) {
		case 0x0106:
			swpacket = new SWSPacketHeartbeat(swheader, data);
			break;
		case 0x0501:
			break;
		case 0x0503:
			break;
		case 0x0522:
			break;
		case 0x0606:
			swpacket = new SWSPacketMyDodgeUsed(swheader, data);
			break;
		case 0x0608:
			swpacket = new SWSPacketMySkillUsed(swheader, data);
			break;
		default:
			//swpacket = new SWSPacket(swheader, data);
			break;
		}

		if (swpacket != nullptr) {
#if DEBUG_CREATESPACKET == 1 // && defined(_DEBUG)
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

VOID SWSPacketMaker::CheckRemainPacket(IPv4Packet* packet) {

	if (packet->_datalength > 0)
		Parse(packet);
}

DWORD SWSPacketMaker::Parse(IPv4Packet* packet) {

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