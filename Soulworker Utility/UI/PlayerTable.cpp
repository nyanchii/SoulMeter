#include "pch.h"
#include ".\UI\PlayerTable.h"

#include ".\Language\Region.h"

#include ".\Damage Meter\Damage Meter.h"
#include ".\Damage Meter\History.h"
#include ".\Damage Meter\MySQLite.h"
#include ".\UI\Option.h"
#include ".\UI\UiWindow.h"
#include ".\UI\UtillWindow.h"
#include ".\UI\PlotWindow.h"
#include ".\Soulworker Packet\PacketInfo.h"
#include ".\Soulworker Packet\SWPacketMaker.h"


PlayerTable::PlayerTable() : _tableResize(0), _globalFontScale(0), _columnFontScale(0), _tableFontScale(0), _curWindowSize(0), _tableTime(0), _accumulatedTime(0)
{

}

PlayerTable::~PlayerTable() {
	ClearTable();
}

VOID PlayerTable::ClearTable() {

	for (auto itr = _selectInfo.begin(); itr != _selectInfo.end(); itr++) {
		delete (*itr)->_specificInfo;
	}

	for (auto itr = _selectInfo.begin(); itr != _selectInfo.end(); itr++) {
		delete (*itr);
	}

	_selectInfo.clear();
	_curWindowSize = 0;
}

VOID PlayerTable::SetupFontScale() {

	ImFont* font = ImGui::GetFont();

	_globalFontScale = font->Scale;
	_columnFontScale = _globalFontScale * UIOPTION.GetColumnFontScale();
	_tableFontScale = _globalFontScale * UIOPTION.GetTableFontScale();
}

VOID PlayerTable::ResizeTalbe() {
	_tableResize = TRUE;
}

VOID PlayerTable::Update() {

	DAMAGEMETER.GetLock();
	{
		ImGuiStyle& style = ImGui::GetStyle();

		ImVec2 prevWindowPadding = style.WindowPadding;

		style.WindowPadding.x = 0;
		style.WindowPadding.y = 0;

		ImVec4 prevInActiveColor = style.Colors[10];
		ImVec4 prevActiveColor = style.Colors[11];

		if (DAMAGEMETER.isRun()) {
			style.Colors[10] = UIOPTION.GetActiveColor();
			style.Colors[11] = UIOPTION.GetActiveColor();
		}
		else {
			style.Colors[10] = UIOPTION.GetInActiveColor();
			style.Colors[11] = UIOPTION.GetInActiveColor();
		}

		_accumulatedTime += UIWINDOW.GetDeltaTime();

		if (_accumulatedTime > UIOPTION.GetRefreshTime()) {
			_tableTime = static_cast<FLOAT>(((DOUBLE)DAMAGEMETER.GetTime()) / 1000);
			_accumulatedTime = 0;
		}

		SetupFontScale();

		ImGuiWindowFlags windowFlag = ImGuiWindowFlags_None;
		windowFlag |= (ImGuiWindowFlags_NoNavInputs | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoCollapse);

		if (!UIOPTION.isOption())
			windowFlag = windowFlag | ImGuiWindowFlags_NoResize;

		CHAR title[256] = { 0 };

		sprintf_s(title, 256, "%s - %02d:%02d.%01d [v1.3.0.6_%s] Ping: %lldms  ###DamageMeter", 
			DAMAGEMETER.GetWorldName(), 
			(UINT)DAMAGEMETER.GetTime() / (60 * 1000), (UINT)(DAMAGEMETER.GetTime() / 1000) % 60, (UINT)DAMAGEMETER.GetTime() % 1000 / 100,
			SWPACKETMAKER.GetKeyInfo(),
			_ping
		);

		ImGui::Begin(title, 0, windowFlag);
		{
			if (!UIOPTION.isOption() || _tableResize)
				SetWindowSize();

			if (UIOPTION.isOption())
				StoreWindowWidth();

			SetMainWindowSize();

			BeginPopupMenu();

			ImGui::OutlineText::PushOutlineText(ImGui::IMGUIOUTLINETEXT(UIOPTION.GetOutlineColor(), 1));
			ImGui::TextAlignCenter::SetTextAlignCenter();
			{
				SetupTable();
			}
			ImGui::TextAlignCenter::UnSetTextAlignCenter();
			ImGui::OutlineText::PopOutlineText();
		}
		ImGui::End();

		ShowSelectedTable();

		style.WindowPadding.x = prevWindowPadding.x;
		style.WindowPadding.y = prevWindowPadding.y;

		style.Colors[10] = prevInActiveColor;
		style.Colors[11] = prevActiveColor;
	}
	DAMAGEMETER.FreeLock();
}

