#include "TexturedObject.h"

void TexturedObject::Draw(const CComPtr<ID3D11DeviceContext>& immediateContext, const CComPtr<ID3D11Buffer>& perFrameBuffer, const CComPtr<ID3D11Buffer>& constantBuffer) const
{
	if (!hasMesh() || !hasShaders() || !hasBuffer() || !hasRasterState() || !_pTexture || !_pSamplerState)
		return;

	

	DirectX::XMMATRIX transform = DirectX::XMMatrixIdentity();
	auto rotationQuaternion = DirectX::XMQuaternionRotationRollPitchYawFromVector(rotation());
	transform *= DirectX::XMMatrixRotationQuaternion(rotationQuaternion);
	transform *= DirectX::XMMatrixScalingFromVector(scale());
	transform *= DirectX::XMMatrixTranslationFromVector(position());

	DefaultObjectBuffer buffer{ XMMatrixTranspose(transform) };

	setBuffers(immediateContext, &buffer);

	immediateContext->VSSetConstantBuffers(1, 1, &constantBuffer.p);
	immediateContext->VSSetConstantBuffers(2, 1, &perFrameBuffer.p);
	immediateContext->PSSetConstantBuffers(1, 1, &constantBuffer.p);
	immediateContext->PSSetConstantBuffers(2, 1, &perFrameBuffer.p);
	immediateContext->PSSetShaderResources(0, 1, &_pTexture.p);
	immediateContext->PSSetSamplers(0, 1, &_pSamplerState.p);

	drawObject(immediateContext);

	ID3D11SamplerState* state = nullptr;
	ID3D11ShaderResourceView* tex = nullptr;

	immediateContext->PSSetSamplers(0, 1, &state);
	immediateContext->PSSetShaderResources(0, 1, &tex);
}

const char* TexturedObject::objectType() const { return "TexturedType"; }