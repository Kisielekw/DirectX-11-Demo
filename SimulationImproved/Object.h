#pragma once
#include <atlcomcli.h>
#include <d3d11.h>
#include <DirectXMath.h>
#include <memory>

#include "IAnimate.h"
#include "ICollider.h"
#include "Mesh.h"
#include "Util.h"

class Object
{
public:
	Object(const char* name, const std::shared_ptr<Mesh>& mesh, const Shader& shader, const CComPtr<ID3D11RasterizerState>& rasterState) :
		_pShader(shader), _pRasterState(rasterState), _pMesh(mesh),_pObjectPosition{ 0.0f, 0.0f, 0.0f, 1.0f }
		,_pObjectRotation{ 0.0f, 0.0f, 0.0f, 1.0f } ,_pObjectScale{ 1.0f, 1.0f, 1.0f, 1.0f }, _pName(name) {}

	virtual ~Object();

	virtual void Draw(const CComPtr<ID3D11DeviceContext>& immediateContext, const CComPtr<ID3D11Buffer>& perFrameBuffer, const CComPtr<ID3D11Buffer>& constantBuffer) const = 0;

	void setMesh(const std::shared_ptr<Mesh>& mesh) { _pMesh = mesh; }
	void setShaders(const Shader& shaders) { _pShader = shaders; }
	void setRasterState(const CComPtr<ID3D11RasterizerState>& rasterState) { _pRasterState = rasterState; }
	void setPosition(const DirectX::XMVECTOR& position) { _pObjectPosition = position; updateCollider(); }
	void setRotation(const DirectX::XMVECTOR& rotation) { _pObjectRotation = rotation; updateCollider(); }
	void setScale(const DirectX::XMVECTOR& scale) { _pObjectScale = scale; updateCollider(); }
	void setAnimation(std::unique_ptr <IAnimate> animation) { _pAnimation = std::move(animation); }
	void setCollider(std::unique_ptr<ICollider> collider)
	{
		_pCollider = std::move(collider);
		updateCollider();
	}

	void updateCollider() const { if (_pCollider) _pCollider->setTransform(_pObjectPosition, _pObjectScale, _pObjectRotation); }
	void animate(float deltaT) { if (_pAnimation) _pAnimation->Animate(deltaT, *this); }

	const std::string& name() const { return _pName; }

	const DirectX::XMVECTOR& position() const { return _pObjectPosition; }
	const DirectX::XMVECTOR& rotation() const { return _pObjectRotation; }
	const DirectX::XMVECTOR& scale() const { return _pObjectScale; }

	const ICollider& collider() const { return *_pCollider; }
	void removeCollider() { _pCollider.release(); }
	bool hasCollider() const { return _pCollider != nullptr; }

	void removeAnimation() { _pAnimation.release(); }
	bool hasAnimation() const { return _pAnimation != nullptr; }

	UINT vertexCount() const { return _pMesh->vertexCount(); }

	virtual const char* objectType() const = 0;

protected:
	void createObjectBuffer(const CComPtr<ID3D11Device>& device, const D3D11_BUFFER_DESC* const bufferDesc);

	void setBuffers(const CComPtr<ID3D11DeviceContext>& immediateContext, const void* const bufferData) const;
	void drawObject(const CComPtr<ID3D11DeviceContext>& immediateContext) const { immediateContext->DrawIndexed(static_cast<UINT>(_pMesh->indexCount()), 0, 0); }

	bool hasMesh() const { return static_cast<bool>(_pMesh); }
	bool hasShaders() const { return static_cast<bool>(_pShader.mVertexShader) || static_cast<bool>(_pShader.mPixelShader);}
	bool hasBuffer() const { return static_cast<bool>(_pObjectBuffer); }
	bool hasRasterState() const { return static_cast<bool>(_pRasterState); }
private:
	CComPtr<ID3D11Buffer> _pObjectBuffer;
	Shader _pShader;
	CComPtr<ID3D11RasterizerState> _pRasterState;

	std::shared_ptr<Mesh> _pMesh;

	DirectX::XMVECTOR _pObjectPosition;
	DirectX::XMVECTOR _pObjectRotation;
	DirectX::XMVECTOR _pObjectScale;

	std::unique_ptr<ICollider> _pCollider;
	std::unique_ptr<IAnimate> _pAnimation;

	std::string _pName;
};

