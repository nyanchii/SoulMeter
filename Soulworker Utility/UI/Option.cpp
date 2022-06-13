#include "pch.h"
#include ".\Language\Region.h"
#include ".\UI\Option.h"
#include ".\UI\HotKey.h"
#include ".\UI\PlayerTable.h"
#include ".\UI\UiWindow.h"
#include ".\Damage Meter\Damage Meter.h"
#include ".\Buff Meter\Buff Meter.h"

UiOption::UiOption()  :  _open(0), _framerate(1), _windowBorderSize(1), _fontScale(1), _columnFontScale(1), _tableFontScale(1), _is1K(0), _is1M(0), _isSoloMode(0), _hideName(0), _isTopMost(true), _cellPadding(0, 0), _windowWidth(800), _refreshTime((FLOAT)0.3) {
	_jobBasicColor[0] = ImVec4(ImGui::ColorConvertU32ToFloat4(ImColor(153, 153, 153, 255)));	// Unknown
	_jobBasicColor[1] = ImVec4(ImGui::ColorConvertU32ToFloat4(ImColor(247, 142, 59, 255)));	// haru
	_jobBasicColor[2] = ImVec4(ImGui::ColorConvertU32ToFloat4(ImColor(59, 147, 247, 255)));	// owin
	_jobBasicColor[3] = ImVec4(ImGui::ColorConvertU32ToFloat4(ImColor(247, 59, 156, 255)));	// lily
	_jobBasicColor[4] = ImVec4(ImGui::ColorConvertU32ToFloat4(ImColor(247, 190, 59, 255)));	// kin
	_jobBasicColor[5] = ImVec4(ImGui::ColorConvertU32ToFloat4(ImColor(161, 59, 247, 255)));	// stella
	_jobBasicColor[6] = ImVec4(ImGui::ColorConvertU32ToFloat4(ImColor(223, 1, 1, 255)));	// iris
	_jobBasicColor[7] = ImVec4(ImGui::ColorConvertU32ToFloat4(ImColor(138, 2, 4, 255)));		// chii
	_jobBasicColor[8] = ImVec4(ImGui::ColorConvertU32ToFloat4(ImColor(118, 206, 158, 255)));	// eph
	_jobBasicColor[9] = ImVec4(ImGui::ColorConvertU32ToFloat4(ImColor(128, 128, 64, 255)));	// nabi

	for (int i = 0; i < 10; i++)
		_jobColor[i] = _jobBasicColor[i];
}

UiOption::~UiOption() {
	
}

BOOL UiOption::ShowFontSelector() {

	ImFont* font_current = ImGui::GetFont();

	ImGui::Text(STR_OPTION_FONTSCALE_DESC);
	ImGui::DragFloat(STR_OPTION_FONTSCALE, &_fontScale, 0.005f, 0.3f, 2.0f, "%.1f");

	font_current->Scale = _fontScale;

	if (ImGui::Checkbox(STR_OPTION_UNIT_1K, (bool*)&_is1K)) {
		if (_is1M)
			_is1M = FALSE;
	}

	if (ImGui::Checkbox(STR_OPTION_UNIT_1M, (bool*)&_is1M)) {
		if (_is1K)
			_is1K = FALSE;
	}
	ImGui::Checkbox(STR_OPTION_SOLO_MODE, (bool*)&_isSoloMode);
	ImGui::Checkbox(STR_OPTION_HIDE_NAME, (bool*)&_hideName);

	return TRUE;
}

