#include "pch.h"
#include ".\Language\Region.h"
#include ".\Damage Meter\MySQLite.h"

#ifdef _LANG_KOREAN
#define LANG "KR"
#endif
#ifdef _LANG_ENGLISH
#define LANG "EN"
#endif
#ifdef _LANG_CHINESES
#define LANG "TC"
#endif
#ifdef _LANG_JAPANESE
#define LANG "JP"
#endif

MySQL::MySQL() : _db(nullptr), _memdb(nullptr) {

}

MySQL::~MySQL() {
	FreeDB();
}

BOOL MySQL::InitDB(){

	if (sqlite3_open(SWDBPATH, &_db) != SQLITE_OK) {
		Log::WriteLogA(const_cast<CHAR*>("Error in InitDB : %s"), sqlite3_errmsg(_db));
		sqlite3_close(_db);
		_db = nullptr;

		return FALSE;
	}

	return TRUE;
}

BOOL MySQL::InitMemDB() {

	if (sqlite3_open(":memory:", &_memdb) != SQLITE_OK) {
		Log::WriteLogA(const_cast<CHAR*>("Error in InitMemDB : %s"), sqlite3_errmsg(_memdb));
		sqlite3_close(_memdb);
		_memdb = nullptr;

		return FALSE;
	}

	return TRUE;
}

VOID MySQL::FreeDB() {

	if (_db != nullptr) {
		sqlite3_close(_db);
		_db = nullptr;
	}

	if (_memdb != nullptr) {
		sqlite3_close(_memdb);
		_memdb = nullptr;
	}

	if (_skill_stmt != nullptr) {
		sqlite3_finalize(_skill_stmt);
		_skill_stmt = nullptr;
	}

	if (_monster_stmt != nullptr) {
		sqlite3_finalize(_monster_stmt);
		_monster_stmt = nullptr;
	}

}

BOOL MySQL::InitSkillDB() {

	CHAR* errbuf = nullptr;

	const CHAR* sql = "CREATE TABLE IF NOT EXISTS Skill(Id INTEGER PRIMARY KEY, Name_TC TEXT NOT NULL, Name_EN TEXT NULL);";

	if (sqlite3_exec(_db, sql, 0, 0, &errbuf) != SQLITE_OK) {
		Log::WriteLogA(const_cast<CHAR*>("Error in InitSkillDB : %s"), errbuf);
		sqlite3_free(errbuf);

		return FALSE;
	}

//	const CHAR* sql2 = "SELECT Name From Skill Where Id = ?";
	std::string sql2 = string("SELECT Name_") + string(LANG) + " From Skill Where Id = ?";

	if (sqlite3_prepare_v2(_db, sql2.c_str(), -1, &_skill_stmt, 0) != SQLITE_OK) {
		Log::WriteLogA(const_cast<CHAR*>("Error in sqlite3_prepare_v2 : %s"), sqlite3_errmsg(_db));

		return FALSE;
	}

	return TRUE;
}

BOOL MySQL::InitMonsterDB() {

	CHAR* errbuf = nullptr;

	//const CHAR* sql = "CREATE TABLE IF NOT EXISTS Monster(Db1 INTEGER, Db2 INTEGER, Name_KR TEXT NOT NULL, PRIMARY KEY(Db1, Db2));";
	const CHAR* sql = "CREATE TABLE IF NOT EXISTS Monster(Db2 INTEGER, Name_TC TEXT NOT NULL, Name_EN TEXT NULL, PRIMARY KEY(Db2));";

	if (sqlite3_exec(_db, sql, 0, 0, &errbuf) != SQLITE_OK) {
		Log::WriteLogA(const_cast<CHAR*>("Error in InitMonsterDB : %s"), errbuf);
		sqlite3_free(errbuf);

		return FALSE;
	}

	//std::string sql2 = "SELECT Name_" LANG " From Monster Where Db1 = ? and Db2 = ?";
	std::string sql2 = "SELECT Name_"s + LANG + ", type From Monster Where Db2 = ?"s;

	if (sqlite3_prepare_v2(_db, sql2.c_str(), -1, &_monster_stmt, 0) != SQLITE_OK) {
		Log::WriteLogA(const_cast<CHAR*>("Error in sqlite3_prepare_v2 : %s"), sqlite3_errmsg(_db));

		return FALSE;
	}

	return TRUE;
}

BOOL MySQL::InitMapDB() {
	CHAR* errbuf = nullptr;

	const CHAR* sql = "CREATE TABLE IF NOT EXISTS Map(Id INTEGER PRIMARY KEY, Name_TC TEXT NOT NULL, Name_EN TEXT NULL);";

	if (sqlite3_exec(_db, sql, 0, 0, &errbuf) != SQLITE_OK) {
		Log::WriteLogA(const_cast<CHAR*>("Error in InitMapDB : %s"), errbuf);
		sqlite3_free(errbuf);

		return FALSE;
	}

	std::string sql2 = "SELECT Name_"s + LANG + " From Map Where Id = ?"s;

	if (sqlite3_prepare_v2(_db, sql2.c_str(), -1, &_map_stmt, 0) != SQLITE_OK) {
		Log::WriteLogA(const_cast<CHAR*>("Error in sqlite3_prepare_v2 : %s"), sqlite3_errmsg(_db));

		return FALSE;
	}

	return TRUE;
}

