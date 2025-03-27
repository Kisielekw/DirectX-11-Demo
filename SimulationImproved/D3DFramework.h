#pragma once
// Simulation template, based on the Microsoft DX11 tutorial 04

#include <windows.h>
#include <d3d11_1.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include <atlbase.h>
#include <bitset>
#include <map>
#include <chrono>
#include <unordered_map>

#include "Player.h"

using namespace DirectX;

#define COMPILE_CSO

class D3DFramework final {

	HINSTANCE _hInst = nullptr;
	HWND _hWnd = nullptr;
	D3D_DRIVER_TYPE _driverType = D3D_DRIVER_TYPE_NULL;
	D3D_FEATURE_LEVEL _featureLevel = D3D_FEATURE_LEVEL_11_1;
	CComPtr <ID3D11Device> _pd3dDevice;
	CComPtr <ID3D11Device1> _pd3dDevice1;
	CComPtr <ID3D11DeviceContext> _pImmediateContext;
	CComPtr <ID3D11DeviceContext1> _pImmediateContext1;
	CComPtr <IDXGISwapChain1> _swapChain;
	CComPtr <IDXGISwapChain1> _swapChain1;
	CComPtr <ID3D11RenderTargetView> _pRenderTargetView;
	CComPtr <ID3D11Buffer> _pConstantBuffer;
	CComPtr <ID3D11Buffer> _pPerFrameBuffer;
	CComPtr <ID3D11Buffer> _pPerFrameLightBuffer;
	CComPtr <ID3D11Texture2D> _pDepthStencil;
	CComPtr <ID3D11DepthStencilView> _pDepthStencilView;
	CComPtr <ID3D11DepthStencilState> _pDepthStencilState;
	CComPtr <ID3D11DepthStencilState> _pDepthStencilStateSky;
	XMMATRIX _View = {};
	XMMATRIX _Projection = {};

	std::chrono::time_point<std::chrono::high_resolution_clock> _pPastFrameTime;

	float _pTimeFactor = 1.0f;
	float _pTotalTime = 0.0f;
	float _pCameraOffset = 15;

	std::unique_ptr<Player> _pPlayer;

	std::vector<std::shared_ptr<Object>> _pObjects;
	std::map<std::string, std::shared_ptr<Mesh>> _pMeshes;
	std::map<std::string, Shader> _pShaders;
	std::map<std::string, CComPtr<ID3D11SamplerState>> _pSamplers;
	std::map<std::string, CComPtr<ID3D11ShaderResourceView>> _pTextures;
	std::map<std::string, CComPtr<ID3D11RasterizerState>> _pRasterStates;

	std::bitset<256> _pKeys {0};
	LPPOINT _pMousePos = nullptr;

	Light _pLights[2] = { {}, {} };

	bool _pIsDay = true;
	
	static std::unique_ptr<D3DFramework> _instance;
public:
	// parasoft-begin-suppress ALL "suppress all violations"
	D3DFramework() = default;
	D3DFramework(D3DFramework&) = delete;
	D3DFramework(D3DFramework&&) = delete;
	D3DFramework operator=(const D3DFramework&) = delete;
	D3DFramework operator=(const D3DFramework&&) = delete;
	~D3DFramework();
	// parasoft-end-suppress ALL "suppress all violations"
	static D3DFramework& getInstance() { return *_instance; }

	static LRESULT CALLBACK wndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	HRESULT initWindow(HINSTANCE hInstance, int nCmdShow);
	static HRESULT compileShaderFromFile(const WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut);
	static HRESULT LoadAndCreateShader(ID3D11Device* pd3dDevice, const wchar_t* fileName, Shader& shaderEntry, const D3D11_INPUT_ELEMENT_DESC* layout, const UINT numOfElements);
	HRESULT initDevice();
	void render();
	void debugImGui(const float deltaTime);
	HRESULT LoadTextures(const char* filePath);
	HRESULT LoadObjects(const char* filePath);
	HRESULT LoadShaders(const char* filePath);
	HRESULT ReadConfigFile();
	void Reset();
};