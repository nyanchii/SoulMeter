#include "pch.h"
#include ".\UI\DX11.h"

DX11::DX11() : _4xMsaaQuality(0), _enable4xMsaa(FALSE), _d3dDevice(nullptr), _d3dDeviceContext(nullptr) {

}

DX11::~DX11() {
	CleanDevice();
}

BOOL DX11::CreateDevice() {

	if (_d3dDevice)
		return TRUE;

	UINT createDeviceFlags = 0;

#if defined(DEBUG) || defined(_DEBUG)
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	D3D_FEATURE_LEVEL featureLevel;
	D3D_FEATURE_LEVEL featureLevelArray[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0 };
	
	HRESULT hr = D3D11CreateDevice(0,
		D3D_DRIVER_TYPE_HARDWARE,
		0,
		createDeviceFlags,
		featureLevelArray,
		2,
		D3D11_SDK_VERSION,
		&_d3dDevice,
		&featureLevel,
		&_d3dDeviceContext
	);

	if (FAILED(hr)) {
		Log::WriteLog(const_cast<LPTSTR>(_T("[DirectX11] [CreateDevice FAILED]\n")));
		return FALSE;
	}

	return TRUE;
}

VOID DX11::Check4XMSAA() {

	if (_d3dDevice == nullptr)
		return;

	HRESULT hr = _d3dDevice->CheckMultisampleQualityLevels(DXGI_FORMAT_R8G8B8A8_UNORM, 4, &_4xMsaaQuality);

	if (FAILED(hr) || _4xMsaaQuality < 1) {
		_enable4xMsaa = FALSE;
		return;
	}

	_enable4xMsaa = TRUE;
}

VOID DX11::CleanDevice() {

	if (_d3dDeviceContext != nullptr) {
		_d3dDeviceContext->ClearState();
		_d3dDeviceContext->Release();
		_d3dDeviceContext = nullptr;
	}

	if (_d3dDevice != nullptr) {
		_d3dDevice->Release();
		_d3dDevice = nullptr;
	}
}

BOOL DX11::Init() {
	
	if (!CreateDevice()) {
		CleanDevice();
		return FALSE;
	}

	Check4XMSAA();

	return TRUE;
}

IDXGISwapChain* DX11::CreateSwapChain(HWND hWnd) {

	if (_d3dDevice == nullptr)
		return nullptr;

	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.BufferDesc.Width = 0;
	sd.BufferDesc.Height = 0;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	if (_enable4xMsaa) {
		sd.SampleDesc.Count = 4;
		sd.SampleDesc.Quality = _4xMsaaQuality - 1;
	}
	else {
		sd.SampleDesc.Count = 1;
		sd.SampleDesc.Quality = 0;
	}

	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.BufferCount = 2;
	sd.OutputWindow = hWnd;
	sd.Windowed = TRUE;
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	IDXGIDevice* dxgiDevice = nullptr;
	IDXGIAdapter* dxgiAdapter = nullptr;
	IDXGIFactory* dxgiFactory = nullptr;
	IDXGISwapChain* swapChain = nullptr;

	do {
		if (FAILED(_d3dDevice->QueryInterface(__uuidof(IDXGIDevice), (void**)&dxgiDevice))) 
			break;
		
		if (FAILED(dxgiDevice->GetParent(__uuidof(IDXGIAdapter), (void**)&dxgiAdapter)))
			break;

		if (FAILED(dxgiAdapter->GetParent(__uuidof(IDXGIFactory), (void**)&dxgiFactory)))
			break;

		if (FAILED(dxgiFactory->CreateSwapChain(_d3dDevice, &sd, &swapChain)))
			break;

	} while (FALSE);

	if (dxgiDevice != nullptr)
		dxgiDevice->Release();

	if (dxgiAdapter != nullptr)
		dxgiAdapter->Release();

	if (dxgiFactory != nullptr)
		dxgiFactory->Release();

	return swapChain;
}

ID3D11RenderTargetView* DX11::CreateRenderTarget(IDXGISwapChain* swapChain) {

	if (swapChain == nullptr || _d3dDevice == nullptr)
		return nullptr;

	ID3D11RenderTargetView* renderTargetView = nullptr;
	ID3D11Texture2D* backBuffer = nullptr;

	do {
		if(FAILED(swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&backBuffer))))
			break;

		_d3dDevice->CreateRenderTargetView(backBuffer, 0, &renderTargetView);
	} while (FALSE);

	if(backBuffer != nullptr)
		backBuffer->Release();

	return renderTargetView;
}

ID3D11Device* DX11::GetDevice() {
	return _d3dDevice;
}

ID3D11DeviceContext* DX11::GetDeviceContext() {
	return _d3dDeviceContext;
}