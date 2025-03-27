
#include "DefaultObject.h"

void DefaultObject::Draw(const CComPtr<ID3D11DeviceContext>& immediateContext, const CComPtr<ID3D11Buffer>& perFrameBuffer, const CComPtr<ID3D11Buffer>& constantBuffer) const
{
	if (!hasMesh() || !hasShaders() || !hasBuffer() || !hasRasterState())
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

	drawObject(immediateContext);
}

const char* DefaultObject::objectType() const { return DEFAULT; }
