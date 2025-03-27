#include "D3DFramework.h"

#include <algorithm>
#include <directxcolors.h>
#include <cassert>
#include <filesystem>
#include <fstream>
#include <memory>
#include <ranges>

#include "BoxCollider.h"
#include "Resource.h"
#include "DDSTextureLoader.h"
#include "DefaultObject.h"
#include "Rotation.h"
#include "SkyBoxObject.h"
#include "SphereCollider.h"
#include "TexturedObject.h"
#include "Translate.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_impl_dx11.h"
#include "imgui/imgui_internal.h"

std::unique_ptr<D3DFramework> D3DFramework::_instance = std::make_unique<D3DFramework>();

//--------------------------------------------------------------------------------------
// Called every time the application receives a message
//--------------------------------------------------------------------------------------
//parasoft-begin-suppress ALL "suppress all violations"
LRESULT CALLBACK D3DFramework::wndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	PAINTSTRUCT ps;
	std::string msg;
	auto& app = D3DFramework::getInstance();
	
	extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	if (ImGui_ImplWin32_WndProcHandler(hWnd, message, wParam, lParam))
		return true;

	switch (message) {
	case WM_PAINT:
		BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	case WM_KEYDOWN:
		switch (wParam) {
		case 'A':
			msg = "A pressed";
			app._pKeys['A'] = true;
			break;
		case 'D':
			msg = "D pressed";
			app._pKeys['D'] = true;
			break;
		case 'W':
			msg = "W pressed";
			app._pKeys['W'] = true;
			break;
		case 'S':
			msg = "S pressed";
			app._pKeys['S'] = true;
			break;
		case 'T':
			msg = "T pressed";
			app._pIsDay = !app._pIsDay;
			break;
		case 'R':
			msg = "R pressed";
			app.Reset();
			break;
		case ' ':
			msg = "' ' pressed";
			app._pKeys[' '] = true;
			break;
		case VK_OEM_PLUS:
			msg = "'=' pressed";
			app._pKeys['='] = true;
			break;
		case VK_OEM_MINUS:
			msg = "'-' pressed";
			app._pKeys['-'] = true;
			break;
		case VK_OEM_COMMA:
			msg = "'<' pressed";
			app._pKeys['<'] = true;
			break;
		case VK_OEM_PERIOD:
			msg = "'>' pressed";
			app._pKeys['>'] = true;
			break;
		case VK_ESCAPE:
			PostQuitMessage(0);
			break;
		default:
			break;
		}
		break;

	case WM_KEYUP:
		switch (wParam) {
		case 'A':
			msg = "A released";
			app._pKeys['A'] = false;
			break;
		case 'D':
			msg = "D released";
			app._pKeys['D'] = false;
			break;
		case 'W':
			msg = "W released";
			app._pKeys['W'] = false;
			break;
		case 'S':
			msg = "S released";
			app._pKeys['S'] = false;
			break;
		case ' ':
			msg = "' ' released";
			app._pKeys[' '] = false;
			break;
		case VK_OEM_PLUS:
			msg = "'=' released";
			app._pKeys['='] = false;
			break;
		case VK_OEM_MINUS:
			msg = "'-' released";
			app._pKeys['-'] = false;
			break;
		case VK_OEM_COMMA:
			msg = "'<' released";
			app._pKeys['<'] = false;
			break;
		case VK_OEM_PERIOD:
			msg = "'>' released";
			app._pKeys['>'] = false;
			break;
		default:
			break;
		}

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;
}


//--------------------------------------------------------------------------------------
// Register class and create window
//--------------------------------------------------------------------------------------
HRESULT D3DFramework::initWindow(HINSTANCE hInstance, int nCmdShow) {
	// Register class
	WNDCLASSEX wcex;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = wndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, reinterpret_cast<LPCTSTR>(IDI_SIMULATION));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);
	wcex.lpszMenuName = nullptr;
	wcex.lpszClassName = L"Starter Template";
	wcex.hIconSm = LoadIcon(wcex.hInstance, reinterpret_cast<LPCTSTR>(IDI_SIMULATION));
	if (!RegisterClassEx(&wcex))
		return E_FAIL;

	// Create window
	_hInst = hInstance;
	RECT rc = { 0, 0, 800, 600 };
	AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);
	_hWnd = CreateWindow(L"Starter Template", L"Direct3D 11 Simulation",
		WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
		CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, nullptr, nullptr, hInstance,
		nullptr);
	if (!_hWnd)
		return E_FAIL;

	ShowWindow(_hWnd, nCmdShow);

	return S_OK;
}


