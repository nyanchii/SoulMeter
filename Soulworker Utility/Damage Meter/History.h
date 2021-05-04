#pragma once
#include ".\Damage Meter\SWDamagePlayer.h"

#define HISTORY SWDamageMeterHistory::getInstance()
#define HISTORY_SIZE 10

typedef struct _HISTORYINFO {
public:
	vector<SWDamagePlayer*> _history;
	UINT32 _worldID;
	SYSTEMTIME _saveTime;
	FLOAT _time;

	VOID Setup(vector<SWDamagePlayer*>& history, UINT32 worldID, FLOAT time);
	VOID Clear();
}HISTORY_INFO;

class SWDamageMeterHistory : public Singleton<SWDamageMeterHistory> {
private:
	HISTORY_INFO _historys[HISTORY_SIZE];
	UINT _curIndex;
	
	VOID ClearHistory(INT index);

public:
	SWDamageMeterHistory() : _curIndex(0) {}
	~SWDamageMeterHistory();

	VOID push_back(vector<SWDamagePlayer*>& playerInfo);
	SIZE_T size();
	const HISTORY_INFO& operator[](INT index);
	SIZE_T getCurrentIndex();
};