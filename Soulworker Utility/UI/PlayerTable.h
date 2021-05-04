#pragma once
#include ".\UI\SpecificInfomation.h"

extern ImGuiContext* GImGui;

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
	VOID StoreColumnWidth(INT columnSize);
	VOID SetupFontScale();

	VOID DrawBar(FLOAT window_Width, FLOAT percent, ImU32 color);
	VOID SetupTable();
	VOID UpdateTable(FLOAT windowWidth);
	
	BOOL _tableResize;

	FLOAT _globalFontScale;
	FLOAT _columnFontScale;
	FLOAT _tableFontScale;

	FLOAT _curWindowSize;

	SIZE_T _selectedHistory;

public:
	PlayerTable();
	~PlayerTable();

	VOID Update();
	VOID ClearTable();
	VOID ResizeTalbe();
};