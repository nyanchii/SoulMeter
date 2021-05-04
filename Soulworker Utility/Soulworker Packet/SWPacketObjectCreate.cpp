#include "pch.h"
#include ".\Soulworker Packet\SWPacket.h"
#include ".\Damage Meter\Damage Meter.h"
#include ".\Soulworker Packet\SWPacketObjectCreate.h"

SWPacketObjectCreate::SWPacketObjectCreate(SWHEADER* swheader, BYTE* data) : SWPacket(swheader, data) {

}

VOID SWPacketObjectCreate::Do() {

	SWPACKETOBJECTCREATE* obj_create = (SWPACKETOBJECTCREATE*)(_data + sizeof(SWHEADER));

	DAMAGEMETER.InsertOwnerID(obj_create->_id, obj_create->_owner_id);
	DAMAGEMETER.InsertDB(obj_create->_id, obj_create->_db2);
}

VOID SWPacketObjectCreate::Log() {

}

VOID SWPacketObjectCreate::Debug() {
	SWPACKETOBJECTCREATE* obj_create = (SWPACKETOBJECTCREATE*)(_data + sizeof(SWHEADER));

	Log::WriteLogA(const_cast<CHAR*>("[DEBUG] [Created Object] [ID = %08x] [Owner ID = %08x] [DB2 = %08x]"), obj_create->_id, obj_create->_owner_id, obj_create->_db2);
}