//--------------------------------------------------------------------------------------
// Helper for compiling shaders with D3DCompile
//
// With VS 11, we could load up prebuilt .cso files instead...
//--------------------------------------------------------------------------------------
HRESULT D3DFramework::compileShaderFromFile(const WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut)
{
	auto dwShaderFlags = static_cast<DWORD>(D3DCOMPILE_ENABLE_STRICTNESS);
#ifdef _DEBUG
	// Set the D3DCOMPILE_DEBUG flag to embed debug information in the shaders.
	// Setting this flag improves the shader debugging experience, but still allows 
	// the shaders to be optimized and to run exactly the way they will run in 
	// the release configuration of this program.
	dwShaderFlags |= D3DCOMPILE_DEBUG;

	// Disable optimizations to further improve shader debugging
	dwShaderFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

	CComPtr <ID3DBlob> pErrorBlob;
	const auto hr = D3DCompileFromFile(szFileName, nullptr, nullptr, szEntryPoint, szShaderModel, dwShaderFlags, 0, ppBlobOut, &pErrorBlob);
	if (FAILED(hr)) {
		if (pErrorBlob)
			OutputDebugStringA(static_cast<const char*>(pErrorBlob->GetBufferPointer()));
		return hr;
	}

	return S_OK;
}

// parasoft-end-suppress ALL "suppress all violations"

HRESULT D3DFramework::LoadAndCreateShader(ID3D11Device* pd3dDevice, const wchar_t* fileName, Shader& shaderEntry, const D3D11_INPUT_ELEMENT_DESC* layout, const UINT numOfElements)
{
	HRESULT hr = compileShaderFromFile(fileName, "VS", "vs_5_0", &shaderEntry.mVertexBlob);
	if (FAILED(hr)) {
		return hr;
	}

	hr = pd3dDevice->CreateVertexShader(
		shaderEntry.mVertexBlob->GetBufferPointer(),
		shaderEntry.mVertexBlob->GetBufferSize(),
		nullptr,
		&shaderEntry.mVertexShader);
	if (FAILED(hr)) {
		return hr;
	}

	hr = pd3dDevice->CreateInputLayout(layout, numOfElements, 
		shaderEntry.mVertexBlob->GetBufferPointer(), 
		shaderEntry.mVertexBlob->GetBufferSize(), &shaderEntry.mInputLayout);
	if (FAILED(hr))
		return hr;

	hr = compileShaderFromFile(fileName, "PS", "ps_5_0", &shaderEntry.mPixelBlob);
	if (FAILED(hr)) {
		return hr;
	}

	hr = pd3dDevice->CreatePixelShader(
		shaderEntry.mPixelBlob->GetBufferPointer(),
		shaderEntry.mPixelBlob->GetBufferSize(),
		nullptr,
		&shaderEntry.mPixelShader);
	if (FAILED(hr)) {
		return hr;
	}

	shaderEntry.mElementNum = numOfElements;

	return S_OK;
}