VOID PlayerTable::SetWindowSize() {

	_tableResize = FALSE;

	ImGuiStyle& style = ImGui::GetStyle();

	if (ImGui::GetScrollMaxY() > 0)
		_curWindowSize += ImGui::GetScrollMaxY();

	ImGui::SetWindowSize(ImVec2(UIOPTION.GetWindowWidth(), FLOOR(_curWindowSize)));
}

VOID PlayerTable::SetMainWindowSize() {

	auto pos = ImGui::GetWindowPos();
	auto size = ImGui::GetWindowSize();


	if (UIOPTION.isTopMost()) {
		SetWindowPos(UIWINDOW.GetHWND(), HWND_TOPMOST, static_cast<INT>(pos.x), static_cast<INT>(pos.y), static_cast<INT>(size.x + 1), static_cast<INT>(size.y + 1), SWP_NOACTIVATE);
	}
	else {
		SetWindowPos(UIWINDOW.GetHWND(), HWND_NOTOPMOST, static_cast<INT>(pos.x), static_cast<INT>(pos.y), static_cast<INT>(size.x + 1), static_cast<INT>(size.y + 1), SWP_NOACTIVATE);
	}

	//SetWindowPos(UIWINDOW.GetHWND(), HWND_NOTOPMOST, pos.x, pos.y, size.x + 1, size.y + 1, SWP_NOACTIVATE);
	
}

VOID PlayerTable::StoreWindowWidth() {
	UIOPTION.SetWindowWidth(ImGui::GetWindowSize().x);
}

VOID PlayerTable::BeginPopupMenu() {

	if (ImGui::BeginPopupContextItem()) {
		//if (ImGui::MenuItem(STR_MENU_RESUME)) {
		//	//
		//	//DAMAGEMETER.Toggle();
		//}

		if (ImGui::MenuItem(STR_MENU_RESET)) {
			DAMAGEMETER.Clear();
			PLAYERTABLE.ClearTable();
		}

		if (ImGui::MenuItem(STR_MENU_TOPMOST,nullptr,UIOPTION.isTopMost())) {
			UIOPTION.ToggleTopMost();
		}


		bool history_open = false;
		if (HISTORY.GetCurrentIndex() > 0 && !DAMAGEMETER.isRun())
			history_open = true;

		if (ImGui::BeginMenu(STR_MENU_HISTORY, history_open)) {

			for (INT i = 0; i < HISTORY_SIZE; i++) {

				if (HISTORY.GetCurrentIndex() - 1 < i)
					break;

				auto history = HISTORY[i];
				CHAR label[128] = { 0 };

				CHAR mapName[MAX_MAP_LEN] = { 0 };
				SWDB.GetMapName(history._worldID, mapName, MAX_MAP_LEN);

				sprintf_s(label, 128, u8"%s %02d:%02d:%02d(%02d:%02d.%01d)", mapName, 
					history._saveTime.wHour, history._saveTime.wMinute, history._saveTime.wSecond,
					(UINT)history._time / (60 * 1000), (UINT)(history._time / 1000) % 60, (UINT)history._time % 1000 / 100
				);
				if (ImGui::MenuItem(label, 0, DAMAGEMETER.GetCurrentHistoryID() == i)) {
					DAMAGEMETER.SetHistory(i);
				}
			}

			ImGui::EndMenu();
		}

		if (ImGui::MenuItem(STR_UTILWINDOW_MEOW)) {
			//UTILLWINDOW.OpenWindow();
			PLOTWINDOW.OpenWindow();
		}

		if (ImGui::MenuItem(STR_MENU_OPTIONS)) {
			UIOPTION.OpenOption();
		}

		if (ImGui::MenuItem(STR_MENU_EXIT))
			exit(1);

		ImGui::EndPopup();
	}
}

