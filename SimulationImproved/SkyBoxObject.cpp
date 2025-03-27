#include "SkyBoxObject.h"

void SkyBoxObject::Draw(const CComPtr<ID3D11DeviceContext>& immediateContext, const CComPtr<ID3D11Buffer>& perFrameBuffer, const CComPtr<ID3D11Buffer>& constantBuffer) const
{
	if (!hasMesh() || !hasShaders() || !hasRasterState() || !_pSkyTexture || !_pSampler)
		return;

	setBuffers(immediateContext, nullptr);

	immediateContext->PSSetSamplers(0, 1, &_pSampler.p);
	immediateContext->PSSetShaderResources(0, 1, &_pSkyTexture.p);

	immediateContext->VSSetConstantBuffers(1, 1, &constantBuffer.p);
	immediateContext->VSSetConstantBuffers(2, 1, &perFrameBuffer.p);
	immediateContext->PSSetConstantBuffers(1, 1, &constantBuffer.p);
	immediateContext->PSSetConstantBuffers(2, 1, &perFrameBuffer.p);

	immediateContext->OMSetDepthStencilState(_pSkyDepth, 1);

	drawObject(immediateContext);

	immediateContext->OMSetDepthStencilState(_pNormalDepth, 1);
}

const char* SkyBoxObject::objectType() const { return "SkyBox"; }