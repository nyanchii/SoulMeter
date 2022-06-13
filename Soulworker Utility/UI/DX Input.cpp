#include "pch.h"
#include ".\UI\DX Input.h"


BOOL DXInput::Init(HINSTANCE hinst, HWND hWnd) {
	
	HRESULT result = DirectInput8Create(hinst, DIRECTINPUT_VERSION, IID_IDirectInput8, (LPVOID*)&_directInput, NULL);

	if (FAILED(result)) {
		Log::WriteLog(const_cast<LPTSTR>(_T("Error in DirectInput8Create")));
		return FALSE;
	}
	

	if (FAILED(_directInput->CreateDevice(GUID_SysKeyboard, &_keyboard, NULL))) {
		Log::WriteLog(const_cast<LPTSTR>(_T("Error in CreateDevice")));
		return FALSE;
	}

	if (FAILED(_keyboard->SetDataFormat(&c_dfDIKeyboard))) {
		Log::WriteLog(const_cast<LPTSTR>(_T("Error in SetDataFormat")));
		return FALSE;
	}

	if (FAILED(_keyboard->SetCooperativeLevel(hWnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE))) {
		Log::WriteLog(const_cast<LPTSTR>(_T("Error in SetCooperativeLevel")));
		return FALSE;
	}

	if (FAILED(_keyboard->Acquire())) {
		Log::WriteLog(const_cast<LPTSTR>(_T("Error in Acquire")));
		return FALSE;
	}

	return TRUE;
}

VOID DXInput::Shutdown() {

	if (_keyboard != nullptr) {
		_keyboard->Unacquire();
		_keyboard->Release();
		_keyboard = nullptr;
	}

	if (_directInput != nullptr) {
		_directInput->Release(); 
		_directInput = nullptr;
	}

}

BOOL DXInput::Update() {

	HRESULT result = _keyboard->GetDeviceState(sizeof(_keyboardState), (LPVOID)&_keyboardState);

	if (FAILED(result))
	{
		if ((result == DIERR_INPUTLOST) || (result == DIERR_NOTACQUIRED))
		{
			_keyboard->Acquire();
		}
		else
		{
			return FALSE;
		}
	}

	for (INT i = 0; i < 256; i++) {
		_inputInfo[i].Update(_keyboardState[i] & 0x80 ? TRUE : FALSE);
	}

	return TRUE;
}

BOOL DXInput::isKeyDown(UINT i) {
	
	if (i >= 0 && i < GetStateSize())
		return _inputInfo[i].isDown();
	else
		return FALSE;
}

BOOL DXInput::isKeyRelease(UINT i) {

	if (i >= 0 && i < GetStateSize())
		return _inputInfo[i].isRelease();
	else
		return FALSE;
}

BOOL DXInput::isKeyPressed(UINT i) {

	if (i >= 0 && i < GetStateSize())
		return _inputInfo[i].isPress();
	else
		return FALSE;
}

BOOL DXInput::isKeyIdle(UINT i) {
	if (i >= 0 && i < GetStateSize())
		return _inputInfo[i].isIdle();
	else
		return FALSE;
}

UINT DXInput::GetStateSize() {
	return 256;
}