//--------------------------------------------------------------------------------------
// Create Direct3D device and swap chain
//--------------------------------------------------------------------------------------
HRESULT D3DFramework::initDevice()
{
// parasoft-begin-suppress ALL "suppress all violations"
	auto hr = S_OK;

	RECT rc;
	GetClientRect(_hWnd, &rc);
	UINT width = rc.right - rc.left;
	UINT height = rc.bottom - rc.top;

	UINT createDeviceFlags = 0;
#ifdef _DEBUG
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	D3D_DRIVER_TYPE driverTypes[] = {
		D3D_DRIVER_TYPE_HARDWARE,
		D3D_DRIVER_TYPE_WARP,
		D3D_DRIVER_TYPE_REFERENCE,
	};
	auto numDriverTypes = ARRAYSIZE(driverTypes);

	D3D_FEATURE_LEVEL featureLevels[] = {
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
	};
	auto numFeatureLevels = static_cast<UINT>(ARRAYSIZE(featureLevels));

	for (UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; driverTypeIndex++) {
		_driverType = driverTypes[driverTypeIndex];
		hr = D3D11CreateDevice(nullptr, _driverType, nullptr, createDeviceFlags, featureLevels, numFeatureLevels, D3D11_SDK_VERSION, &_pd3dDevice, &_featureLevel, &_pImmediateContext);

		if (hr == E_INVALIDARG)
			// DirectX 11.0 platforms will not recognize D3D_FEATURE_LEVEL_11_1 so we need to retry without it
			hr = D3D11CreateDevice(nullptr, _driverType, nullptr, createDeviceFlags, &featureLevels[1], numFeatureLevels - 1, D3D11_SDK_VERSION, &_pd3dDevice, &_featureLevel, &_pImmediateContext);

		if (SUCCEEDED(hr))
			break;
	}
	if (FAILED(hr))
		return hr;

	// Obtain DXGI factory from device (since we used nullptr for pAdapter above)
	CComPtr <IDXGIFactory1> dxgiFactory;
	{
		CComPtr <IDXGIDevice> dxgiDevice;
		hr = _pd3dDevice->QueryInterface(__uuidof(IDXGIDevice), reinterpret_cast<void**>(&dxgiDevice));
		if (SUCCEEDED(hr)) {
			CComPtr <IDXGIAdapter> adapter;
			hr = dxgiDevice->GetAdapter(&adapter);
			if (SUCCEEDED(hr)) {
				hr = adapter->GetParent(__uuidof(IDXGIFactory1), reinterpret_cast<void**>(&dxgiFactory));
			}
		}
	}
	if (FAILED(hr))
		return hr;

	// Create swap chain
	CComPtr <IDXGIFactory2> dxgiFactory2;
	hr = dxgiFactory->QueryInterface(__uuidof(IDXGIFactory2), reinterpret_cast<void**>(&dxgiFactory2));

	// DirectX 11.1 or later
	hr = _pd3dDevice->QueryInterface(__uuidof(ID3D11Device1), reinterpret_cast<void**>(&_pd3dDevice1));
	if (SUCCEEDED(hr)) {
		(void)_pImmediateContext->QueryInterface(__uuidof(ID3D11DeviceContext1), reinterpret_cast<void**>(&_pImmediateContext1));
	}

	DXGI_SWAP_CHAIN_DESC1 sd{};
	sd.Width = width;
	sd.Height = height;
	sd.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.BufferCount = 1;

	hr = dxgiFactory2->CreateSwapChainForHwnd(_pd3dDevice, _hWnd, &sd, nullptr, nullptr, &_swapChain1);
	if (SUCCEEDED(hr)) {
		hr = _swapChain1->QueryInterface(__uuidof(IDXGISwapChain), reinterpret_cast<void**>(&_swapChain));
	}

	// Note this tutorial doesn't handle full-screen swap chains so we block the ALT+ENTER shortcut
	dxgiFactory->MakeWindowAssociation(_hWnd, DXGI_MWA_NO_ALT_ENTER);

	if (FAILED(hr))
		return hr;

	// parasoft-end-suppress ALL "suppress all violations"

	// Create a render target view
	CComPtr <ID3D11Texture2D> pBackBuffer;
	hr = _swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&pBackBuffer));
	if (FAILED(hr))
		return hr;

	hr = _pd3dDevice->CreateRenderTargetView(pBackBuffer, nullptr, &_pRenderTargetView);
	if (FAILED(hr))
		return hr;

	// Initialize the Depth Stencil
	D3D11_TEXTURE2D_DESC descDepth{};
	descDepth.Height = height;
	descDepth.Width = width;
	descDepth.MipLevels = 1;
	descDepth.ArraySize = 1;
	descDepth.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	descDepth.SampleDesc.Count = 1;
	descDepth.SampleDesc.Quality = 0;
	descDepth.Usage = D3D11_USAGE_DEFAULT;
	descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;

	hr = _pd3dDevice->CreateTexture2D(&descDepth, nullptr, &_pDepthStencil);
	if (FAILED(hr))
		return hr;

	D3D11_DEPTH_STENCIL_VIEW_DESC dsDesc{};
	dsDesc.Format = descDepth.Format;
	dsDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	dsDesc.Texture2D.MipSlice = 0;

	hr = _pd3dDevice->CreateDepthStencilView(_pDepthStencil, &dsDesc, &_pDepthStencilView);
	if (FAILED(hr))
		return hr;

	D3D11_DEPTH_STENCIL_DESC dsd{};
	dsd.DepthEnable = TRUE;
	dsd.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	dsd.DepthFunc = D3D11_COMPARISON_LESS;
	dsd.StencilEnable = FALSE;

	hr = _pd3dDevice->CreateDepthStencilState(&dsd, &_pDepthStencilState);
	if (FAILED(hr))
		return hr;

	_pImmediateContext->OMSetDepthStencilState(_pDepthStencilState, 1);
	_pImmediateContext->OMSetRenderTargets(1, &_pRenderTargetView.p, _pDepthStencilView);

	dsd.DepthEnable = false;
	hr = _pd3dDevice->CreateDepthStencilState(&dsd, &_pDepthStencilStateSky);
	if (FAILED(hr))
		return hr;