BOOL UiOption::ShowTableOption() {

	ImGuiStyle& style = ImGui::GetStyle();

	ImGui::SliderFloat(STR_OPTION_WINDOW_BORDER_SIZE, &_windowBorderSize, 0.0f, 1.0f, "%.0f");
	style.WindowBorderSize = _windowBorderSize;
	ImGui::SliderFloat2(STR_OPTION_CELL_PADDING, (float*)&_cellPadding, 0.0f, 20.0f, "%.0f");
	style.CellPadding = _cellPadding;
	ImGui::DragFloat(STR_OPTION_COLUMN_FONT_SCALE, &_columnFontScale, 0.005f, 0.3f, 2.0f, "%.1f");
	ImGui::DragFloat(STR_OPTION_TABLE_FONT_SCALE, &_tableFontScale, 0.005f, 0.3f, 2.0f, "%.1f");
	ImGui::Separator();
	ImGui::DragFloat(STR_OPTION_TABLE_REFRESH_TIME, &_refreshTime, 0.005f, 0.1f, 1.0f, "%.1f");
	ImGui::Separator();
	ImGui::ColorEdit4("##ColorText", (FLOAT*)&_textColor, ImGuiColorEditFlags_None); 
	ImGui::SameLine(); 	ImGui::Text(ImGui::GetStyleColorName(0));
	style.Colors[0] = _textColor;
	ImGui::ColorEdit4("##ColorBgr", (FLOAT*)&_windowBg, ImGuiColorEditFlags_None);
	ImGui::SameLine();	ImGui::Text(ImGui::GetStyleColorName(2));
	style.Colors[2] = _windowBg;
	ImGui::ColorEdit4("##ColorOutline", (FLOAT*)&_outlineColor, ImGuiColorEditFlags_None);
	ImGui::SameLine();	ImGui::Text(STR_OPTION_TEXT_OUTLINE_COLOR);
	ImGui::ColorEdit4("##ColorActiveColor", (FLOAT*)&_activeColor[1], ImGuiColorEditFlags_None);
	ImGui::SameLine();	ImGui::Text(STR_OPTION_ACTIVE_COLOR);
	ImGui::ColorEdit4("##ColorInActiveColor", (FLOAT*)&_activeColor[0], ImGuiColorEditFlags_None);
	ImGui::SameLine();	ImGui::Text(STR_OPTION_INACTIVE_COLOR);

	const char job[10][32] = { {STR_CHAR_UNKNOWN}, {STR_CHAR_HARU}, {STR_CHAR_ERWIN}, {STR_CHAR_LILY}, {STR_CHAR_JIN}, {STR_CHAR_STELLA},{STR_CHAR_IRIS}, {STR_CHAR_CHII}, {STR_CHAR_Ephnel}, {STR_CHAR_NABI} };

	for (int i = 0; i < 10; i++) {
		ImGui::PushID(i);
		ImGui::ColorEdit4("##Color", (FLOAT*)&_jobColor[i], ImGuiColorEditFlags_None);
		ImGui::SameLine();	ImGui::Text(job[i]);

		if (memcmp(&_jobColor[i], &_jobBasicColor[i], sizeof(ImVec4)) != 0) {
			ImGui::SameLine(0.0f, style.ItemInnerSpacing.x); 
			if (ImGui::Button(STR_OPTION_RESTORE_DEFAULT_COLOR)) {
				_jobColor[i] = _jobBasicColor[i];
			}
		}

		ImGui::PopID();
	}

	return TRUE;
}

BOOL UiOption::ShowHotkeySetting() {

	const char* text =
		STR_OPTION_HOTKEY_DESC_1
		STR_OPTION_HOTKEY_DESC_2
		STR_OPTION_HOTKEY_DESC_3
		STR_OPTION_HOTKEY_DESC_4
		STR_OPTION_HOTKEY_DESC_5;

	ImGui::Text(text);

	return TRUE;
}