VOID PlayerTable::SetupTable() {

	ImGuiTableFlags tableFlags = ImGuiTableFlags_None;
	tableFlags |= (ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable | ImGuiTableFlags_Resizable);

	if (ImGui::BeginTable("###Player Table", 36, tableFlags)) {

		ImGuiTableColumnFlags columnFlags = ImGuiTableColumnFlags_None;
		columnFlags |= ImGuiTableColumnFlags_NoSort;

		ImGui::SetWindowFontScale(_columnFontScale);

		ImGui::TableSetupColumn(STR_TABLE_NAME, ImGuiTableColumnFlags_NoReorder | ImGuiTableColumnFlags_NoHide | ImGuiTableColumnFlags_NoClip | ImGuiTableColumnFlags_WidthFixed | columnFlags, -1);
		ImGui::TableSetupColumn(STR_TABLE_DPS, columnFlags | ImGuiTableColumnFlags_WidthFixed, -1);
		ImGui::TableSetupColumn(STR_TABLE_DAMAGE_PERCENT, columnFlags | ImGuiTableColumnFlags_WidthFixed, -1);
		ImGui::TableSetupColumn(STR_TABLE_TOTAL_DAMAGE, columnFlags | ImGuiTableColumnFlags_WidthFixed, -1);
		ImGui::TableSetupColumn(STR_TABLE_TOTAL_HIT, columnFlags | ImGuiTableColumnFlags_WidthFixed, -1);
		ImGui::TableSetupColumn(STR_TABLE_CRIT_RATE, columnFlags | ImGuiTableColumnFlags_WidthFixed, -1);
		ImGui::TableSetupColumn(STR_TABLE_HIT_PER_SECOND, columnFlags | ImGuiTableColumnFlags_WidthFixed, -1);
		ImGui::TableSetupColumn(STR_TABLE_CRIT_HIT_PER_SECOND, columnFlags | ImGuiTableColumnFlags_WidthFixed, -1);
		ImGui::TableSetupColumn(STR_TABLE_SKILL_PER_SECOND, columnFlags | ImGuiTableColumnFlags_WidthFixed, -1);
		ImGui::TableSetupColumn(STR_TABLE_MAX_COMBO, columnFlags | ImGuiTableColumnFlags_WidthFixed, -1);
		ImGui::TableSetupColumn(STR_TABLE_ATTACK_CDMG_SUM, columnFlags | ImGuiTableColumnFlags_WidthFixed, -1);
		ImGui::TableSetupColumn(STR_TABLE_SOUL_GAUGE, columnFlags | ImGuiTableColumnFlags_WidthFixed, -1);
		ImGui::TableSetupColumn(STR_TABLE_ATTACK_SPEED, columnFlags | ImGuiTableColumnFlags_WidthFixed, -1);
		ImGui::TableSetupColumn(STR_TABLE_ARMOR_BREAK, columnFlags | ImGuiTableColumnFlags_WidthFixed, -1);
		ImGui::TableSetupColumn(STR_TABLE_STAMINA, columnFlags | ImGuiTableColumnFlags_WidthFixed, -1);
		ImGui::TableSetupColumn(STR_TABLE_SOUL_VAPOR, columnFlags | ImGuiTableColumnFlags_WidthFixed, -1);
		ImGui::TableSetupColumn(STR_TABLE_SOULSTONE_PERCENT, columnFlags | ImGuiTableColumnFlags_WidthFixed, -1);
		ImGui::TableSetupColumn(STR_TABLE_SOULSTONE_PROC, columnFlags | ImGuiTableColumnFlags_WidthFixed, -1);
		ImGui::TableSetupColumn(STR_TABLE_SOULSTONE_DAMAGE, columnFlags | ImGuiTableColumnFlags_WidthFixed, -1);
		ImGui::TableSetupColumn(STR_TABLE_AVERAGE_AB, columnFlags | ImGuiTableColumnFlags_WidthFixed, -1);
		ImGui::TableSetupColumn(STR_TABLE_MiSS, columnFlags | ImGuiTableColumnFlags_WidthFixed, -1);
		ImGui::TableSetupColumn(STR_TABLE_MISS_RATE, columnFlags | ImGuiTableColumnFlags_WidthFixed, -1);
		ImGui::TableSetupColumn(STR_TABLE_PARTIAL, columnFlags | ImGuiTableColumnFlags_WidthFixed, -1);
		ImGui::TableSetupColumn(STR_TABLE_GET_HIT_INCLUDEZERO, columnFlags | ImGuiTableColumnFlags_WidthFixed, -1);
		ImGui::TableSetupColumn(STR_TABLE_GET_HIT, columnFlags | ImGuiTableColumnFlags_WidthFixed, -1);
		ImGui::TableSetupColumn(STR_TABLE_GET_HIT_BS, columnFlags | ImGuiTableColumnFlags_WidthFixed, -1);
		ImGui::TableSetupColumn(STR_TABLE_ACC1_BS, columnFlags | ImGuiTableColumnFlags_WidthFixed, -1);
		ImGui::TableSetupColumn(STR_TABLE_ACC2_BS, columnFlags | ImGuiTableColumnFlags_WidthFixed, -1);
		ImGui::TableSetupColumn(STR_TABLE_ACC3_BS, columnFlags | ImGuiTableColumnFlags_WidthFixed, -1);
		ImGui::TableSetupColumn(STR_TABLE_EVADE_RATE_A, columnFlags | ImGuiTableColumnFlags_WidthFixed, -1);
		ImGui::TableSetupColumn(STR_TABLE_EVADE_RATE_B, columnFlags | ImGuiTableColumnFlags_WidthFixed, -1);
		ImGui::TableSetupColumn(STR_TABLE_GIGA_ENLIGHTEN, columnFlags | ImGuiTableColumnFlags_WidthFixed, -1);
		ImGui::TableSetupColumn(STR_TABLE_TERA_ENLIGHTEN, columnFlags | ImGuiTableColumnFlags_WidthFixed, -1);
		ImGui::TableSetupColumn(STR_TABLE_LOSED_HP, columnFlags | ImGuiTableColumnFlags_WidthFixed, -1);
		ImGui::TableSetupColumn(STR_TABLE_DODGE_COUNT, columnFlags | ImGuiTableColumnFlags_WidthFixed, -1);
		ImGui::TableSetupColumn(STR_TABLE_DEATH, columnFlags | ImGuiTableColumnFlags_WidthFixed, -1);
		//ImGuiTableColumnFlags_WidthStretch


		ImGui::TableHeadersRow();

		FLOAT window_width = ImGui::GetWindowWidth();

		ImGui::SetWindowFontScale(_tableFontScale);

		UpdateTable(window_width);

		ImGui::SetWindowFontScale(_globalFontScale);

		ImGui::EndTable();
	}

}

