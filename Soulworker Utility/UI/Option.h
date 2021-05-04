#pragma once

#define UIOPTION UiOption::getInstance()

#define OPTION_FILE_NAME "option.xml"

#define DEBUG_READ_XML 0
#define DEBUG_COLUMN_WIDTH 0

class UiOption : public Singleton<UiOption> {
private:
	ImVec4 _jobColor[10];
	ImVec4 _jobBasicColor[10];
	ImVec4 _activeColor[2];
	ImVec4 _outlineColor;

	INT _styleIndex;
	BOOL ShowStyleSelector(const CHAR* label);

	FLOAT _fontScale;
	BOOL ShowFontSelector();

	FLOAT _columnFontScale;
	FLOAT _tableFontScale;
	BOOL _is1K;
	BOOL _is1M;
	BOOL _autoSize;
	FLOAT _columnWidth[8];
	BOOL ShowTableOption();

	BOOL ShowHotkeySetting();
	BOOL ShowAbout();

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

	const ImU32& GetJobColor(UINT index);
	const ImU32& GetOutlineColor();
	const FLOAT& GetFontScale();
	const FLOAT& GetColumnFontScale();
	const FLOAT& GetTableFontScale();
	const ImVec4& GetActiveColor();
	const ImVec4& GetInActiveColor();
	const BOOL& is1K();
	const BOOL& is1M();
	const BOOL& isAutoSize();

	const FLOAT& GetColumnNameSize();
	const FLOAT& GetColumnDpsSize();
	const FLOAT& GetColumnDperSize();
	const FLOAT& GetColumnDamageSize();
	const FLOAT& GetColumnHitSize();
	const FLOAT& GetColumnCritSize();
	const FLOAT& GetColumnHitsSize();
	const FLOAT& GetColumnMaxcSize();

	VOID SetColumnNameSize(FLOAT size);
	VOID SetColumnDpsSize(FLOAT size);
	VOID SetColumnDperSize(FLOAT size);
	VOID SetColumnDamageSize(FLOAT size);
	VOID SetColumnHitSize(FLOAT size);
	VOID SetColumnCritSize(FLOAT size);
	VOID SetColumnHitsSize(FLOAT size);
	VOID SetColumnMaxcSize(FLOAT size);

	FLOAT& operator[](INT index);
};