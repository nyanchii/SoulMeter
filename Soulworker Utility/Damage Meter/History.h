#pragma once
#include ".\Damage Meter\SWDamagePlayer.h"
#include ".\Buff Meter\Buff Meter.h"
#include ".\UI\PlotWindow.h"

#define HISTORY SWDamageMeterHistory::getInstance()
#define HISTORY_SIZE 10

typedef struct _HISTORYINFO {
public:
	vector<SWDamagePlayer*> _history;
	vector<PLAYERBUF*> _buffHistory;
	PlotInfo* _plotHistory;
	UINT32 _worldID;
	SYSTEMTIME _saveTime;
	ULONG64 _time;

	VOID Setup(vector<SWDamagePlayer*>& history, vector<PLAYERBUF*>& buffHistory, PlotInfo* plotHistory, UINT32 worldID, ULONG64 time);
	VOID Clear();
}HISTORY_INFO;

class SWDamageMeterHistory : public Singleton<SWDamageMeterHistory> {
private:
	HISTORY_INFO _historys[HISTORY_SIZE];
	INT _curIndex = 0;
	
	VOID ClearHistory(INT index);

public:
	SWDamageMeterHistory() : _curIndex(0) {}
	~SWDamageMeterHistory();

	VOID push_back(vector<SWDamagePlayer*>& playerInfo, std::vector<PLAYERBUF*>& playerBuffInfo, PlotInfo* playerPlotInfo);
	SIZE_T size();
	const HISTORY_INFO& operator[](INT index);
	INT GetCurrentIndex()
	{
		return _curIndex;
	}
};