VOID UiOption::Helper() {

	static UINT32 helper = 1;
	CHAR name[128] = { 0 };

	UINT monster[4] = { 604, 605, 10000206, 10194613 };
	UINT skill[4] = { 72000233, 72000331, 72000433, 72000638 };
	UINT buff[4] = { 10001, 10111, 10222, 10333 };

	if (DAMAGEMETER.GetWorldID() == 0) {
		DAMAGEMETER.SetWorldID(20011);
	}

	DAMAGEMETER.InsertDB(0, monster[0]);
	DAMAGEMETER.InsertDB(1, monster[1]);
	DAMAGEMETER.InsertDB(2, monster[2]);
	DAMAGEMETER.InsertDB(3, monster[3]);
	DAMAGEMETER.InsertDB(4, monster[0]);
	DAMAGEMETER.InsertDB(5, monster[1]);
	DAMAGEMETER.InsertDB(6, monster[2]);
	DAMAGEMETER.InsertDB(7, monster[3]);

	for (INT i = 0; i < 4; i++) {
		sprintf_s(name, 128, "%s %d", STR_OPTION_TEST_VALUE_PLAYER, helper);
		
		UINT32 id;
		if (helper == 3) {
			id = DAMAGEMETER.GetMyID();
		}
		else {
			id = helper;
			DAMAGEMETER.InsertPlayerMetadata(id, name, helper % 10);
		}

		//DAMAGEMETER.InsertPlayerMetadata(id, name, helper % 10);
		DAMAGEMETER.AddDamage(id, helper * 10000, helper * 5000, 4, helper * 2, i % 4, skill[i % 4]);
		DAMAGEMETER.AddDamage(id, helper * 20000, helper * 5000, 4, helper * 3, (i + 1) % 4, skill[(i + 1) % 4]);
		DAMAGEMETER.AddDamage(id, helper * 30000, helper * 5000, 4, helper * 4, (i + 2) % 4, skill[(i + 2) % 4]);
		DAMAGEMETER.AddDamage(id, helper * 40000, helper * 5000, 4, helper * 5, (i + 3) % 4, skill[(i + 3) % 4]);
		DAMAGEMETER.AddDamage(id, helper * 20000, helper * 5000, 4, helper * 3, (i + 4) % 4, skill[(i + 1) % 4]);
		DAMAGEMETER.AddDamage(id, helper * 30000, helper * 5000, 4, helper * 4, (i + 5) % 4, skill[(i + 2) % 4]);
		DAMAGEMETER.AddDamage(id, helper * 40000, helper * 5000, 4, helper * 5, (i + 6) % 4, skill[(i + 3) % 4]);
		DAMAGEMETER.AddDamage(id, helper * 40000, helper * 5000, 4, helper * 5, (i + 7) % 4, skill[(i + 3) % 4]);

		BUFFMETER.AddBuff(id, buff[id % 4], 1 + id);
		helper++;
	}
}

VOID UiOption::OpenOption() {

	_open = TRUE;

	if (DAMAGEMETER.size() < 1) {
		Helper();
		PLAYERTABLE.ResizeTalbe();
	}

	ImGui::Begin(STR_OPTION_WINDOWS_NAME, 0, ImGuiWindowFlags_None);

		if (ImGui::Button(STR_OPTION_ADD_TEST_VALUE)) {
			Helper();
		}

		ImGui::SameLine(); 		
		
		if (ImGui::Button(STR_OPTION_SAVE_AND_EXIT)) {
			SaveOption();
			if (DAMAGEMETER.GetWorldID() == 20011) {
				DAMAGEMETER.SetWorldID(0);
			}
			_open = FALSE;
		}

#ifdef _DEBUG
		if (ImGui::Button("START TIMER")) {
			DAMAGEMETER.Start();
		}
		ImGui::SameLine();
		if (ImGui::Button("STOP TIMER")) {
			DAMAGEMETER.Suspend();
		}
#endif

		ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.2f);
		ShowFontSelector();
		ImGui::PopItemWidth();

		if (ImGui::BeginTabBar("##tabs")) {
			if (ImGui::BeginTabItem(STR_OPTION_TAB_TABLE_SETTING)) {
				ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.5f);
				ShowTableOption();
				ImGui::PopItemWidth();
				ImGui::EndTabItem();
			}

			if (ImGui::BeginTabItem(STR_OPTION_TAB_HOTKEY_SETTING)) {
				ShowHotkeySetting();
				ImGui::EndTabItem();
			}

			ImGui::EndTabBar();
		}

		ImGui::End();
}

VOID UiOption::Init() {

	if (!GetOption()) {
		SetBasicOption();
	}
}

