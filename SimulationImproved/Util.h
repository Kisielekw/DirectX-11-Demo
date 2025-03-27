#pragma once
#include <atlcomcli.h>
#include <DirectXMath.h>
#include <d3d11.h>
#define S_OK static_cast<HRESULT>(0L)

struct DefaultObjectBuffer
{
	DirectX::XMMATRIX transform;
};

struct Shader
{
	CComPtr<ID3DBlob> mVertexBlob;
	CComPtr<ID3DBlob> mPixelBlob;
	CComPtr<ID3D11VertexShader> mVertexShader;
	CComPtr<ID3D11PixelShader> mPixelShader;
	CComPtr<ID3D11InputLayout> mInputLayout;
	UINT mElementNum;
};

struct Light
{
	DirectX::XMVECTOR mPosition;
	DirectX::XMVECTOR mDirection;
	DirectX::XMVECTOR mColourSetting;
};

struct PerFrameLightBuffer
{
	Light mLights[2];
	DirectX::XMMATRIX mView;
	DirectX::XMVECTOR mEyePos;
	int mLightSetting; // 0 = Day, 1 = Torch
};

struct ConstantBuffer
{
	DirectX::XMMATRIX mProjection;
};

struct PerFrameBuffer
{
	DirectX::XMMATRIX mView;
	DirectX::XMVECTOR mEyePos;
};

struct SimpleVertex
{
	DirectX::XMFLOAT3 Pos;
	DirectX::XMFLOAT3 Normal;
	DirectX::XMFLOAT2 TexCoord;
};

struct ComplexVertex
{
	DirectX::XMFLOAT3 Pos;
	DirectX::XMFLOAT3 Normal;
	DirectX::XMFLOAT2 TexCoord;
	DirectX::XMFLOAT3 Tangent;
};

#define DEFAULT "Default"
#define ERROR L"Error"