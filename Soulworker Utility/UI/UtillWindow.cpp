#include "pch.h"
#include "UtillWindow.h"
#include ".\Language\Region.h"

VOID UtillWindow::AddLog(string log)
{
	SYSTEMTIME SystemTime;
	GetLocalTime(&SystemTime);
	sprintf_s(CurrentDate, 32, "[%2d:%2d:%2d] ",
		SystemTime.wHour,
		SystemTime.wMinute,
		SystemTime.wSecond);
	log.insert(0, CurrentDate);
	logList.push_back(log);

	if (logList.size() > 200) {
		logList.pop_front();
	}
}

VOID UtillWindow::OpenWindow()
{
	_isOpen = true;
}

VOID UtillWindow::Update()
{
	//if (_isOpen) {
	//	ImGui::Begin(u8"", &_isOpen, ImGuiWindowFlags_None);
	//	
	//	ImGui::BeginChild("Scrolling");
	//	std::list<string>::iterator it;
	//	for (it = logList.begin(); it != logList.end(); it++)
	//		ImGui::Text(it->c_str());
	//	ImGui::SetScrollHere(1.0f);
	//	ImGui::EndChild();
	//	ImGui::End();
	//}
	if (_isOpen) {
		if (ImGui::BeginTabItem(STR_UTILWINDOW_COMBATLOG))
		{
			ImGui::BeginChild("Scrolling");
			std::list<string>::iterator it;
			for (it = logList.begin(); it != logList.end(); it++)
				ImGui::Text(it->c_str());
			ImGui::SetScrollHere(1.0f);
			ImGui::EndChild();

			ImGui::EndTabItem();
		}
	}
}

UtillWindow::UtillWindow()
{
}

UtillWindow::~UtillWindow()
{
}
