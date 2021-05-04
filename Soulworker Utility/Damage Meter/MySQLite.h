#pragma once
#include "Third Party\SQLite\include\sqlite3.h"

#define SWDB MySQL::getInstance()
#define SWDBPATH "SWDB.db"

#define DEBUG_SKILL_NAME 1
#define DEBUG_MONSTER_NAME 1

class MySQL : public Singleton<MySQL> {
private:
	sqlite3* _db;
	sqlite3* _memdb;

	BOOL InitDB();
	BOOL InitMemDB();
	VOID FreeDB();

	BOOL InitSkillDB();
	BOOL InitMonsterDB();
	BOOL InitMapDB();
	BOOL InitSkillTimelineDB();
	BOOL InitBuffTimelineDB();

	sqlite3_stmt* _skill_stmt;
	sqlite3_stmt* _monster_stmt;
	sqlite3_stmt* _map_stmt;

public:
	MySQL();
	~MySQL();

	BOOL Init();
	BOOL GetSkillName(UINT32 skillId, _Out_ CHAR* out_buffer, _In_ SIZE_T out_buffer_length);
	BOOL GetMonsterName(USHORT DB1, UINT32 DB2, _Out_ CHAR* out_buffer, _In_ SIZE_T out_buffer_length);
	BOOL GetMapName(UINT32 mapId, _Out_ CHAR* out_buffer, _In_ SIZE_T out_buffer_length);
};