BOOL MySQL::InitBuffDB() {
	CHAR* errbuf = nullptr;

	const CHAR* sql = "CREATE TABLE IF NOT EXISTS Buff (Id INTEGER PRIMARY KEY, Name_EN TEXT NULL, Name_TC TEXT NOT NULL)";

	if (sqlite3_exec(_db, sql, 0, 0, &errbuf) != SQLITE_OK) {
		Log::WriteLogA(const_cast<CHAR*>("Error in InitBuffDB : %s"), errbuf);
		sqlite3_free(errbuf);

		return FALSE;
	}

	std::string sql2 = "SELECT Name_"s + LANG + " From Buff Where Id = ?";

	if (sqlite3_prepare_v2(_db, sql2.c_str(), -1, &_buff_stmt, 0) != SQLITE_OK) {
		Log::WriteLogA(const_cast<CHAR*>("Error in sqlite3_prepare_v2 : %s"), sqlite3_errmsg(_db));

		return FALSE;
	}

	return TRUE;
}

BOOL MySQL::InitSkillTimelineDB() {

	return TRUE;
}

BOOL MySQL::InitBuffTimelineDB() {

	return TRUE;
}

BOOL MySQL::Init() {

	BOOL success = TRUE;

	do {
		if (!InitDB()) {
			success = FALSE;
			break;
		}

		if (!InitSkillDB()) {
			success = FALSE;
			break;
		}

		if (!InitMonsterDB()) {
			success = FALSE;
			break;
		}

		if (!InitMapDB()) {
			success = FALSE;
			break;
		}

		if (!InitMemDB()) {
			success = FALSE;
			break;
		}

		if (!InitBuffDB()) {
			success = FALSE;
			break;
		}

		if (!InitSkillTimelineDB()) {
			success = FALSE;
			break;
		}

		if (!InitBuffTimelineDB()) {
			success = FALSE;
			break;
		}
	} while (FALSE);

	if (!success)
		FreeDB();

	return success;
}

BOOL MySQL::GetSkillName(UINT32 skillId, CHAR* out_buffer, SIZE_T out_buffer_length) {

	if (out_buffer == nullptr || _skill_stmt == nullptr)
		return FALSE;

	sprintf_s(out_buffer, out_buffer_length, "%d", skillId);

	sqlite3_reset(_skill_stmt);

	sqlite3_bind_int(_skill_stmt, 1, skillId);
	INT step = sqlite3_step(_skill_stmt);

	if (step == SQLITE_ROW) {
		const CHAR* result = (const CHAR*)sqlite3_column_text(_skill_stmt, 0);

		if (result == nullptr || strlen(result) > out_buffer_length)
			return FALSE;

		strcpy_s(out_buffer, out_buffer_length, result);
	}

	return TRUE;
}

BOOL MySQL::GetMonsterName(UINT32 DB2, CHAR* out_buffer, SIZE_T out_buffer_length) {

	if (out_buffer == nullptr || _monster_stmt == nullptr)
		return FALSE;

	sprintf_s(out_buffer, out_buffer_length, "%d", DB2);

	sqlite3_reset(_monster_stmt);

	sqlite3_bind_int(_monster_stmt, 1, DB2);

	INT step = sqlite3_step(_monster_stmt);

	if (step == SQLITE_ROW) {
		const CHAR* result = (const CHAR*)sqlite3_column_text(_monster_stmt, 0);

		if (result == nullptr || strlen(result) > out_buffer_length)
			return FALSE;

		strcpy_s(out_buffer, out_buffer_length, result);
	}

	return TRUE;
}

BOOL MySQL::GetMonsterType(UINT32 DB2, INT32* type) {

	if (type == nullptr || _monster_stmt == nullptr)
		return FALSE;

	sqlite3_reset(_monster_stmt);

	sqlite3_bind_int(_monster_stmt, 1, DB2);

	INT step = sqlite3_step(_monster_stmt);

	if (step == SQLITE_ROW) {
		INT32 result = sqlite3_column_int(_monster_stmt, 1);
		memcpy_s(type, sizeof(INT32), &result, sizeof(INT32));
	}

	return TRUE;
}

BOOL MySQL::GetMapName(UINT32 mapID, CHAR* out_buffer, SIZE_T out_buffer_length){

	if (out_buffer == nullptr || _map_stmt == nullptr)
		return FALSE;

	if (mapID == 0) {
		strcpy_s(out_buffer, out_buffer_length, const_cast<CHAR*>(STR_WORLD_NO_INFORMATION));
		return TRUE;
	}

	sprintf_s(out_buffer, out_buffer_length, "%d", mapID);

	sqlite3_reset(_map_stmt);

	sqlite3_bind_int(_map_stmt, 1, mapID);

	INT step = sqlite3_step(_map_stmt);

	if (step == SQLITE_ROW) {
		const CHAR* result = (const CHAR*)sqlite3_column_text(_map_stmt, 0);

		if (result == nullptr || strlen(result) > out_buffer_length)
			return FALSE;

		strcpy_s(out_buffer, out_buffer_length, result);
	}

	return TRUE;
}

BOOL MySQL::GetBuffName(UINT32 buffId, CHAR* out_buffer, SIZE_T out_buffer_length) {

	if (out_buffer == nullptr || _buff_stmt == nullptr)
		return FALSE;

	if (buffId == 0) {
		strcpy_s(out_buffer, out_buffer_length, const_cast<CHAR*>(u8"Unknown"));
		return TRUE;
	}

	sprintf_s(out_buffer, out_buffer_length, "%d", buffId);

	sqlite3_reset(_buff_stmt);

	sqlite3_bind_int(_buff_stmt, 1, buffId);

	INT step = sqlite3_step(_buff_stmt);

	if (step == SQLITE_ROW) {
		const CHAR* result = (const CHAR*)sqlite3_column_text(_buff_stmt, 0);

		if (result == nullptr || strlen(result) > out_buffer_length)
			return FALSE;

		strcpy_s(out_buffer, out_buffer_length, result);
	}

	return TRUE;
}

