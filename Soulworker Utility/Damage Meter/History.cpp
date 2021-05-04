#include "pch.h"
#include ".\Damage Meter\History.h"
#include ".\Damage Meter\Damage Meter.h"

VOID _HISTORYINFO::Setup(vector<SWDamagePlayer*>& history, UINT32 worldID, FLOAT time) {
	_history = history;
	_worldID = worldID;
	_time = time;
	GetLocalTime(&_saveTime);
}

VOID _HISTORYINFO::Clear(){
	
	for (auto itr = _history.begin(); itr != _history.end(); itr++)
		delete (*itr);

	_history.clear();
	_worldID = 0;
	ZeroMemory(&_saveTime, sizeof(SYSTEMTIME));

}

SWDamageMeterHistory::~SWDamageMeterHistory() {
	for (INT i = 0; i < HISTORY_SIZE; i++) {
		_historys[i].Clear();
	}
}

VOID SWDamageMeterHistory::ClearHistory(INT index) {

	_historys[index].Clear();
}

VOID SWDamageMeterHistory::push_back(vector<SWDamagePlayer*>& playerInfo) {

	ClearHistory(_curIndex % HISTORY_SIZE);
	_historys[_curIndex++ % HISTORY_SIZE].Setup(playerInfo, DAMAGEMETER.GetWorldID(), DAMAGEMETER.GetTime());
}

const HISTORY_INFO& SWDamageMeterHistory::operator[](INT index) {
	
	if (index >= HISTORY_SIZE || index < 0)
		assert("History index out of bounds");

	return _historys[index];
}

SIZE_T SWDamageMeterHistory::size() {

	if (_curIndex >= HISTORY_SIZE)
		return HISTORY_SIZE;
	else
		return _curIndex % HISTORY_SIZE;
}

SIZE_T SWDamageMeterHistory::getCurrentIndex() {
	return _curIndex;
}