BOOL UiOption::GetOption() {

	tinyxml2::XMLDocument doc;
	ImGuiStyle& style = ImGui::GetStyle();

	if (doc.LoadFile(OPTION_FILE_NAME))
		return FALSE;

	tinyxml2::XMLNode* node = doc.FirstChildElement("SDM");

	if (!node)
		return FALSE;

	// Option
	tinyxml2::XMLElement* ele = node->FirstChildElement("Option");

	if (!ele)
		return FALSE;

	auto attr = ele->FindAttribute("GlobalScale");

	if (attr == nullptr)
		return FALSE;

	attr->QueryFloatValue(&_fontScale);

#if DEBUG_READ_XML == 1
	Log::WriteLog(const_cast<LPTSTR>(_T("Read FontScale = %.1f")), _fontScale);
#endif

	attr = ele->FindAttribute("TableScale");

	if (attr == nullptr)
		return FALSE;

	attr->QueryFloatValue(&_tableFontScale);

#if DEBUG_READ_XML == 1
	Log::WriteLog(const_cast<LPTSTR>(_T("Read TableFontScale = %.1f")), _tableFontScale);
#endif

	attr = ele->FindAttribute("ColumnScale");

	if (attr == nullptr)
		return FALSE;

	attr->QueryFloatValue(&_columnFontScale);

#if DEBUG_READ_XML == 1
	Log::WriteLog(const_cast<LPTSTR>(_T("Read ColumnFontScale = %.1f")), _columnFontScale);
#endif

	attr = ele->FindAttribute("K");

	if (attr == nullptr)
		return FALSE;

	attr->QueryIntValue(&_is1K);

#if DEBUG_READ_XML == 1
	Log::WriteLog(const_cast<LPTSTR>(_T("Read 1K = %d")), _is1K);
#endif

	attr = ele->FindAttribute("M");

	if (attr == nullptr)
		return FALSE;

	attr->QueryIntValue(&_is1M);

	attr = ele->FindAttribute("IsSoloMode");
	if (attr == nullptr)
		return FALSE;
	attr->QueryIntValue(&_isSoloMode);

	attr = ele->FindAttribute("DoHideName");
	if (attr == nullptr)
		return FALSE;
	attr->QueryIntValue(&_hideName);

	attr = ele->FindAttribute("IsTopMost");
	if (attr == nullptr)
		return FALSE;
	attr->QueryIntValue(&_isTopMost);

#if DEBUG_READ_XML == 1
	Log::WriteLog(const_cast<LPTSTR>(_T("Read 1M = %d")), _is1M);
#endif

	attr = ele->FindAttribute("CellPaddingX");

	if (attr == nullptr)
		return FALSE;

	attr->QueryFloatValue(&_cellPadding.x);
	style.CellPadding.x = _cellPadding.x;

#if DEBUG_READ_XML == 1
	Log::WriteLog(const_cast<LPTSTR>(_T("Read CellPadding X = %f")), _cellPadding.x);
#endif

	attr = ele->FindAttribute("CellPaddingY");

	if (attr == nullptr)
		return FALSE;

	attr->QueryFloatValue(&_cellPadding.y);
	style.CellPadding.y = _cellPadding.y;

#if DEBUG_READ_XML == 1
	Log::WriteLog(const_cast<LPTSTR>(_T("Read CellPadding Y = %f")), _cellPadding.y);
#endif

	attr = ele->FindAttribute("BorderSize");

	if (attr == nullptr)
		return FALSE;

	attr->QueryFloatValue(&_windowBorderSize);
	style.WindowBorderSize = _windowBorderSize;

#if DEBUG_READ_XML == 1
	Log::WriteLog(const_cast<LPTSTR>(_T("Read WindowBorderSize = %f")), _windowBorderSize);
#endif

	attr = ele->FindAttribute("WindowWidth");

	if (attr == nullptr)
		return FALSE;

	attr->QueryFloatValue(&_windowWidth);


#if DEBUG_READ_XML == 1
	Log::WriteLog(const_cast<LPTSTR>(_T("Read WindowWidth = %f")), _windowWidth);
#endif

	attr = ele->FindAttribute("RefreshTime");

	if (attr == nullptr)
		return FALSE;

	attr->QueryFloatValue(&_refreshTime);

#if DEBUG_READ_XML == 1
		Log::WriteLog(const_cast<LPTSTR>(_T("Read RefreshTime = %f")), _refreshTime);
#endif
		attr = ele->FindAttribute("WinPosX");

		if (attr == nullptr)
			return FALSE;

		FLOAT winX, winY;

		attr->QueryFloatValue(&winX);

		attr = ele->FindAttribute("WinPosY");

		if (attr == nullptr)
			return FALSE;

		attr->QueryFloatValue(&winY);

		//SetWindowPos(UIWINDOW.GetHWND(), HWND_NOTOPMOST, winX, winY, 0, 0, SWP_NOSIZE);
		SetWindowPos(UIWINDOW.GetHWND(), HWND_TOPMOST, static_cast<INT>(winX), static_cast<INT>(winY), 0, 0, SWP_NOSIZE);

#if DEBUG_READ_XML == 1
		Log::WriteLog(const_cast<LPTSTR>(_T("Read WinPos(X,Y) = (%f, %f)")), winX, winY);
#endif

	// Text Color
	ele = ele->NextSiblingElement("TextColor");

	if (!ele)
		return FALSE;

	const char name[4][8] = { {"r"}, {"g"}, {"b"}, {"a"} };

	for (int i = 0; i < 4; i++) {
		attr = ele->FindAttribute(name[i]);

		if (attr == nullptr)
			return FALSE;

		switch (i) {
		case 0:
			attr->QueryFloatValue(&_textColor.x);
			break;
		case 1:
			attr->QueryFloatValue(&_textColor.y);
			break;
		case 2:
			attr->QueryFloatValue(&_textColor.z);
			break;
		case 3:
			attr->QueryFloatValue(&_textColor.w);
			break;
		}
	}

	style.Colors[0] = _textColor;

#if DEBUG_READ_XML == 1
	Log::WriteLog(const_cast<LPTSTR>(_T("Read TextColor = %.1f, %.1f, %.1f, %.1f")), _textColor.x, _textColor.y, _textColor.z, _textColor.w);
#endif

	// WindowBg Color
	ele = ele->NextSiblingElement("WindowBgColor");

	if (!ele)
		return FALSE;

	for (int i = 0; i < 4; i++) {
		attr = ele->FindAttribute(name[i]);

		if (attr == nullptr)
			return FALSE;

		switch (i) {
		case 0:
			attr->QueryFloatValue(&_windowBg.x);
			break;
		case 1:
			attr->QueryFloatValue(&_windowBg.y);
			break;
		case 2:
			attr->QueryFloatValue(&_windowBg.z);
			break;
		case 3:
			attr->QueryFloatValue(&_windowBg.w);
			break;
		}
	}

	style.Colors[2] = _windowBg;

#if DEBUG_READ_XML == 1
	Log::WriteLog(const_cast<LPTSTR>(_T("Read WindowBgColor = %.1f, %.1f, %.1f, %.1f")), _windowBg.x, _windowBg.y, _windowBg.z, _windowBg.w);
#endif

	// Outline Color
	ele = ele->NextSiblingElement("OutlineColor");
		
	if (!ele)
		return FALSE;

	for (int i = 0; i < 4; i++) {
		attr = ele->FindAttribute(name[i]);

		if (attr == nullptr)
			return FALSE;
		
		switch (i) {
		case 0:
			attr->QueryFloatValue(&_outlineColor.x);
			break;
		case 1:
			attr->QueryFloatValue(&_outlineColor.y);
			break;
		case 2:
			attr->QueryFloatValue(&_outlineColor.z);
			break;
		case 3:
			attr->QueryFloatValue(&_outlineColor.w);
			break;
		}
	}

#if DEBUG_READ_XML == 1
	Log::WriteLog(const_cast<LPTSTR>(_T("Read OutlineColor = %.1f, %.1f, %.1f, %.1f")), _outlineColor.x, _outlineColor.y, _outlineColor.z, _outlineColor.w);
#endif

	// ActiveColor
	ele = ele->NextSiblingElement("ActiveColor");

	if (!ele)
		return FALSE;

	for (int i = 0; i < 4; i++) {
		attr = ele->FindAttribute(name[i]);

		if (attr == nullptr)
			return FALSE;

		switch (i) {
		case 0:
			attr->QueryFloatValue(&_activeColor[1].x);
			break;
		case 1:
			attr->QueryFloatValue(&_activeColor[1].y);
			break;
		case 2:
			attr->QueryFloatValue(&_activeColor[1].z);
			break;
		case 3:
			attr->QueryFloatValue(&_activeColor[1].w);
			break;
		}
	}

#if DEBUG_READ_XML == 1
	Log::WriteLog(const_cast<LPTSTR>(_T("Read ActiveColor = %.1f, %.1f, %.1f, %.1f")), _activeColor[1].x, _activeColor[1].y, _activeColor[1].z, _activeColor[1].w);
#endif

	ele = ele->NextSiblingElement("InActiveColor");

	if (!ele)
		return FALSE;

	for (int i = 0; i < 4; i++) {
		attr = ele->FindAttribute(name[i]);

		if (attr == nullptr)
			return FALSE;

		switch (i) {
		case 0:
			attr->QueryFloatValue(&_activeColor[0].x);
			break;
		case 1:
			attr->QueryFloatValue(&_activeColor[0].y);
			break;
		case 2:
			attr->QueryFloatValue(&_activeColor[0].z);
			break;
		case 3:
			attr->QueryFloatValue(&_activeColor[0].w);
			break;
		}
	}

#if DEBUG_READ_XML == 1
	Log::WriteLog(const_cast<LPTSTR>(_T("Read InActiveColor = %.1f, %.1f, %.1f, %.1f")), _activeColor[0].x, _activeColor[0].y, _activeColor[0].z, _activeColor[0].w);
#endif

	for (int i = 0; i < 10; i++) {
		char temp[32] = { 0 };
		sprintf_s(temp, 32, "JobColor%d", i);
		ele = ele->NextSiblingElement(temp);

		if (!ele)
			return FALSE;

		for (int j = 0; j < 4; j++) {
			attr = ele->FindAttribute(name[j]);

			if (attr == nullptr)
				return FALSE;

			switch (j) {
			case 0:
				attr->QueryFloatValue(&_jobColor[i].x);
				break;
			case 1:
				attr->QueryFloatValue(&_jobColor[i].y);
				break;
			case 2:
				attr->QueryFloatValue(&_jobColor[i].z);
				break;
			case 3:
				attr->QueryFloatValue(&_jobColor[i].w);
				break;
			}
		}

#if DEBUG_READ_XML == 1
		Log::WriteLog(const_cast<LPTSTR>(_T("Read JobColor%d = %.1f, %.1f, %.1f, %.1f")), i, _jobColor[i].x, _jobColor[i].y, _jobColor[i].z, _jobColor[i].w);
#endif
	}
	
	INT hotkeyID = 0;

	do {

		INT key[3] = { -1 };
		CHAR name2[AUTO_HOTKEY_NAME_LEN] = { 0 };
		sprintf_s(name2, AUTO_HOTKEY_NAME_LEN, "HOTKEY%d", hotkeyID++);

		ele = ele->NextSiblingElement(name2);

		if (ele == nullptr)
			break;

		for (INT i = 0; i < 3; i++) {

			char temp[12] = { 0 };
			sprintf_s(temp, 12, "key%d", i + 1);

			attr = ele->FindAttribute(temp);

			if (attr == nullptr)
				break;

			attr->QueryIntValue(&key[i]);
		}

		if (ele->GetText() != nullptr) {
			strcpy_s(name2, ele->GetText());
		}

#if DEBUG_READ_XML == 1
		Log::WriteLog(const_cast<LPTSTR>(_T("Read Hotkey %s, key1 = %d, key2 = %d, key3 = %d")), name2, key[0], key[1], key[2]);
#endif
		
		if (strcmp(name2, u8"Toogle") == 0)
			HOTKEY.InsertHotkeyToogle(key[0], key[1], key[2]);
		else if (strcmp(name2, u8"Clear") == 0)
			HOTKEY.InsertHotkeyStop(key[0], key[1], key[2]);
		
	} while (TRUE);

	return TRUE;
}

