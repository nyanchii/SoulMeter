#pragma once

#define WIN32_LEAN_AND_MEAN

#include <Windows.h>

#include <DbgHelp.h>
#pragma comment(lib, "Dbghelp.lib")

#include <stdio.h>
#include <mutex>
#include <string>
#include <functional>
#include <time.h>
#include <array>
#include <comdef.h>
#include <locale>
#include <tchar.h>
#include <vector>
#include <algorithm>
#include <codecvt>

// IMGUI
#include "imgui\imgui.h"
#include "imgui\backends\imgui_impl_win32.h"
#include "imgui\backends\imgui_impl_dx11.h"
#include "imgui\Custom.h"

// tinyxml2
#include "tinyxml2\tinyxml2.h"

// DirectX
#include <d3d11.h>
#include <dinput.h>

// NPcap
#include "npcap\Include\pcap.h"

// WinDivert
#include "Windivert\include\windivert.h"

// SQLite3
#include "SQLite\include\sqlite3.h"

// Utility
#include ".\Util\Type.h"
#include ".\Util\Singleton.h"
#include ".\Util\Log.h"
#include ".\Util\ExceptionHandler.h"
#include ".\Util\CriticalSection.h"
#include ".\Util\MultiThreadSync.h"
#include ".\Util\MemoryPool.h"
