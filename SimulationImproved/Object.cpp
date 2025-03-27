#include "Object.h"

Object::~Object() = default;

void Object::setBuffers(const CComPtr<ID3D11DeviceContext>& immediateContext, const void* const bufferData) const
{
	immediateContext->IASetInputLayout(_pShader.mInputLayout);
	immediateContext->RSSetState(_pRasterState);

	_pMesh->bindBuffers(immediateContext);
	if(bufferData)
		immediateContext->UpdateSubresource(_pObjectBuffer, 0, nullptr, bufferData, 0, 0);

	immediateContext->VSSetShader(_pShader.mVertexShader, nullptr, 0);
	immediateContext->VSSetConstantBuffers(0, 1, &_pObjectBuffer.p);
	immediateContext->PSSetShader(_pShader.mPixelShader, nullptr, 0);
	immediateContext->PSSetConstantBuffers(0, 1, &_pObjectBuffer.p);
}

void Object::createObjectBuffer(const CComPtr<ID3D11Device>& device, const D3D11_BUFFER_DESC* const bufferDesc)
{
	const auto hr = device->CreateBuffer(bufferDesc, nullptr, &_pObjectBuffer);
	if (FAILED(hr))
		throw hr;
}