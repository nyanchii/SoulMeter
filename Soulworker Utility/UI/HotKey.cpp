#include "pch.h"
#include "HotKey.h"
#include ".\UI\DX Input.h"
#include ".\UI\PlayerTable.h"
#include ".\Damage Meter\Damage Meter.h"

AutoHotKey::~AutoHotKey() {
	_callbacks.clear();
}

AutoHotKey::AutoHotKey(const INT key1, INT key2, INT key3, const CHAR* name, INT callback_num, HOTKEYCALLBACK* callback, ...) {
	_key[0] = key1; _key[1] = key2; _key[2] = key3;
	_isActive = FALSE;

	if (key3 == -1 && key2 == -1)
		_hotkeyCount = 1;
	else if (key3 == -1 && key2 != -1)
		_hotkeyCount = 2;
	else if (key3 != -1 && key2 != -1)
		_hotkeyCount = 3;

	if (name != nullptr)
		strcpy_s(_name, name);

	va_list va;
	va_start(va, callback);

	for (INT i = 0; i < callback_num; i++) {
		_callbacks.push_back(move(*callback));
		callback = va_arg(va, HOTKEYCALLBACK*);
	}

	va_end(va);
}

AutoHotKey::AutoHotKey(const INT key1, INT key2, const CHAR* name, INT callback_num, HOTKEYCALLBACK* callback, ...) {
	_key[0] = key1; _key[1] = key2; _key[2] = -1;
	_isActive = FALSE;

	if (key2 == -1)
		_hotkeyCount = 1;
	else if (key2 != -1)
		_hotkeyCount = 2;

	if (name != nullptr)
		strcpy_s(_name, name);

	va_list va;
	va_start(va, callback);

	for (INT i = 0; i < callback_num; i++) {
		_callbacks.push_back(move(*callback));
		callback = va_arg(va, HOTKEYCALLBACK*);
	}

	va_end(va);
}

AutoHotKey::AutoHotKey(const INT key1, const CHAR* name, INT callback_num, HOTKEYCALLBACK* callback, ...) : _hotkeyCount(1) {
	_key[0] = key1; _key[1] = -1, _key[2] = -1;
	_isActive = FALSE;

	if (name != nullptr)
		strcpy_s(_name, name);

	va_list va;
	va_start(va, callback);

	for (INT i = 0; i < callback_num; i++) {
		_callbacks.push_back(move(*callback));
		callback = va_arg(va, HOTKEYCALLBACK*);
	}

	va_end(va);
}

VOID AutoHotKey::CheckKey() {

	if (_hotkeyCount > HOTKEY._pressedKey.size()) {
		_isActive = FALSE;
		return;
	}

	if (_isActive) {
		if (DXINPUT.isKeyIdle(_key[0]) || DXINPUT.isKeyIdle(_key[1]) || DXINPUT.isKeyIdle(_key[2])) {
			_isActive = FALSE;
		}
		else {
			return;
		}
	}

	for (auto itr = HOTKEY._pressedKey.begin(); itr != HOTKEY._pressedKey.end(); itr++) {

		BOOL find = FALSE;

		for (INT i = 0; i < _hotkeyCount; i++) {
			if (_key[i] == *itr) {
				find = TRUE;
				break;
			}
		}

		if (!find) {
			_isActive = FALSE;
			return;
		}
	}

#if DEBUG_HOTKEY == 1
	Log::WriteLog(const_cast<LPTSTR>(_T("[DEBUG] [HOTKEY] [KEY1 = %d] [KEY2 = %d] [KEY3 = %d] [%s]")), _key[0], _key[1], _key[2], _name);
#endif

	_isActive = TRUE;

	for (auto itr = _callbacks.begin(); itr != _callbacks.end(); itr++) {
		if ((*itr) != nullptr) {
			DAMAGEMETER.GetLock();
			{
				(*itr)();
			}
			DAMAGEMETER.FreeLock();
		}
	}
}

HotKey::~HotKey() {
	for (auto itr = _hotkeys.begin(); itr != _hotkeys.end(); itr++) {
		delete* itr;
	}
}

VOID HotKey::CheckKey() {

	for (UINT i = 0; i < DXINPUT.GetStateSize(); i++) {
		if (DXINPUT.isKeyRelease(i)) {
			for (auto itr = _pressedKey.begin(); itr != _pressedKey.end(); itr++) {
				if (*itr == i) {

#if DEBUG_HOTKEY == 1
					Log::WriteLogA(const_cast<char*>("Release Key - %d"), i);
#endif
					_pressedKey.erase(itr);
					break;
				}
			}
		}
	}

	for (UINT i = 0; i < DXINPUT.GetStateSize(); i++) {
		if (DXINPUT.isKeyDown(i)) {
#if DEBUG_HOTKEY == 1
			Log::WriteLogA(const_cast<char*>("Down Key - %d"), i);
#endif
			_pressedKey.push_back(i);
		}
	}
}

VOID HotKey::CheckHotKey() {
	
	for (auto itr = _hotkeys.begin(); itr != _hotkeys.end(); itr++) {
		(*itr)->CheckKey();
	}
}

VOID HotKey::Update() {

	CheckKey();
	CheckHotKey();

}

VOID HotKey::InsertHotkeyToogle(INT key1, INT key2, INT key3) {
	
//	HOTKEYCALLBACK callback = bind(&SWDamageMeter::Toggle, &DAMAGEMETER);

//	_hotkeys.push_back(new AutoHotKey(key1, key2, key3, "Toogle", 1, &callback));

}

VOID HotKey::InsertHotkeyStop(INT key1, INT key2, INT key3) {

	HOTKEYCALLBACK callback1 = bind(&SWDamageMeter::Clear, &DAMAGEMETER);
	HOTKEYCALLBACK callback2 = bind(&PlayerTable::ClearTable, &PLAYERTABLE);

	_hotkeys.push_back(new AutoHotKey(key1, key2, key3, "Clear", 2, &callback1, &callback2));
}

vector<AutoHotKey*>::const_iterator HotKey::begin() { 
	return _hotkeys.begin(); 
}

vector<AutoHotKey*>::const_iterator HotKey::end() { 
	return _hotkeys.end(); 
}