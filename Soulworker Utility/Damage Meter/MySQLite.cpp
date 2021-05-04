#include "pch.h"
#include ".\Damage Meter\MySQLite.h"

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

	const CHAR* sql = "CREATE TABLE IF NOT EXISTS Skill(Id INTEGER PRIMARY KEY, Name TEXT NOT NULL);";

	if (sqlite3_exec(_db, sql, 0, 0, &errbuf) != SQLITE_OK) {
		Log::WriteLogA(const_cast<CHAR*>("Error in InitSkillDB : %s"), errbuf);
		sqlite3_free(errbuf);

		return FALSE;
	}

	const CHAR* sql2 = "SELECT Name From Skill Where Id = ?";

	if (sqlite3_prepare_v2(_db, sql2, -1, &_skill_stmt, 0) != SQLITE_OK) {
		Log::WriteLogA(const_cast<CHAR*>("Error in sqlite3_prepare_v2 : %s"), sqlite3_errmsg(_db));

		return FALSE;
	}

	return TRUE;
}

BOOL MySQL::InitMonsterDB() {

	CHAR* errbuf = nullptr;

	const CHAR* sql = "CREATE TABLE IF NOT EXISTS Monster(Db1 INTEGER, Db2 INTEGER, Name TEXT NOT NULL, PRIMARY KEY(Db1, Db2));";

	if (sqlite3_exec(_db, sql, 0, 0, &errbuf) != SQLITE_OK) {
		Log::WriteLogA(const_cast<CHAR*>("Error in InitMonsterDB : %s"), errbuf);
		sqlite3_free(errbuf);

		return FALSE;
	}

	const CHAR* sql2 = "SELECT Name From Monster Where Db1 = ? and Db2 = ?";

	if (sqlite3_prepare_v2(_db, sql2, -1, &_monster_stmt, 0) != SQLITE_OK) {
		Log::WriteLogA(const_cast<CHAR*>("Error in sqlite3_prepare_v2 : %s"), sqlite3_errmsg(_db));

		return FALSE;
	}

	return TRUE;
}

BOOL MySQL::InitMapDB() {
	CHAR* errbuf = nullptr;

	const CHAR* sql = "CREATE TABLE IF NOT EXISTS Map(Id INTEGER PRIMARY KEY, Name TEXT NOT NULL);";

	if (sqlite3_exec(_db, sql, 0, 0, &errbuf) != SQLITE_OK) {
		Log::WriteLogA(const_cast<CHAR*>("Error in InitMapDB : %s"), errbuf);
		sqlite3_free(errbuf);

		return FALSE;
	}

	const CHAR* sql2 = "SELECT Name From Map Where Id = ?";

	if (sqlite3_prepare_v2(_db, sql2, -1, &_map_stmt, 0) != SQLITE_OK) {
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

BOOL MySQL::GetMonsterName(USHORT DB1, UINT32 DB2, CHAR* out_buffer, SIZE_T out_buffer_length) {

	if (out_buffer == nullptr || _monster_stmt == nullptr)
		return FALSE;

	if (DB1 == 0) {
		strcpy_s(out_buffer, out_buffer_length, const_cast<CHAR*>(u8"未知怪物"));
		return TRUE;
	}

	sprintf_s(out_buffer, out_buffer_length, "%d %d", DB1, DB2);

	sqlite3_reset(_monster_stmt);

	sqlite3_bind_int(_monster_stmt, 1, DB1);
	sqlite3_bind_int(_monster_stmt, 2, DB2);

	INT step = sqlite3_step(_monster_stmt);

	if (step == SQLITE_ROW) {
		const CHAR* result = (const CHAR*)sqlite3_column_text(_monster_stmt, 0);

		if (result == nullptr || strlen(result) > out_buffer_length)
			return FALSE;

		strcpy_s(out_buffer, out_buffer_length, result);
	}

	return TRUE;
}

BOOL MySQL::GetMapName(UINT32 mapID, CHAR* out_buffer, SIZE_T out_buffer_length){

	if (out_buffer == nullptr || _map_stmt == nullptr)
		return FALSE;

	if (mapID == 0) {
		strcpy_s(out_buffer, out_buffer_length, const_cast<CHAR*>(u8"未知"));
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