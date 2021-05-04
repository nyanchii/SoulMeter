#pragma once

#define DIRECTX11 DX11::getInstance()

class DX11 : public Singleton<DX11> {
private:
	ID3D11Device* _d3dDevice;
	ID3D11DeviceContext* _d3dDeviceContext;
	UINT _4xMsaaQuality;
	BOOL _enable4xMsaa;

	BOOL CreateDevice();
	VOID Check4XMSAA();
	VOID CleanDevice();

public:
	DX11();
	~DX11();

	BOOL Init();

	IDXGISwapChain* CreateSwapChain(HWND hWnd);
	ID3D11RenderTargetView* CreateRenderTarget(IDXGISwapChain* swapChain);

	ID3D11Device* GetDevice();
	ID3D11DeviceContext* GetDeviceContext();
};