#include "pch.h"
#include ".\Soulworker Packet\SWPacket.h"
#include ".\Damage Meter\Damage Meter.h"
#include ".\Soulworker Packet\SWPacketAkasic.h"

SWPacketAkasic::SWPacketAkasic(SWHEADER* swheader, BYTE* data) : SWPacket(swheader, data) {

}

VOID SWPacketAkasic::Do() {

	SWPACKETAKASIC* akasic = (SWPACKETAKASIC*)(_data + sizeof(SWHEADER));

	DAMAGEMETER.InsertOwnerID(akasic->_id, akasic->_ownder_id);
}

VOID SWPacketAkasic::Log() {

}

VOID SWPacketAkasic::Debug() {
	SWPACKETAKASIC* akasic = (SWPACKETAKASIC*)(_data + sizeof(SWHEADER));

	Log::WriteLogA(const_cast<CHAR*>("[DEBUG] [Akasic] [ID = %08x] [Owner ID = %08x]"), akasic->_id, akasic->_ownder_id);
}