BOOL UiOption::SaveOption() {
	tinyxml2::XMLDocument doc;

	tinyxml2::XMLDeclaration* dec = doc.NewDeclaration();
	doc.LinkEndChild(dec);

	tinyxml2::XMLElement* root = doc.NewElement("SDM");
	doc.LinkEndChild(root);

	tinyxml2::XMLElement* option = doc.NewElement("Option");
	root->LinkEndChild(option);

	option->SetAttribute("IsTopMost", _isTopMost);

	option->SetAttribute("GlobalScale", _fontScale);
	option->SetAttribute("TableScale", _tableFontScale);
	option->SetAttribute("ColumnScale", _columnFontScale);
	option->SetAttribute("K", _is1K);
	option->SetAttribute("M", _is1M);
	option->SetAttribute("IsSoloMode", _isSoloMode);
	option->SetAttribute("DoHideName", _hideName);
	option->SetAttribute("CellPaddingX", _cellPadding.x);
	option->SetAttribute("CellPaddingY", _cellPadding.y);
	option->SetAttribute("BorderSize", _windowBorderSize);
	option->SetAttribute("WindowWidth", _windowWidth);
	option->SetAttribute("RefreshTime", _refreshTime);


	RECT rect;
	GetWindowRect(UIWINDOW.GetHWND(), &rect);
	option->SetAttribute("WinPosX", (FLOAT)rect.left);
	option->SetAttribute("WinPosY", (FLOAT)rect.top);

	tinyxml2::XMLElement* text_color = doc.NewElement("TextColor");
	root->LinkEndChild(text_color);
	text_color->SetAttribute("r", _textColor.x);
	text_color->SetAttribute("g", _textColor.y);
	text_color->SetAttribute("b", _textColor.z);
	text_color->SetAttribute("a", _textColor.w);

	tinyxml2::XMLElement* windowbg_color = doc.NewElement("WindowBgColor");
	root->LinkEndChild(windowbg_color);
	windowbg_color->SetAttribute("r", _windowBg.x);
	windowbg_color->SetAttribute("g", _windowBg.y);
	windowbg_color->SetAttribute("b", _windowBg.z);
	windowbg_color->SetAttribute("a", _windowBg.w);
	
	tinyxml2::XMLElement* outline_color = doc.NewElement("OutlineColor");
	root->LinkEndChild(outline_color);
	outline_color->SetAttribute("r", _outlineColor.x);
	outline_color->SetAttribute("g", _outlineColor.y);
	outline_color->SetAttribute("b", _outlineColor.z);
	outline_color->SetAttribute("a", _outlineColor.w);

	tinyxml2::XMLElement* active_color = doc.NewElement("ActiveColor");
	root->LinkEndChild(active_color);
	active_color->SetAttribute("r", _activeColor[1].x);
	active_color->SetAttribute("g", _activeColor[1].y);
	active_color->SetAttribute("b", _activeColor[1].z);
	active_color->SetAttribute("a", _activeColor[1].w);

	tinyxml2::XMLElement* inactive_color = doc.NewElement("InActiveColor");
	root->LinkEndChild(inactive_color);
	inactive_color->SetAttribute("r", _activeColor[0].x);
	inactive_color->SetAttribute("g", _activeColor[0].y);
	inactive_color->SetAttribute("b", _activeColor[0].z);
	inactive_color->SetAttribute("a", _activeColor[0].w);

	for (int i = 0; i < 10; i++) {

		char buffer[32] = { 0 };
		sprintf_s(buffer, 32, "JobColor%d", i);
		tinyxml2::XMLElement* job_color = doc.NewElement(buffer);
		root->LinkEndChild(job_color);

		job_color->SetAttribute("r", _jobColor[i].x);
		job_color->SetAttribute("g", _jobColor[i].y);
		job_color->SetAttribute("b", _jobColor[i].z);
		job_color->SetAttribute("a", _jobColor[i].w);
	}
	
	INT hotkeyid = 0;

	for (auto itr = HOTKEY.begin(); itr != HOTKEY.end(); itr++) {

		char buffer[32] = { 0 };
		sprintf_s(buffer, 32, "HOTKEY%d", hotkeyid++);
		tinyxml2::XMLElement* hotkey = doc.NewElement(buffer);
		root->LinkEndChild(hotkey);

		hotkey->SetAttribute("key1", (*itr)->GetKey()[0]);
		hotkey->SetAttribute("key2", (*itr)->GetKey()[1]);
		hotkey->SetAttribute("key3", (*itr)->GetKey()[2]);
		hotkey->SetText((*itr)->GetName());
	}

	doc.SaveFile(OPTION_FILE_NAME);

	return TRUE;
}