VOID PlayerTable::UpdateTable(FLOAT windowWidth) {
	UINT64 max_Damage = 1;
	CHAR comma[128] = { 0 }; CHAR label[128] = { 0 };

	for (auto itr = DAMAGEMETER.begin(); itr != DAMAGEMETER.end(); itr++) {

		// 
		if (UIOPTION.isSoloMode() && DAMAGEMETER.GetPlayerName((*itr)->GetID()) != STR_TABLE_YOU) {
			continue;
		}

		// 
		if (itr == DAMAGEMETER.begin())
			max_Damage = (*itr)->GetDamage();

		FLOAT damage_percent = static_cast<FLOAT>((DOUBLE)(*itr)->GetDamage() / (DOUBLE)max_Damage);

		if (damage_percent > 1)
			damage_percent = 1;
		else if (damage_percent < 0)
			damage_percent = 0;

		ImGui::TableNextRow();
		ImGui::TableNextColumn();


		DrawBar(windowWidth, damage_percent, UIOPTION.GetJobColor(DAMAGEMETER.GetPlayerJob((*itr)->GetID())));
		UINT64 milliTableTime = (UINT64)((DOUBLE)_tableTime * 1000);

		// NAME
		const CHAR* playerName = DAMAGEMETER.GetPlayerName((*itr)->GetID());
		if (UIOPTION.doHideName() && playerName != STR_TABLE_YOU) {
			playerName = "";
		}

		ImGuiStyle& style = ImGui::GetStyle();
		ImVec4 saved = ImVec4(style.Colors[0].x, style.Colors[0].y, style.Colors[0].z, style.Colors[0].w);
		
		UINT32 playerId = (*itr)->GetID();
		if (playerId == DAMAGEMETER.GetAggro()) {
			style.Colors[0] = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
		}
		else if (playerId == DAMAGEMETER.GetOwnerID(DAMAGEMETER.GetAggro())) {
			style.Colors[0] = ImVec4(0.0f, 0.0f, 1.0f, 1.0f);
		}
		
		//colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 0.95f);
		if (ImGui::Selectable(playerName, false, ImGuiSelectableFlags_SpanAllColumns))
			ToggleSelectInfo((*itr)->GetID());

		ImGui::TableNextColumn();
		style.Colors[0] = saved;


		// DPS
		if (_tableTime < 1) {
			sprintf_s(label, 128, "%.0lf", (float)0);
			ImGui::Text(comma);
		}
		else {
			DOUBLE dps = ((DOUBLE)(*itr)->GetDamage()) / _tableTime;
			if (UIOPTION.is1K())
				dps /= 1000;
			else if (UIOPTION.is1M())
				dps /= 1000000;
			sprintf_s(label, 128, "%.0lf", dps);
			TextCommma(label, comma);
			if (UIOPTION.is1K())
				strcat_s(comma, 128, "K");
			else if (UIOPTION.is1M())
				strcat_s(comma, 128, "M");
			ImGui::Text(comma);

			bool isFirstElement = ((itr - DAMAGEMETER.begin()) == 0);
			PLOTWINDOW.AddData((*itr)->GetID(), DAMAGEMETER.GetPlayerName((*itr)->GetID()), dps, _tableTime, isFirstElement);
		}
		

		ImGui::TableNextColumn();

		// D%
		if (DAMAGEMETER.GetPlayerTotalDamage() == 0) {
			sprintf_s(label, 128, "%.0lf", (float)0);
			ImGui::Text(label);
		}
		else {
			sprintf_s(label, 128, "%.0lf", ((DOUBLE)(*itr)->GetDamage() / (DOUBLE)DAMAGEMETER.GetPlayerTotalDamage()) * 100);
			ImGui::Text(label);
		}

		ImGui::TableNextColumn();

		// DAMAGE
		UINT64 damage = (*itr)->GetDamage();
		if (UIOPTION.is1K())
			damage /= 1000;
		else if (UIOPTION.is1M())
			damage /= 1000000;
		sprintf_s(label, 128, "%llu", damage);
		TextCommma(label, comma);
		if (UIOPTION.is1K())
			strcat_s(comma, 128, "K");
		else if (UIOPTION.is1M())
			strcat_s(comma, 128, "M");
		ImGui::Text(comma);

		ImGui::TableNextColumn();

		// HIT
		sprintf_s(label, 128, "%d", (*itr)->GetHitCount());
		TextCommma(label, comma);
		ImGui::Text(comma);

		ImGui::TableNextColumn();

		// CRIT
		FLOAT crit = 0;

		if ((*itr)->GetHitCount() != 0)
			crit = (FLOAT)(*itr)->GetCritHitCountForCritRate() / (FLOAT)(*itr)->GetHitCountForCritRate() * 100;

		sprintf_s(label, 128, "%.1f", crit);
		ImGui::Text(label);

		ImGui::TableNextColumn();

		// HIT/S
		if (_tableTime == (float)0) {
			sprintf_s(label, 128, "%d", 0);
			ImGui::Text(label);
		}
		else {
			sprintf_s(label, 128, "%.2lf", (DOUBLE)(*itr)->GetHitCount() / _tableTime);
			ImGui::Text(label);

		}
		ImGui::TableNextColumn();

		//CRIT/S
		if (_tableTime == (float)0) {
			sprintf_s(label, 128, "%d", 0);
			ImGui::Text(label);
		}
		else {
			sprintf_s(label, 128, "%.2lf", (DOUBLE)(*itr)->GetCritHitCount() / _tableTime);
			ImGui::Text(label);
		}

		ImGui::TableNextColumn();

		// Skill/s
		if (_tableTime == 0.0f) {
			sprintf_s(label, 128, "-");
			ImGui::Text(label);
		}
		else {
			sprintf_s(label, 128, "%.2lf", (DOUBLE)(*itr)->GetSkillUsed() / _tableTime);
			ImGui::Text(label);
		}

		ImGui::TableNextColumn();


		// MAXC
		sprintf_s(label, 128, "%d", (*itr)->GetMaxCombo());
		TextCommma(label, comma);
		ImGui::Text(comma);

		ImGui::TableNextColumn();

		UINT32 playerID = (*itr)->GetID();
		SWDamageMeter::SW_PLAYER_METADATA* playerMetaData = DAMAGEMETER.GetPlayerMetaData(playerID);

		// 
		if (playerMetaData == NULL) {
			continue;
		}

		if (DAMAGEMETER.GetPlayerName((*itr)->GetID()) != STR_TABLE_YOU || _tableTime == 0) {
			//// 
			sprintf_s(label, 128, "-");
			ImGui::Text(label);
			ImGui::TableNextColumn();

			//SG
			sprintf_s(label, 128, "%.1f", playerMetaData->GetStat(StatType::SG));
			ImGui::Text(label);
			ImGui::TableNextColumn();
			// 
			sprintf_s(label, 128, "%.1f", playerMetaData->GetStat(StatType::AttackSpeed));
			ImGui::Text(label);
			ImGui::TableNextColumn();

			// 
			sprintf_s(label, 128, "-");
			ImGui::Text(label);
			ImGui::TableNextColumn();
			// 
			sprintf_s(label, 128, "-");
			ImGui::Text(label);
			ImGui::TableNextColumn();
			// SV
			sprintf_s(label, 128, "-");
			ImGui::Text(label);
			ImGui::TableNextColumn();
		}
		else {
			// 
			DOUBLE gongchihap = (DOUBLE)playerMetaData->GetStat(StatType::MaxAttack) + (DOUBLE)playerMetaData->GetStat(StatType::CritDamage);
			if (UIOPTION.is1K())
				gongchihap /= 1000;
			else if (UIOPTION.is1M())
				gongchihap /= 1000000;
			sprintf_s(label, 128, "%.0f", gongchihap);
			TextCommma(label, comma);
			if (UIOPTION.is1K())
				strcat_s(comma, 128, "K");
			else if (UIOPTION.is1M())
				strcat_s(comma, 128, "M");
			ImGui::Text(comma);
			ImGui::TableNextColumn();

			// SG
			sprintf_s(label, 128, "%.1f", playerMetaData->GetStat(StatType::SG));
			ImGui::Text(label);

			ImGui::TableNextColumn();
			// 
			sprintf_s(label, 128, "%.1f", playerMetaData->GetStat(StatType::AttackSpeed));
			ImGui::Text(label);

			ImGui::TableNextColumn();
			// 
			sprintf_s(label, 128, "%.1f", playerMetaData->GetStat(StatType::ArmorBreak));
			PLOTWINDOW.AddAbData(playerMetaData->GetStat(StatType::ArmorBreak), _tableTime);
			ImGui::Text(label);
			
			ImGui::TableNextColumn();
			// stamina
			sprintf_s(label, 128, "%.1f", playerMetaData->GetStat(StatType::Stamina));
			ImGui::Text(label);

			ImGui::TableNextColumn();
			// SV
			sprintf_s(label, 128, "%.1f", playerMetaData->GetStat(StatType::SV));
			ImGui::Text(label);
			ImGui::TableNextColumn();
		}



		// 
		DOUBLE soulstoneAllPercent;
		if ((*itr)->GetDamage() == 0) {
			soulstoneAllPercent = 0.0;
		}
		else {
			soulstoneAllPercent = ((double)(*itr)->GetSoulstoneDamage()) / (*itr)->GetDamage() * 100;
		}

		sprintf_s(label, 128, "%.1f", soulstoneAllPercent);
		ImGui::Text(label);
		ImGui::TableNextColumn();

		//
		DOUBLE soulstoneProcRate;
		if ((*itr)->GetCritHitCountForCritRate() == 0) {
			soulstoneProcRate = 0.0;
		}
		else {
			soulstoneProcRate = ((double)(*itr)->GetSoulstoneCount()) / (*itr)->GetHitCountForCritRate() * 100;
		}

		sprintf_s(label, 128, "%.1f", soulstoneProcRate);
		ImGui::Text(label);
		ImGui::TableNextColumn();

		// 
		DOUBLE soulstoneDamage;
		if ((*itr)->GetDamageForSoulstone() == 0) {
			soulstoneDamage = 0.0;
		}
		else {
			soulstoneDamage = ((double)(*itr)->GetSoulStoneDamageForSoulstone()) / (*itr)->GetDamageForSoulstone() * 100;
		}
		sprintf_s(label, 128, "%.1f", soulstoneDamage);
		ImGui::Text(label);
		ImGui::TableNextColumn();

		// 
		static DOUBLE savedResult = 0;

		if (DAMAGEMETER.GetPlayerName((*itr)->GetID()) != STR_TABLE_YOU || _tableTime == 0) {
			sprintf_s(label, 128, "-");
		}

		else if (DAMAGEMETER.isHistoryMode()) {
			savedResult = (*itr)->GetHistoryAvgAB();
			sprintf_s(label, 128, "%.1f", savedResult);
		}
		else {

			if ((INT64)(milliTableTime - playerMetaData->_avgABPreviousTime) < 0) {
				sprintf_s(label, 128, "%.1f", savedResult);
			}
			else {
				UINT64 timeDifference = (milliTableTime - playerMetaData->_avgABPreviousTime);
				DOUBLE currentAB = playerMetaData->GetStat(StatType::ArmorBreak);
				currentAB = currentAB > 100.0 ? 100.0 : currentAB; // 
				UINT64 calculatedAvgAB = static_cast<UINT64>((playerMetaData->_avgABSum + timeDifference * currentAB));

				savedResult = (DOUBLE)calculatedAvgAB / milliTableTime;
				sprintf_s(label, 128, "%.1f", savedResult);
			}
		}

		ImGui::Text(label);
		ImGui::TableNextColumn();
		// Miss
		sprintf_s(label, 128, "%d", (*itr)->GetMissCount());
		TextCommma(label, comma);
		ImGui::Text(comma);

		ImGui::TableNextColumn();
		// Miss%
		if ((*itr)->GetMissCount() == 0 || (*itr)->GetHitCountForCritRate() == 0) {
			sprintf_s(label, 128, "%.1f", 0.0);
		}
		else {
			sprintf_s(label, 128, "%.1f", (DOUBLE)(*itr)->GetMissCount() / (*itr)->GetHitCountForCritRate() * 100);
		}

		ImGui::Text(label);
		ImGui::TableNextColumn();

		// 
		sprintf_s(label, 128, "%.1f", playerMetaData->GetStat(StatType::PartialDamage));
		ImGui::Text(label);
		ImGui::TableNextColumn();

		//
		sprintf_s(label, 128, "%d", (*itr)->GetGetHitAll());
		TextCommma(label, comma);
		ImGui::Text(comma);

		ImGui::TableNextColumn();

		// 
		sprintf_s(label, 128, "%d", (*itr)->GetGetHit());
		TextCommma(label, comma);
		ImGui::Text(comma);

		ImGui::TableNextColumn();

		// 
		sprintf_s(label, 128, "%d", (*itr)->GetGetHitBS());
		TextCommma(label, comma);
		ImGui::Text(comma);

		ImGui::TableNextColumn();

		// 
		static DOUBLE gear90savedResult = 0.0;
		static DOUBLE gear50savedResult = 0.0;
		static DOUBLE acc01savedResult = 0.0;
		static DOUBLE acc02savedResult = 0.0;

		if (DAMAGEMETER.GetPlayerName((*itr)->GetID()) != STR_TABLE_YOU || _tableTime == 0) {
			sprintf_s(label, 128, "-");
			TextCommma(label, comma);
			ImGui::Text(comma);
			ImGui::TableNextColumn();

			sprintf_s(label, 128, "-");
			TextCommma(label, comma);
			ImGui::Text(comma);
			ImGui::TableNextColumn();

			sprintf_s(label, 128, "-");
			TextCommma(label, comma);
			ImGui::Text(comma);
			ImGui::TableNextColumn();
		}

		else if (DAMAGEMETER.isHistoryMode()) {
			DOUBLE gearSavedResultSum = (*itr)->GetHistoryBS(90) + (*itr)->GetHistoryBS(50);
			acc01savedResult = (*itr)->GetHistoryBS(1);
			acc02savedResult = (*itr)->GetHistoryBS(2);

			sprintf_s(label, 128, "%.0f", gearSavedResultSum + (*itr)->GetHistoryBS(1));
			TextCommma(label, comma);
			ImGui::Text(comma);
			ImGui::TableNextColumn();

			sprintf_s(label, 128, "%.0f", gearSavedResultSum + (*itr)->GetHistoryBS(2));
			TextCommma(label, comma);
			ImGui::Text(comma);
			ImGui::TableNextColumn();

			sprintf_s(label, 128, "%.0f", gearSavedResultSum + 650);
			TextCommma(label, comma);
			ImGui::Text(comma);
			ImGui::TableNextColumn();
		}
		else {

			UINT64 bs3GearOngoing = playerMetaData->CalBsGear3Set(false, milliTableTime);
			if (bs3GearOngoing != 0 && _accumulatedTime == 0) {
				gear90savedResult = (DOUBLE)(playerMetaData->_gear90Sum + bs3GearOngoing) / milliTableTime;
			}
			if (bs3GearOngoing == 0 && _accumulatedTime == 0) {
				gear90savedResult = (DOUBLE)playerMetaData->_gear90Sum / milliTableTime;
			}

			UINT64 bs4GearOngoing = playerMetaData->CalBsGear4Set(false, milliTableTime);
			if (bs4GearOngoing != 0 && _accumulatedTime == 0) {
				gear50savedResult = (DOUBLE)(playerMetaData->_gear50Sum + bs4GearOngoing) / milliTableTime;
			}
			if (bs4GearOngoing == 0 && _accumulatedTime == 0) {
				gear50savedResult = (DOUBLE)(playerMetaData->_gear50Sum) / milliTableTime;
			}

			UINT64 bsAcc1Ongoing = playerMetaData->CalBsAccSet1(false, milliTableTime);
			if (bsAcc1Ongoing != 0 && _accumulatedTime == 0) {
				acc01savedResult = (DOUBLE)(playerMetaData->_acc01Sum + bsAcc1Ongoing) / milliTableTime;
			}
			if (bsAcc1Ongoing == 0 && _accumulatedTime == 0) {
				acc01savedResult = (DOUBLE)(playerMetaData->_acc01Sum) / milliTableTime;
			}

			UINT64 bsAcc2Ongoing = playerMetaData->CalBsAccSet2(false, milliTableTime);
			if (bsAcc2Ongoing != 0 && _accumulatedTime == 0) {
				acc02savedResult = (DOUBLE)(playerMetaData->_acc02Sum + bsAcc2Ongoing) / milliTableTime;
			}
			if (bsAcc2Ongoing == 0 && _accumulatedTime == 0) {
				acc02savedResult = (DOUBLE)(playerMetaData->_acc02Sum) / milliTableTime;
			}

			DOUBLE gearSum = gear50savedResult + gear90savedResult;

			sprintf_s(label, 128, "%.0f", gearSum + acc01savedResult);
			TextCommma(label, comma);
			ImGui::Text(comma);
			ImGui::TableNextColumn();

			sprintf_s(label, 128, "%.0f", gearSum + acc02savedResult);
			TextCommma(label, comma);
			ImGui::Text(comma);
			ImGui::TableNextColumn();

			sprintf_s(label, 128, "%.0f", gearSum + 650);
			TextCommma(label, comma);
			ImGui::Text(comma);
			ImGui::TableNextColumn();

			//if ((INT64)(milliTableTime - playerMetaData->_gear90PreviousTime) < 0) {
			//	sprintf_s(label, 128, "%.1f", gear90savedResult);
			//}
			//else if (playerMetaData->_gear90Started == false) {
			//	sprintf_s(label, 128, "0.0");
			//}
			//else {
			//	UINT64 timeDifference = (milliTableTime - playerMetaData->_gear90PreviousTime);
			//	timeDifference = (timeDifference >= 5000) ? 5000 : timeDifference;

			//	UINT64 gear90Sum = (playerMetaData->_gear90Sum + timeDifference * 500);

			//	gear90savedResult = (DOUBLE)gear90Sum / milliTableTime;
			//	sprintf_s(label, 128, "%.1f", gear90savedResult);
			//}


		}

		//


		if ((*itr)->GetGetHitAll() == 0) {
			sprintf_s(label, 128, "-");
		}
		else {
			sprintf_s(label, 128, "%.1f%%", (DOUBLE)(*itr)->GetGetHitMissed() / (*itr)->GetGetHitAll() * 100);
		}
		ImGui::Text(label);
		ImGui::TableNextColumn();


		if ((*itr)->GetGetHit() == 0) {
			sprintf_s(label, 128, "-");
		}
		else {
			sprintf_s(label, 128, "%.1f%%", (DOUBLE)(*itr)->GetGetHitMissedReal() / (*itr)->GetGetHit() * 100);
		}
		ImGui::Text(label);
		ImGui::TableNextColumn();


		// 
		if (DAMAGEMETER.GetPlayerName((*itr)->GetID()) != STR_TABLE_YOU || _tableTime == 0) {
			sprintf_s(label, 128, "-");
			ImGui::Text(label);
			ImGui::TableNextColumn();

			sprintf_s(label, 128, "-");
			ImGui::Text(label);
			ImGui::TableNextColumn();
		}
		else {
			sprintf_s(label, 128, "%u", (*itr)->GetGigaEnlighten());
			ImGui::Text(label);
			ImGui::TableNextColumn();

			sprintf_s(label, 128, "%u", (*itr)->GetTeraEnlighten());
			ImGui::Text(label);
			ImGui::TableNextColumn();
		}

		// HP
		DOUBLE losedHP = 0.0;
		if (DAMAGEMETER.isHistoryMode()) {
			losedHP = (*itr)->GetHistoryLosedHP();
		}
		else {
			losedHP = playerMetaData->_losedHp;
		}
		if (UIOPTION.is1K())
			losedHP /= 1000;
		else if (UIOPTION.is1M())
			losedHP /= 1000000;
		sprintf_s(label, 128, "%.0f", losedHP);
		TextCommma(label, comma);

		if (UIOPTION.is1K())
			strcat_s(comma, 128, "K");
		else if (UIOPTION.is1M())
			strcat_s(comma, 128, "M");

		ImGui::Text(comma);
		ImGui::TableNextColumn();

		// 
		if (DAMAGEMETER.GetPlayerName((*itr)->GetID()) != STR_TABLE_YOU || _tableTime == 0) {
			sprintf_s(label, 128, "-");
			ImGui::Text(label);
			ImGui::TableNextColumn();
		}
		else {
			sprintf_s(label, 128, "%u", (*itr)->GetDodgeUsed());
			ImGui::Text(label);
			ImGui::TableNextColumn();
		}

		// Death Counter
		sprintf_s(label, 128, "%u", (*itr)->GetDeathCount());
		ImGui::Text(label);
		ImGui::TableNextColumn();

		//  (etc)
		PLOTWINDOW.AddJqData((*itr)->GetJqStack(), _tableTime);
	}
}