// parasoft-begin-suppress ALL "suppress all violations"

	// Setup the viewport
	D3D11_VIEWPORT vp;
	vp.Width = static_cast<FLOAT>(width);
	vp.Height = static_cast<FLOAT>(height);
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	_pImmediateContext->RSSetViewports(1, &vp);

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls

	// Setup Platform/Renderer backends
	ImGui_ImplWin32_Init(_hWnd);
	ImGui_ImplDX11_Init(_pd3dDevice.p, _pImmediateContext.p);

// parasoft-end-suppress ALL "suppress all violations"

#ifdef COMPILE_CSO
	hr = LoadShaders("Shaders");
	if(FAILED(hr))
	{
		MessageBox(nullptr, L"The FX file cannot be compiled.  Please ensure it exists and the path is correct.", ERROR, MB_OK);
		return hr;
	}

// parasoft-begin-suppress ALL "suppress all violations"
#else
	{
		const std::string fileName{ "..\\Debug\\Simulation_VS.cso" };
		std::ifstream fin(fileName, std::ios::binary);
		if (!fin) {
			MessageBox(nullptr, L"The CSO file cannot be found.", ERROR, MB_OK);
			return E_FAIL;
		}
		std::vector<unsigned char> byteCode(std::istreambuf_iterator<char>(fin), {});

		hr = _pd3dDevice->CreateVertexShader(&byteCode[0], byteCode.size(), nullptr, &_pVertexShader);
		if (FAILED(hr)) {
			return hr;
		}

		// Create the input layout
		hr = _pd3dDevice->CreateInputLayout(layout, numElements, &byteCode[0], byteCode.size(), &_pVertexLayout);
		if (FAILED(hr))
			return hr;
	}

	{
		const std::string fileName{ "..\\Debug\\Simulation_PS.cso" };
		std::ifstream fin(fileName, std::ios::binary);
		if (!fin) {
			MessageBox(nullptr, L"The CSO file cannot be found.", ERROR, MB_OK);
			return E_FAIL;
		}
		std::vector<unsigned char> byteCode(std::istreambuf_iterator<char>(fin), {});

		hr = _pd3dDevice->CreatePixelShader(&byteCode[0], byteCode.size(), nullptr, &_pPixelShader);
		if (FAILED(hr)) {
			return hr;
		}
	}