BOOL UiOption::SetBasicOption() {

	ImGui::StyleColorsDark();

	ImGuiStyle& style = ImGui::GetStyle();
	ImGuiIO& io = ImGui::GetIO();

	_outlineColor = ImVec4(ImGui::ColorConvertU32ToFloat4(ImColor(0, 0, 0, 255)));
	_activeColor[0] = style.Colors[10];
	_activeColor[1] = style.Colors[11];
	_textColor = style.Colors[0];
	_windowBg = style.Colors[2];

	HOTKEY.InsertHotkeyToogle(DIK_LCONTROL, DIK_END, -1);
	HOTKEY.InsertHotkeyStop(DIK_LCONTROL, DIK_DELETE, -1);

	Helper();
	PLAYERTABLE.ResizeTalbe();
	_open = TRUE;

	return TRUE;
}

BOOL UiOption::ToggleTopMost() {

	_isTopMost = _isTopMost ? false : true;
	
	return SaveOption();
}

const ImU32 UiOption::GetJobColor(UINT index) {

	if (index < 0 || index > 9)
		return ImGui::ColorConvertFloat4ToU32(_jobColor[0]);

	return ImGui::ColorConvertFloat4ToU32(_jobColor[index]);
}

const ImU32 UiOption::GetOutlineColor() {
	return ImGui::ColorConvertFloat4ToU32(_outlineColor);
}

