#pragma once

#define DXINPUT DXInput::getInstance()

#pragma comment(lib, "dinput8.lib") 
#pragma comment(lib, "dxguid.lib")

enum INPUT_STATUS {
	Idle = 0,
	Down = 1,
	Press = 2,
	Release = 3,
};

typedef struct _INPUTINFO {
	INPUT_STATUS status;

public:
	_INPUTINFO() :status(INPUT_STATUS::Idle) { }

	VOID Update(BOOL isDown) {

		if (isDown) {
			if (status == INPUT_STATUS::Idle) {
				status = INPUT_STATUS::Down;
				return;
			}

			if (status == INPUT_STATUS::Down) {
				status = INPUT_STATUS::Press;
				return;
			}

			if (status == INPUT_STATUS::Release) {
				status = INPUT_STATUS::Down;
				return;
			}
		}

		if (!isDown) {
			if (status == INPUT_STATUS::Down) {
				status = INPUT_STATUS::Release;
				return;
			}

			if (status == INPUT_STATUS::Press) {
				status = INPUT_STATUS::Release;
				return;
			}

			if (status == INPUT_STATUS::Release) {
				status = INPUT_STATUS::Idle;
				return;
			}
		}
	}

	BOOL isDown() { if (status == INPUT_STATUS::Down) return TRUE; return FALSE; }
	BOOL isPress() { if (status == INPUT_STATUS::Press) return TRUE; return FALSE; }
	BOOL isRelease() { if (status == INPUT_STATUS::Release) return TRUE; return FALSE; }
	BOOL isIdle() { if (status == INPUT_STATUS::Idle) return TRUE; return FALSE; }
}INPUTINFO;

class DXInput : public Singleton<DXInput> {
private:
	IDirectInput8* _directInput;
	IDirectInputDevice8* _keyboard;

	INPUTINFO _inputInfo[256];
	UCHAR _keyboardState[256];

	VOID Shutdown();

public:
	DXInput() : _directInput(nullptr), _keyboard(nullptr) {}
	~DXInput() { Shutdown(); }

	BOOL Init(HINSTANCE hinst, HWND hWnd);
	BOOL Update();

	BOOL isKeyDown(UINT i);
	BOOL isKeyRelease(UINT i);
	BOOL isKeyPressed(UINT i);
	BOOL isKeyIdle(UINT i);

	UINT GetStateSize();
};