#endif	
	// parasoft-end-suppress ALL "suppress all violations"
	// Initialize the projection matrix
	_Projection = XMMatrixPerspectiveFovLH(XM_PIDIV2, width / static_cast<FLOAT>(height), 0.01f, 100.0f);

	ConstantBuffer cb = { XMMatrixTranspose(_Projection) };

	D3D11_BUFFER_DESC bd = {};
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(ConstantBuffer);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA data = {};
	data.pSysMem = &cb;

	hr = _pd3dDevice->CreateBuffer(&bd, &data, &_pConstantBuffer);
	if (FAILED(hr))
		return hr;

	bd.ByteWidth = sizeof(PerFrameBuffer); 
	hr = _pd3dDevice->CreateBuffer(&bd, nullptr, &_pPerFrameBuffer);
	if (FAILED(hr))
		return hr;

	bd.ByteWidth = sizeof(PerFrameLightBuffer); 
	hr = _pd3dDevice->CreateBuffer(&bd, nullptr, &_pPerFrameLightBuffer);
	if (FAILED(hr))
		return hr;

	_pRasterStates.try_emplace(DEFAULT);

	D3D11_RASTERIZER_DESC rasterDesc {};
	rasterDesc.FrontCounterClockwise = FALSE;
	rasterDesc.CullMode = D3D11_CULL_BACK;
	rasterDesc.FillMode = D3D11_FILL_SOLID;
	rasterDesc.ScissorEnable = false;
	rasterDesc.DepthBias = 0;
	rasterDesc.DepthBiasClamp = 0.0f;
	rasterDesc.DepthClipEnable = true;
	rasterDesc.MultisampleEnable = false;
	rasterDesc.SlopeScaledDepthBias = 0.0f;
	hr = _pd3dDevice->CreateRasterizerState(&rasterDesc, &_pRasterStates[DEFAULT]);
	if (FAILED(hr))
		return hr;

	const char* const sB = "SkyBox";
	_pRasterStates.try_emplace(sB);

	rasterDesc.DepthClipEnable = false;
	rasterDesc.CullMode = D3D11_CULL_FRONT;
	hr = _pd3dDevice->CreateRasterizerState(&rasterDesc, &_pRasterStates[sB]);
	if (FAILED(hr))
		return hr;

	hr = LoadObjects("Objects");
	if (FAILED(hr))
	{
		MessageBox(nullptr, L"The OBJ file cannot be compiled.  Please ensure it exists and the path is correct.", ERROR, MB_OK);
		return hr;
	}

	hr = LoadTextures("Textures");
	if (FAILED(hr))
	{
		MessageBox(nullptr, L"The DDS file cannot be compiled.  Please ensure it exists and the path is correct.", ERROR, MB_OK);
		return hr;
	}

	D3D11_SAMPLER_DESC sampleDesc{};
	sampleDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampleDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampleDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampleDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampleDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;

	_pSamplers.try_emplace(DEFAULT);
	hr = _pd3dDevice->CreateSamplerState(&sampleDesc, &_pSamplers[DEFAULT]);
	if (FAILED(hr))
		return hr;

	ReadConfigFile();

	return S_OK;
}


//--------------------------------------------------------------------------------------
// Clean up the objects we've created
//--------------------------------------------------------------------------------------
D3DFramework::~D3DFramework() {
	try {
		if (_pImmediateContext)
			_pImmediateContext->ClearState();

		//Shutdown Imgui 
		ImGui_ImplDX11_Shutdown();
		ImGui_ImplWin32_Shutdown();
		ImGui::DestroyContext();
	}
	catch (...) {

	}
}


