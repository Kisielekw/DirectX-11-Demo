#pragma once
#include "Object.h"

class DefaultObject final : public Object
{
public:
	DefaultObject(const char* name, const CComPtr<ID3D11Device>& device, const std::shared_ptr<Mesh>& mesh,
		const Shader& shader, const CComPtr<ID3D11RasterizerState>& rasterState) :
		Object(name, mesh, shader, rasterState)
	{
		D3D11_BUFFER_DESC bd = {};

		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.ByteWidth = sizeof(DefaultObjectBuffer);
		bd.CPUAccessFlags = 0;
		bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		createObjectBuffer(device, &bd);
	}

	void Draw(const CComPtr<ID3D11DeviceContext>&, const CComPtr<ID3D11Buffer>&, const CComPtr<ID3D11Buffer>&) const override final;

	const char* objectType() const override final;
};

