#pragma once

#define UIOPTION UiOption::getInstance()

#define OPTION_FILE_NAME "option.xml"

#ifdef _DEBUG
#define DEBUG_READ_XML 0
#define DEBUG_COLUMN_WIDTH 0
#endif

class UiOption : public Singleton<UiOption> {
private:
	ImVec4 _jobColor[10];
	ImVec4 _jobBasicColor[10];
	ImVec4 _activeColor[2];
	ImVec4 _outlineColor;
	ImVec4 _textColor;
	ImVec4 _windowBg;

	FLOAT _fontScale;
	BOOL ShowFontSelector();

	FLOAT _columnFontScale;
	FLOAT _tableFontScale;
	BOOL _is1K;
	BOOL _is1M;
	BOOL _isSoloMode;
	BOOL _hideName;
	ImVec2 _cellPadding;
	FLOAT _framerate;
	FLOAT _windowBorderSize;
	FLOAT _windowWidth;
	FLOAT _refreshTime;
	BOOL _isTopMost;
	BOOL ShowTableOption();

	BOOL ShowHotkeySetting();

	VOID Helper();

	BOOL GetOption();
	BOOL SaveOption();
	BOOL SetBasicOption();
	

	BOOL _open;

public:
	UiOption();
	~UiOption();

	const BOOL& isOption();
	VOID OpenOption();
	VOID Update();
	VOID Init();

	const ImU32 GetJobColor(UINT index);
	const ImU32 GetOutlineColor();
	const FLOAT& GetFontScale();
	const FLOAT& GetColumnFontScale();
	const FLOAT& GetTableFontScale();
	const ImVec4& GetActiveColor();
	const ImVec4& GetInActiveColor();
	const ImVec4& GetWindowBGColor();
	const BOOL& is1K();
	const BOOL& is1M();
	const BOOL& isSoloMode();
	const BOOL& doHideName();
	const BOOL& isTopMost();

	BOOL ToggleTopMost();

	const FLOAT& GetFramerate();
	VOID SetFramerate(FLOAT i);

	const FLOAT& GetWindowWidth();
	VOID SetWindowWidth(const FLOAT& width);

	const FLOAT& GetRefreshTime();
};