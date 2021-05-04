#include "pch.h"
#include ".\UI\DX11.h"
#include ".\UI\DX Input.h"
#include ".\UI\UiWindow.h"
#include ".\UI\PlayerTable.h"
#include ".\UI\Option.h"
#include ".\UI\HotKey.h"
#include ".\Damage Meter\Damage Meter.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace {
	UiWindow* uiWindow = 0;
}

LRESULT CALLBACK MainWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {

	return uiWindow->WndProc(hWnd, msg, wParam, lParam);
}

UiWindow::UiWindow() : _x(0), _y(0), _width(0), _height(0), _swapChain(nullptr), _renderTargetView(nullptr), _hInst(0), _hWnd(0), _imGuiContext(nullptr) {
	uiWindow = this;
}

UiWindow::~UiWindow() {

	if (_swapChain != nullptr) {
		_swapChain->Release();
		_swapChain = nullptr;
	}

	if (_renderTargetView != nullptr) {
		_renderTargetView->Release();
		_renderTargetView = nullptr;
	}

	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

BOOL UiWindow::Init(UINT x, UINT y, UINT width, UINT height) {

	WNDCLASSEX wc;
	_hInst = GetModuleHandle(NULL);

	if (!GetClassInfoEx(_hInst, UI_WINDOW_CLASSNAME, &wc)) {
		wc.cbSize = sizeof(WNDCLASSEX);
		wc.style = CS_HREDRAW | CS_VREDRAW | CS_CLASSDC;
		wc.lpfnWndProc = MainWndProc;
		wc.cbClsExtra = 0;
		wc.cbWndExtra = 0;
		wc.hInstance = _hInst;
		wc.hIcon = NULL;
		wc.hCursor = NULL;
		wc.hbrBackground = NULL;
		wc.lpszMenuName = NULL;
		wc.lpszClassName = UI_WINDOW_CLASSNAME;
		wc.hIconSm = NULL;
	}

	if (!RegisterClassEx(&wc)) {
		Log::WriteLog(const_cast<LPTSTR>(_T("Error in RegisterClassEx")));
		return FALSE;
	}

	string wndName = Log::gen_random(16);

	if ((_hWnd = CreateWindowEx(WS_EX_TOPMOST, wc.lpszClassName, wstring(wndName.begin(), wndName.end()).c_str(), WS_POPUP, x, y, width, height, NULL, NULL, _hInst, NULL)) == NULL) {
		Log::WriteLog(const_cast<LPTSTR>(_T("Error in CreateWindowEx : %x")), GetLastError());
		return FALSE;
	}

	_x = x; _y = y; _width = width, _height = height;

	if (!DIRECTX11.Init()) {
		Log::WriteLog(const_cast<LPTSTR>(_T("Error in DirectX Init")));
		return FALSE;
	}

	
	if (!DXINPUT.Init(_hInst, _hWnd)) {
		Log::WriteLog(const_cast<LPTSTR>(_T("Error in Direct Input Init")));
		return FALSE;
	}
	

	if ((_swapChain = DIRECTX11.CreateSwapChain(_hWnd)) == nullptr) {
		Log::WriteLog(const_cast<LPTSTR>(_T("Error in CreateSwapChain")));
		return FALSE;
	}

	if ((_renderTargetView = DIRECTX11.CreateRenderTarget(_swapChain)) == nullptr) {
		Log::WriteLog(const_cast<LPTSTR>(_T("Error in CreateRenderTarget")));
		return FALSE;
	}

	ShowWindow(_hWnd, SW_SHOWDEFAULT);
	UpdateWindow(_hWnd);

	if (!InitImGUI()) {
		Log::WriteLog(const_cast<LPTSTR>(_T("Error in Init ImGUI")));
		return FALSE;
	}

	return TRUE;
}

BOOL UiWindow::InitImGUI() {

	if (!IMGUI_CHECKVERSION())
		return FALSE;

	_imGuiContext = ImGui::CreateContext();
	
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavNoCaptureKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

	ImGuiStyle& style = ImGui::GetStyle();

	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		style.WindowRounding = 0.0f;
		style.Colors[ImGuiCol_WindowBg].w = 1.0f;
	}

	if (!ImGui_ImplWin32_Init(_hWnd))
		return FALSE;

	if (!ImGui_ImplDX11_Init(DIRECTX11.GetDevice(), DIRECTX11.GetDeviceContext()))
		return FALSE;

	SetFontList();
	UIOPTION.Init();

	return TRUE;
}

BOOL UiWindow::SetFontList() {

	_finddata_t fd;
	const char* path = ".\\Font\\";
	const char* filter = "*.ttf";

	string fontDir(path);
	fontDir.append(filter);
	
	auto handle = _findfirst(fontDir.c_str(), &fd);

	if (handle == -1)
		return FALSE;

	ImGuiIO& io = ImGui::GetIO();
	ImFontConfig config;
	config.OversampleH = 1;
	config.OversampleV = 1;

	do {
		char fontPath[MAX_BUFFER_LENGTH] = { 0 };
		strcat_s(fontPath, path);
		strcat_s(fontPath, fd.name);
		io.Fonts->AddFontFromFileTTF(fontPath, 24, &config, io.Fonts->GetGlyphRangesChineseFull());
	} while (_findnext(handle, &fd) != -1);
	
	_findclose(handle);

	return TRUE;
}

VOID UiWindow::Run() {

	MSG msg = { 0 };

	while (msg.message != WM_QUIT) {

		if (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
			continue;
		}

		Update();
		Sleep(1000 / 30); // fixed high cpu use, limit 30fps
	}
}

VOID UiWindow::Update() {

	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	DXINPUT.Update();
	HOTKEY.Update();
	UIOPTION.Update();
	UpdateMainTable();

	DrawScene();
}

VOID UiWindow::DrawScene() {
	
	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

	ImGui::Render();
	DIRECTX11.GetDeviceContext()->OMSetRenderTargets(1, &_renderTargetView, NULL);
	DIRECTX11.GetDeviceContext()->ClearRenderTargetView(_renderTargetView, (float*)&clear_color);
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	
	ImGuiIO& io = ImGui::GetIO();

	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
	}

	_swapChain->Present(1, 0);
}

LRESULT UiWindow::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {

	if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
		return true;

	switch (msg) {
	case WM_SIZE:
		_width = LOWORD(lParam);
		_height = HIWORD(lParam);
		OnResize();
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

VOID UiWindow::OnResize() {
	
	if (_swapChain == nullptr)
		return;
	
	if (_renderTargetView != nullptr) {
		_renderTargetView->Release();
		_renderTargetView = nullptr;
	}

	_swapChain->ResizeBuffers(0, _width, _height, DXGI_FORMAT_R8G8B8A8_UNORM, 0);

	if ((_renderTargetView = DIRECTX11.CreateRenderTarget(_swapChain)) == nullptr) {
		Log::WriteLog(const_cast<LPTSTR>(_T("Error in CreateRenderTarget")));
		exit(-1);
	}
}

VOID UiWindow::UpdateMainTable() {
	PLAYERTABLE.Update();
}