#pragma once
#include ".\Soulworker Packet\SWPacket.h"
#include ".\Soulworker Packet\SWPacketDamage.h"
#include ".\Soulworker Packet\SWPacketObjectCreate.h"
#include ".\Soulworker Packet\SWPacketWorldChange.h"
#include ".\Soulworker Packet\SWPacketAkasic.h"
#include ".\Soulworker Packet\SWPacketParty.h"
#include ".\Soulworker Packet\SWPacketPos.h"
#include ".\Soulworker Packet\SWPacketMazeStart.h"
#include ".\Soulworker Packet\SWPacketMazeEnd.h"

enum OPcode {
	DAMAGE = 0x1306,
	OBJECTCREATE = 0x1504,
	WORLDCHANGE = 0x0204,
	AKASIC = 0x7b06,
	PARTY = 0x0912,
	POS = 0x092e,
	MAZEEND = 0x7511,	// 임시
	MAZESTART = 0x0804,	// 임시
};