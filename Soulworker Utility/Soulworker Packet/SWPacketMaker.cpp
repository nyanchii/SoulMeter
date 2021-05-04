#include "pch.h"
#include ".\Soulworker Packet\PacketType.h"
#include ".\Soulworker Packet\SWPacketMaker.h"
#include <exception>

SWPacketMaker::SWPacketMaker() {
	_isSegmentation = FALSE;
}

SWPacketMaker::~SWPacketMaker() {

}

SWHEADER* SWPacketMaker::GetSWHeader(IPv4Packet* packet) {

	if (packet == nullptr || packet->_datalength < sizeof(SWHEADER))
		return nullptr;

	SWHEADER* swheader = (SWHEADER*)(packet->_data);

	if (swheader->_magic != SWMAGIC || swheader->_const_value01 != SWCONSTVALUE)
		return nullptr;

	return swheader;
}

BYTE* SWPacketMaker::GetSWData(IPv4Packet* packet) {

	return (BYTE*)(packet->_data);
}

VOID SWPacketMaker::Decrypt(BYTE* data, const UINT size, const UINT start) {

	if (data == nullptr || size < 0 || start < 0)
		return;

	for (UINT i = 0; i < size; i++) 
		//data[i + start] ^= _keyTable[i % 3]; // for krn
		data[i + start] ^= _keyTable[16 * SWMAGIC - 15 * (i/15) + i];

}

VOID SWPacketMaker::ResizePacket(IPv4Packet* packet) {

	SWHEADER* swheader = GetSWHeader(packet);

#if DEBUG_RESIZEPACKET == 1
	Log::WriteLogA(const_cast<CHAR*>("[ResizePacket Before] [Packet Length = %d] [SWPacket Size = %u]"), packet->_datalength, swheader->_size);
#endif

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

		if (swheader == nullptr)
			return NO_SWHEADER;

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

	Decrypt(data, swheader->_size - sizeof(SWHEADER) + 2, sizeof(SWHEADER) - 2);

	SWPacket* swpacket = nullptr;

	DAMAGEMETER.GetLock();
	{
#if DEBUG_PRINTOPCODE == 1
		Log::WriteLogA(const_cast<CHAR*>("[DEBUG] [OPCode] %04x"), swheader->_op);
#endif
		switch (swheader->_op) {
		case OPcode::DAMAGE:
			swpacket = new SWPacketDamage(swheader, data);
			break;
		case OPcode::OBJECTCREATE:
			swpacket = new SWPacketObjectCreate(swheader, data);
			break;
		case OPcode::WORLDCHANGE:
			swpacket = new SWPacketWorldChange(swheader, data);
			break;
		case OPcode::AKASIC:
			swpacket = new SWPacketAkasic(swheader, data);
			break;
		case OPcode::PARTY:
			swpacket = new SWPacketParty(swheader, data);
			break;
		case OPcode::POS:
			/*Log::WriteLogA(const_cast<CHAR*>("[PARTY] OP : %04x\tsize : %04x\n"), swheader->_op, swheader->_size);

			for (int i = 0; i < swheader->_size; i++)
				Log::WriteLogANoDate(const_cast<CHAR*>("%02x "), data[i]);
			Log::WriteLogANoDate(const_cast<CHAR*>("\n"));*/
			swpacket = new SWPacketPos(swheader, data);
			break;
		case OPcode::MAZESTART:
			swpacket = new SWPacketMazeStart(swheader, data);
			break;
		case OPcode::MAZEEND:
			swpacket = new SWPacketMazeEnd(swheader, data);
			break;
		default:
#if DEBUG_CREATEPACKET == 1 // save cpu use and memory
			swpacket = new SWPacket(swheader, data);
#endif
			break;
		}

		if (swpacket != nullptr) {
#if DEBUG_CREATEPACKET == 1
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

int SWPacketMaker::bytesToInt(byte* bytes, int size)
{
	int addr = bytes[0] & 0xFF;
	addr |= ((bytes[1] << 8) & 0xFF00);
	addr |= ((bytes[2] << 16) & 0xFF0000);
	addr |= ((bytes[3] << 24) & 0xFF000000);
	return addr;
}