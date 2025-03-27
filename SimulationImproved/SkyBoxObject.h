#pragma once
#include "Object.h"

class SkyBoxObject : public Object
{
public:
	SkyBoxObject(const std::shared_ptr<Mesh>& mesh, const Shader& shader, const CComPtr<ID3D11RasterizerState>& rasterState,
		const CComPtr<ID3D11DepthStencilState>& normal, const CComPtr<ID3D11DepthStencilState>& sky,
		const CComPtr<ID3D11SamplerState>& sampler, const CComPtr<ID3D11ShaderResourceView>& texture) :
	Object("Sky", mesh, shader, rasterState), _pSkyDepth(sky), _pNormalDepth(normal), _pSampler(sampler), _pSkyTexture(texture) {}

	void Draw(const CComPtr<ID3D11DeviceContext>& immediateContext, const CComPtr<ID3D11Buffer>& perFrameBuffer,
		const CComPtr<ID3D11Buffer>& constantBuffer) const override;

	const char* objectType() const override;

private:
	CComPtr<ID3D11DepthStencilState> _pSkyDepth;
	CComPtr<ID3D11DepthStencilState> _pNormalDepth;
	CComPtr<ID3D11SamplerState> _pSampler;
	CComPtr<ID3D11ShaderResourceView> _pSkyTexture;
};