VOID PlayerTable::DrawBar(FLOAT window_Width, FLOAT percent, ImU32 color) {

	auto draw_list = ImGui::GetWindowDrawList();

	FLOAT result_width = window_Width * percent;
	FLOAT height = ImGui::GetFontSize();
	ImVec2 line = ImGui::GetCursorScreenPos();

	line.x = FLOOR(line.x);	line.y = line.y;
	height = height;

	draw_list->AddRectFilled(ImVec2(line.x, line.y), ImVec2(line.x + result_width, line.y + height), color, 0, 0);
}

BOOL PlayerTable::ToggleSelectInfo(UINT32 id) {

	for (auto itr = _selectInfo.begin(); itr != _selectInfo.end(); itr++) {
		if ((*itr)->_playerID == id) {
			(*itr)->_isSelected = !(*itr)->_isSelected;

			return (*itr)->_isSelected;
		}
	}

	SELECTED_PLAYER* selectinfo = new SELECTED_PLAYER(id, TRUE, new SpecificInformation(id));
	_selectInfo.push_back(selectinfo);

	return selectinfo->_isSelected;
}

VOID PlayerTable::ShowSelectedTable() {

	for (auto itr = _selectInfo.begin(); itr != _selectInfo.end(); itr++) {
		if ((*itr)->_isSelected == TRUE) {
			(*itr)->_specificInfo->Update(&(*itr)->_isSelected, itr - _selectInfo.begin());
		}
	}
}