const FLOAT& UiOption::GetFontScale() {
	return _fontScale;
}

const FLOAT& UiOption::GetColumnFontScale() {
	return _columnFontScale;
}

const FLOAT& UiOption::GetTableFontScale() {
	return _tableFontScale;
}

const ImVec4& UiOption::GetActiveColor() {
	return _activeColor[1];
}

const ImVec4& UiOption::GetInActiveColor() {
	return _activeColor[0];
}

const BOOL& UiOption::is1K() {
	return _is1K;
}

const BOOL& UiOption::is1M() {
	return _is1M;
}

const BOOL& UiOption::isSoloMode(){
	return _isSoloMode;
}

const BOOL& UiOption::doHideName()
{
	return _hideName;
}

const BOOL& UiOption::isTopMost()
{
	return _isTopMost;
}

VOID UiOption::Update() {

	ImFont* font = ImGui::GetFont();
	font->Scale = _fontScale;

	if (_open)
		OpenOption();

#if DEBUG_COLUMN_WIDTH == 1
	for (INT i = 0; i < 8; i++)
		Log::WriteLog(const_cast<LPTSTR>(_T("[DEBUG] [Column Width] [%d] [%f]")), i, UIOPTION[i]);
#endif
}

const BOOL& UiOption::isOption() {
	return _open;
}

const FLOAT& UiOption::GetFramerate() {
	return _framerate;
}

VOID UiOption::SetFramerate(FLOAT i) {

	if (i < 0)
		i = 0;
	else if (i > 4)
		i = 4;

	_framerate = i;
}

const ImVec4& UiOption::GetWindowBGColor() {
	return _windowBg;
}

const FLOAT& UiOption::GetWindowWidth() {
	return _windowWidth;
}

VOID UiOption::SetWindowWidth(const FLOAT& width) {
	_windowWidth = width;
}

const FLOAT& UiOption::GetRefreshTime() {
	return _refreshTime;
}