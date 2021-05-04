#include "pch.h"
#include ".\Packet Capture/MyNpcap.h"
#include ".\Packet Capture/MyWinDivert.h"
#include ".\Damage Meter/Damage Meter.h"
#include ".\UI\UiWindow.h"
#include ".\Damage Meter\MySQLite.h"

#if defined(DEBUG) || defined(_DEBUG)
#pragma comment(linker, "/entry:WinMainCRTStartup /subsystem:console" )
#endif

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance, PSTR cmdLine, int showCmd) {

	_wsetlocale(LC_ALL, L"zh-TW");

	MiniDump::Begin();
	
	if (!SWDB.Init()) {
		Log::WriteLog(const_cast<LPTSTR>(_T("InitDB Failed")));
		exit(-1);
	}

	if (WINDIVERT.Init()) {
		if (NPCAP.Init()) {
			exit(-1);
		}
	}

	UiWindow ui = UiWindow();
	ui.Init(0, 0, 0, 0);
	ui.Run();

	MiniDump::End();
}