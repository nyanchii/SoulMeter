#pragma once
#include ".\UI\SpecificInfomation.h"

typedef struct _SELECTED_PLAYER {
	
	UINT32 _playerID;
	BOOL _isSelected;
	SpecificInformation* _specificInfo;

	_SELECTED_PLAYER(UINT32 playerID, BOOL isSelected, SpecificInformation* specificInfo) : _playerID(playerID), _isSelected(isSelected), _specificInfo(specificInfo) { }
}SELECTED_PLAYER;

#define PLAYERTABLE PlayerTable::getInstance()

class PlayerTable : public Singleton<PlayerTable> {
private:
	vector<SELECTED_PLAYER*> _selectInfo;

	BOOL ToggleSelectInfo(UINT32 id);
	VOID ShowSelectedTable();
	VOID BeginPopupMenu();

	VOID SetWindowSize();
	VOID SetMainWindowSize();
	VOID StoreWindowWidth();
	VOID SetupFontScale();

	VOID DrawBar(FLOAT window_Width, FLOAT percent, ImU32 color);
	VOID SetupTable();
	VOID UpdateTable(FLOAT windowWidth);

	FLOAT _globalFontScale;
	FLOAT _columnFontScale;
	FLOAT _tableFontScale;

	FLOAT _curWindowSize;

	BOOL _tableResize;

	FLOAT _tableTime;
	FLOAT _accumulatedTime;

public:
	PlayerTable();
	~PlayerTable();

	VOID Update();
	VOID ClearTable();
	VOID ResizeTalbe();

	ULONG64 _lastSendTimestamp = 0;
	ULONG64 _ping = 0;

	FLOAT GetTableTime()
	{
		return _tableTime;
	}
};