//--------------------------------------------------------------------------------------
// Render a frame
//--------------------------------------------------------------------------------------
void D3DFramework::render() {
	if (_pPastFrameTime.time_since_epoch().count() == 0)
	{
		_pPastFrameTime = std::chrono::high_resolution_clock::now();
	}

	const auto frameTime = std::chrono::high_resolution_clock::now();
	const float deltaTime = std::chrono::duration<float>(frameTime - _pPastFrameTime).count();
	const float frameTimeFactor = deltaTime * _pTimeFactor;
	_pTotalTime += frameTimeFactor;

	if(_pKeys['='])
		_pCameraOffset -= 2 * deltaTime;
	if (_pKeys['-'])
		_pCameraOffset += 2 * deltaTime;

	_pCameraOffset = std::clamp(_pCameraOffset, 2.0f, 15.0f);

	if (_pKeys['<'])
		_pTimeFactor -= 1 * deltaTime;
	if (_pKeys['>'])
		_pTimeFactor += 1 * deltaTime;

	_pTimeFactor = std::clamp(_pTimeFactor, 0.0f, 10.0f);

	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	debugImGui(deltaTime);

	_pPlayer->update(frameTimeFactor, _pKeys, _pObjects);
	const XMVECTOR eyePos = XMVectorAdd(_pObjects[1]->position(), XMVectorSet(0, 0, -_pCameraOffset, 0));

	//
	// Clear the back buffer
	//
	_pImmediateContext->ClearRenderTargetView(_pRenderTargetView, Colors::MidnightBlue);
	_pImmediateContext->ClearDepthStencilView(_pDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	_View = XMMatrixLookAtLH(eyePos, _pObjects[1]->position(), XMVectorSet(0, 1, 0, 0));

	_pLights[1].mPosition = _pObjects[2]->position();

	const float x = cosf(_pPlayer->getLookAt()) * _pPlayer->getLastMoveDir();
	const float y = sinf(_pPlayer->getLookAt());
	_pLights[1].mDirection = XMVectorSet(x, y, 0, 0);

	PerFrameLightBuffer flb = {};
	flb.mEyePos = eyePos;
	flb.mLightSetting = _pIsDay ? 0 : 1;
	flb.mLights[0] = _pLights[0];
	flb.mLights[1] = _pLights[1];
	flb.mView = XMMatrixTranspose(_View);
	_pImmediateContext->UpdateSubresource(_pPerFrameLightBuffer, 0, nullptr, &flb, 0, 0);

	PerFrameBuffer fb = {};
	fb.mEyePos = eyePos;
	fb.mView = XMMatrixTranspose(_View);
	_pImmediateContext->UpdateSubresource(_pPerFrameBuffer, 0, nullptr, &fb, 0, 0);

	for (const auto& object : _pObjects)
	{
		object->animate(frameTimeFactor);
		if (object->name() == "Sky")
			object->Draw(_pImmediateContext, _pPerFrameBuffer, _pConstantBuffer);
		else
			object->Draw(_pImmediateContext, _pPerFrameLightBuffer, _pConstantBuffer);
	}

	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	//
	// Present our back buffer to our front buffer
	//
	_swapChain->Present(0, 0);

	_pPastFrameTime = frameTime;
}

void D3DFramework::debugImGui(const float deltaTime)
{
	ImGui::Begin("Debug Controls");
	ImGui::Text("Frame Time: %f", deltaTime);
	ImVec2 mousePos = ImGui::GetMousePos();
	ImGui::Text("Mouse Position: %f, %f", mousePos.x, mousePos.y);
	ImGui::SliderFloat("Time Factor", &_pTimeFactor, 0, 10);

	if (ImGui::CollapsingHeader("Player"))
	{
		float friction = _pPlayer->getFriction();
		float speed = _pPlayer->getSpeed();
		float jumpPow = _pPlayer->getJumpPower();
		float lookAt = _pPlayer->getLookAt();
		XMVECTOR position = _pObjects[1]->position();
		XMVECTOR velocity = _pPlayer->getVelocity();

		ImGui::DragFloat("Friction", &friction);
		ImGui::DragFloat("Speed", &speed);
		ImGui::DragFloat("Jump Power", &jumpPow);
		ImGui::DragFloat("Look At Angle", &lookAt);
		ImGui::DragFloat3("Position", position.m128_f32);
		ImGui::DragFloat3("Velocity", velocity.m128_f32);

		_pPlayer->setFriction(friction);
		_pPlayer->setSpeed(speed);
		_pPlayer->setJumpPower(jumpPow);
		_pPlayer->getPlayerObject().setPosition(position);
	}
	if (ImGui::CollapsingHeader("Camera"))	
	{
		ImGui::SliderFloat("Eye Offset", &_pCameraOffset, 2, 15);
	}
	if (ImGui::CollapsingHeader("Sun Light"))
	{
		ImGui::ColorEdit3("Sun Colour", _pLights[0].mColourSetting.m128_f32);
		ImGui::SliderFloat3("Sun Direction", _pLights[0].mDirection.m128_f32, -1, 1);
	}
	if (ImGui::CollapsingHeader("Torch Light"))
	{
		ImGui::ColorEdit3("Torch Colour", _pLights[1].mColourSetting.m128_f32);
		ImGui::DragFloat3("Torch Position", _pLights[1].mPosition.m128_f32);
		ImGui::DragFloat3("Torch Direction", _pLights[1].mDirection.m128_f32);
	}
	if (ImGui::CollapsingHeader("Objects"))
	{
		if(ImGui::TreeNode("Add Object"))
		{
			static char name[128] = "Object";
			static int mesh = 0;
			static int shader = 0;
			static int raster = 0;
			static int texture = 0;
			static float position[3] = { 0, 0, 0 };
			static float scale[3] = { 1, 1, 1 };
			static float rotation[3] = { 0, 0, 0 };

			ImGui::InputText("Name", name, 128);
			ImGui::Combo("Mesh", &mesh, "Cube\0Plane\0Sphere\0");
			ImGui::Combo("Shader", &shader, "Default\0Textured\0SkyBox\0ToonShader\0");
			ImGui::Combo("Raster", &raster, "Default\0SkyBox\0");
			ImGui::Combo("Texture", &texture, "Arrow\0CubeMap\0");
			ImGui::DragFloat3("Position", position);
			ImGui::DragFloat3("Scale", scale);
			ImGui::DragFloat3("Rotation", rotation);

			if (ImGui::Button("Add"))
			{
				const auto meshName = mesh == 0 ? "Cube" : mesh == 1 ? "Plane" : "Sphere";
				const auto shaderName = shader == 0 ? "Default.fx" : shader == 1 ? "Textured.fx" : shader == 2 ? "SkyBox.fx" : "ToonShader.fx";
				const auto rasterName = raster == 0 ? DEFAULT : "SkyBox";
				const auto textureName = texture == 0 ? "Arrow.dds" : "CubeMap.dds";

				const auto mesh = _pMeshes.find(meshName);
				const auto shader = _pShaders.find(shaderName);
				const auto raster = _pRasterStates.find(rasterName);
				const auto texture = _pTextures.find(textureName);

				_pObjects.emplace_back(std::make_shared<DefaultObject>(name, _pd3dDevice, mesh->second,
					shader->second, raster->second));
				_pObjects.back()->setPosition(XMVectorSet(position[0], position[1], position[2], 1));
				_pObjects.back()->setScale(XMVectorSet(scale[0], scale[1], scale[2], 1));
				_pObjects.back()->setRotation(XMVectorSet(rotation[0], rotation[1], rotation[2], 1));
			}
			ImGui::TreePop();
		}

		for (int i = 0; i < _pObjects.size(); i++)
		{
			std::shared_ptr<Object> object = _pObjects[i];
			if (object->name() == "Player")
				continue;
			if (ImGui::TreeNode(object->name().c_str()))
			{
				XMVECTOR position = object->position();
				XMVECTOR scale = object->scale();
				XMVECTOR rotation = object->rotation();
				ImGui::Text("Object Type: %s", object->objectType());
				ImGui::Text("Vertex Count: %i", object->vertexCount());
				ImGui::DragFloat3("Position", position.m128_f32);
				ImGui::DragFloat3("Scale", scale.m128_f32);
				ImGui::DragFloat3("Rotation", rotation.m128_f32);

				object->setPosition(position);
				object->setRotation(rotation);
				object->setScale(scale);
				if(ImGui::Button("Delete"))
				{
					_pObjects.erase(std::ranges::find(_pObjects, object));
					i--;
				}
				if(ImGui::TreeNode("Collider"))
				{
					if (object->hasCollider())
					{
						if (ImGui::Button("Delete Collider"))
							object->removeCollider();
					}
					else
					{
						if (ImGui::Button("Add Sphere Collider"))
							object->setCollider(std::make_unique<SphereCollider>());
						if (ImGui::Button("Add Box Collider"))
							object->setCollider(std::make_unique<BoxCollider>());
					}
					ImGui::TreePop();
				}
				if(ImGui::TreeNode("Animation"))
				{
					if (object->hasAnimation())
					{
						if (ImGui::Button("Delete Animation"))
							object->removeAnimation();
					}
					else
					{
						static XMVECTOR animationVector{};
						static float animationSpeed = 0;
						ImGui::DragFloat3("Animation Vector", animationVector.m128_f32);
						ImGui::DragFloat("Speed", &animationSpeed);
						if (ImGui::Button("Add Translation Animation"))
						{
							object->setAnimation(std::make_unique<Translate>(animationVector, animationSpeed));
						}
						if (ImGui::Button("Add Rotation Animation"))
						{
							object->setAnimation(std::make_unique<Rotation>(animationVector, animationSpeed));
						}
					}
					ImGui::TreePop();
				}

				ImGui::TreePop();
			}
		}
	}

	ImGui::End();

}

HRESULT D3DFramework::LoadShaders(const char* filePath)
{
	const D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};

	for (const auto& entry : std::filesystem::directory_iterator(filePath)) {
		if (std::filesystem::is_regular_file(entry.status())) {
			if (entry.path().extension() != ".fx")
				continue;
			_pShaders.try_emplace(entry.path().filename().string());
			const auto hr =LoadAndCreateShader(_pd3dDevice, entry.path().c_str(), _pShaders[entry.path().filename().string()], layout, 3);
			if(FAILED(hr))
				return hr;
		}
	}

	return S_OK;
}


