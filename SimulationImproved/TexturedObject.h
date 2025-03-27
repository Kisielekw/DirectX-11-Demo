#pragma once
#include "Object.h"
class TexturedObject : public Object
{
public:
	TexturedObject(const char* name, const CComPtr<ID3D11Device>& device, const std::shared_ptr<Mesh>& mesh,
		const Shader& shader, const CComPtr<ID3D11RasterizerState>& rasterState,
		const CComPtr<ID3D11SamplerState>& sampler, const CComPtr<ID3D11ShaderResourceView>& texture) :
		Object(name, mesh, shader, rasterState), _pTexture(texture), _pSamplerState(sampler)
	{
		D3D11_BUFFER_DESC bd = {};

		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.ByteWidth = sizeof(DefaultObjectBuffer);
		bd.CPUAccessFlags = 0;
		bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		createObjectBuffer(device, &bd);
	}

	virtual void Draw(const CComPtr<ID3D11DeviceContext>& immediateContext, const CComPtr<ID3D11Buffer>& perFrameBuffer, const CComPtr<ID3D11Buffer>& constantBuffer) const override;

	const char* objectType() const override;

	void setSampler(const CComPtr<ID3D11SamplerState>& sampler) { _pSamplerState = sampler; }
	void setTexture(const CComPtr<ID3D11ShaderResourceView>& texture) { _pTexture = texture; }
private:
	CComPtr<ID3D11ShaderResourceView> _pTexture;
	CComPtr<ID3D11SamplerState> _pSamplerState;
};

