#include "pch.h"
#include ".\UI\SpecificInfomation.h"
#include ".\UI\Option.h"
#include ".\Damage Meter\Damage Meter.h"


SpecificInformation::SpecificInformation(UINT32 playerID) : _playerID(playerID), _monsterID(0), _globalFontScale(0), _columnFontScale(0), _tableFontScale(0) {

}

SpecificInformation::~SpecificInformation() {
	Clear();
}

VOID SpecificInformation::Clear() {

}

VOID SpecificInformation::SetupFontScale() {
	ImFont* font = ImGui::GetFont();

	_globalFontScale = font->Scale;
	_columnFontScale = _globalFontScale * UIOPTION.GetColumnFontScale();
	_tableFontScale = _globalFontScale * UIOPTION.GetTableFontScale();
}

VOID SpecificInformation::Update(BOOL* open) {

	SetupFontScale();

	CHAR title[128] = { 0 };

	sprintf_s(title, 128, "%s %s###PaouN7zAC8K", DAMAGEMETER.GetPlayerName(_playerID), u8"詳細資料");
	ImGui::Begin(title, (bool*)open, ImGuiWindowFlags_None);
	{
		sprintf_s(title, 128, "##tab%d", _playerID);

		ImGui::BeginTabBar(title);
		{
			UpdateSkillInfo();
		}
		ImGui::EndTabBar();
	}
	ImGui::End();
}

VOID SpecificInformation::UpdateSkillInfo() {
	
	ImGui::BeginTabItem(u8"技能輸出");
	{
		UpdateMonsterCombo();
		
		ImGui::OutlineText::PushOutlineText(ImGui::IMGUIOUTLINETEXT(UIOPTION.GetOutlineColor(), 1));
		ImGui::TextAlignCenter::SetTextAlignCenter();
		{
			UpdateSkillTable();
		}
		ImGui::TextAlignCenter::UnSetTextAlignCenter();
		ImGui::OutlineText::PopOutlineText();
	}
	ImGui::EndTabItem();
}

VOID SpecificInformation::UpdateMonsterCombo() {

	auto player = DAMAGEMETER.GetPlayerInfo(_playerID);

	if (player == DAMAGEMETER.end())
		return;

	const CHAR* comboPreview = nullptr;

	auto monster = (*player)->GetMonsterInfo(_monsterID);

	if (monster != (*player)->end()) {
		comboPreview = (*monster)->GetName();
	}

	if(ImGui::BeginCombo(u8"怪物", comboPreview, ImGuiComboFlags_HeightLarge)) {
	
		for (auto itr = (*player)->begin(); itr != (*player)->end(); itr++) {
			
			CHAR label[128] = { 0 };
			sprintf_s(label, 128, "%s##%d", (*itr)->GetName(), (*itr)->GetID());

			if (ImGui::Selectable(label)) {
				_monsterID = (*itr)->GetID();
#if DEBUG_MONSTER_COMBO == 1
				Log::WriteLog(const_cast<LPTSTR>(_T("[DEBUG] [MONSTER COMBO] [MONSTER ID = %d] [NAME = %s]")), _monsterID, (*itr)->GetName());
#endif
			}
		}

		ImGui::EndCombo();
	}
}