HRESULT D3DFramework::LoadObjects(const char* filePath)
{
	for (const auto& entry : std::filesystem::directory_iterator(filePath)) {
		if (std::filesystem::is_regular_file(entry.status())) {
			if (entry.path().extension() != ".obj")
				continue;
			const auto hr = Mesh::LoadOBJFile(entry.path().string().c_str(), _pd3dDevice, _pShaders[DEFAULT], _pMeshes);
			if (FAILED(hr))
				return hr;
		}
	}

	return S_OK;
}

HRESULT D3DFramework::LoadTextures(const char* filePath)
{
	for (const auto& entry : std::filesystem::directory_iterator(filePath)) {
		if (std::filesystem::is_regular_file(entry.status())) {
			if (entry.path().extension() != ".dds")
				continue;
			_pTextures.try_emplace(entry.path().filename().string());
			const auto hr = CreateDDSTextureFromFile(_pd3dDevice, entry.path().c_str(), nullptr, &_pTextures[entry.path().filename().string()]);
			if (FAILED(hr))
				return hr;
		}
	}

	return S_OK;
}

void D3DFramework::Reset()
{
	_pObjects.clear();
	_pIsDay = true;
	_pTimeFactor = 1;
	_pCameraOffset = 15;
	ReadConfigFile();
}