VOID SpecificInformation::UpdateSkillTable() {

	auto player = DAMAGEMETER.GetPlayerInfo(_playerID);

	if (player == DAMAGEMETER.end())
		return;

	auto monster = (*player)->GetMonsterInfo(_monsterID);

	if (monster == (*player)->end())
		return;

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

	CHAR table[128] = { 0 };
	sprintf_s(table, 128, "##skilltable%d", _playerID);
	if(ImGui::BeginTable(table, 7, ImGuiTableFlags_Resizable | ImGuiTableFlags_Hideable | ImGuiTableFlags_Reorderable)) {
	
		ImGui::SetWindowFontScale(_columnFontScale);

		ImGui::TableSetupColumn("NAME", ImGuiTableColumnFlags_NoReorder | ImGuiTableColumnFlags_NoHide | ImGuiTableColumnFlags_NoClip | ImGuiTableColumnFlags_WidthFixed, -1);
		ImGui::TableSetupColumn("DPS", ImGuiTableColumnFlags_WidthFixed, -1);
		ImGui::TableSetupColumn("D%", ImGuiTableColumnFlags_WidthFixed, -1);
		ImGui::TableSetupColumn("DAMAGE", ImGuiTableColumnFlags_WidthFixed | ImGuiTableColumnFlags_DefaultHide, -1);
		ImGui::TableSetupColumn("HIT", ImGuiTableColumnFlags_WidthFixed | ImGuiTableColumnFlags_DefaultHide, -1);
		ImGui::TableSetupColumn("CRIT", ImGuiTableColumnFlags_WidthFixed | ImGuiTableColumnFlags_DefaultHide, -1);
		ImGui::TableSetupColumn("HIT/s", ImGuiTableColumnFlags_WidthFixed, -1);
		ImGui::TableHeadersRow();

		UINT64 max_Damage = 1;
		CHAR comma[128] = { 0 }; CHAR label[128] = { 0 };
		UINT windowWidth = ImGui::GetWindowWidth();

		ImGui::SetWindowFontScale(_tableFontScale);

		for (auto itr = (*monster)->begin(); itr != (*monster)->end(); itr++) {

			if (itr == (*monster)->begin())
				max_Damage = (*itr)->GetDamage();

			FLOAT damage_percent = (DOUBLE)(*itr)->GetDamage() / (DOUBLE)max_Damage;

			if (damage_percent > 1)
				damage_percent = 1;
			else if (damage_percent < 0)
				damage_percent = 0;

			ImGui::TableNextRow();
			ImGui::TableNextColumn();

			DrawBar(windowWidth, damage_percent, UIOPTION.GetJobColor(DAMAGEMETER.GetPlayerJob(_playerID)));

			// NAME
			ImGui::Text((*itr)->GetName());

			ImGui::TableNextColumn();

			// DPS
			DOUBLE dps = ((DOUBLE)(*itr)->GetDamage()) / DAMAGEMETER.GetTime();
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

			ImGui::TableNextColumn();

			// D%
			sprintf_s(label, 128, "%.0lf", ((DOUBLE)(*itr)->GetDamage() / (DOUBLE)(*monster)->GetSkillTotalDamage() * 100));
			ImGui::Text(label);

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
				crit = (FLOAT)(*itr)->GetCritHitCount() / (FLOAT)(*itr)->GetHitCount() * 100;

			sprintf_s(label, 128, "%.0f", crit);
			ImGui::Text(label);

			ImGui::TableNextColumn();

			// HIT/S
			sprintf_s(label, 128, "%.2lf", (DOUBLE)(*itr)->GetHitCount() / DAMAGEMETER.GetTime());
			ImGui::Text(label);
		}
		
		ImGui::SetWindowFontScale(_globalFontScale);
		ImGui::EndTable();
	}

	style.WindowPadding.x = prevWindowPadding.x;
	style.WindowPadding.y = prevWindowPadding.y;

	style.Colors[10] = prevInActiveColor;
	style.Colors[11] = prevActiveColor;

}

VOID SpecificInformation::DrawBar(FLOAT window_Width, FLOAT percent, ImU32 color) {

	auto draw_list = ImGui::GetWindowDrawList();

	FLOAT result_width = window_Width * percent;
	FLOAT height = ImGui::GetFontSize();
	ImVec2 line = ImGui::GetCursorScreenPos();

	line.x = FLOOR(line.x);	line.y = line.y;
	height = height;

	draw_list->AddRectFilled(ImVec2(line.x, line.y), ImVec2(line.x + result_width, line.y + height), color, 0, 0);
}