HRESULT D3DFramework::ReadConfigFile()
{
	std::ifstream file("GameConfig.config");
	std::string key;
	int lightNum = 0;
	while (file >> key)
	{
		if(key == "O")
		{
			char modifier;
			file >> modifier;
			if(modifier == '+')
			{
				std::string type;
				file >> type;
				if(type == "setCollider")
				{
					file >> type;
					if (type == "SphereCollider")
						_pObjects.back()->setCollider(std::make_unique<SphereCollider>());
					else if (type == "BoxCollider")
						_pObjects.back()->setCollider(std::make_unique<BoxCollider>());
				}
				else if(type == "setPosition")
				{
					XMVECTOR position{};
					file >> position.m128_f32[0] >> position.m128_f32[1] >> position.m128_f32[2];
					_pObjects.back()->setPosition(position);
				}
				else if(type == "setRotation")
				{
					XMVECTOR rotation{};
					file >> rotation.m128_f32[0] >> rotation.m128_f32[1] >> rotation.m128_f32[2];
					_pObjects.back()->setRotation(rotation);
				}
				else if(type == "setScale")
				{
					XMVECTOR scale{};
					file >> scale.m128_f32[0] >> scale.m128_f32[1] >> scale.m128_f32[2];
					_pObjects.back()->setScale(scale);
				}
				else if(type == "setAnimation")
				{
					std::string animationType;
					float speed;
					file >> animationType >> speed;
					XMVECTOR amount{};
					file >> amount.m128_f32[0] >> amount.m128_f32[1] >> amount.m128_f32[2];
					if(animationType == "Translation")
					{
						_pObjects.back()->setAnimation(std::make_unique<Translate>(amount, speed));
					}
					else if (animationType == "Rotation")
					{
						_pObjects.back()->setAnimation(std::make_unique<Rotation>(amount, speed));
					}
				}

				continue;
			}

			file.putback(modifier);
			std::string type, name, mesh, shader, raster;
			file >> type >> name >> mesh >> shader >> raster;
			if (type == "SkyBox")
			{
				std::string texture;
				file >> texture;
				_pObjects.emplace_back(std::make_shared<SkyBoxObject>(_pMeshes[mesh], _pShaders[shader],
					_pRasterStates[raster], _pDepthStencilState, _pDepthStencilStateSky, _pSamplers[DEFAULT],
					_pTextures[texture]));
			}
			else if (type == DEFAULT)
				_pObjects.emplace_back(std::make_shared<DefaultObject>(name.c_str(), _pd3dDevice, _pMeshes[mesh], _pShaders[shader],
					_pRasterStates[raster]));
			else if (type == "Textured")
			{
				std::string texture;
				file >> texture;
				_pObjects.emplace_back(std::make_shared<TexturedObject>(name.c_str(), _pd3dDevice, _pMeshes[mesh], _pShaders[shader],
					_pRasterStates[raster], _pSamplers[DEFAULT], _pTextures[texture]));
			}
		}
		else if(key == "P")
		{
			int pObject, lObject;
			file >> pObject >> lObject;
			_pPlayer = std::make_unique<Player>(_pObjects[pObject], _pObjects[lObject]);
		}
		else if(key == "L")
		{
			std::string type;
			XMVECTOR colour{}, direction{};
			file >> type >> colour.m128_f32[0] >> colour.m128_f32[1] >> colour.m128_f32[2] >> direction.m128_f32[0] >> direction.m128_f32[1] >> direction.m128_f32[2];
			colour.m128_f32[3] = type == "Directional" ? 0 : 1;
			_pLights[lightNum++] = { XMVectorSet(0,0,0,0), direction, colour };
		}
	}

	return S_OK;
}
