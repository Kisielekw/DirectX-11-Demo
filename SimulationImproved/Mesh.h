#pragma once
#include <d3d11_1.h>
#include <directxmath.h>
#include <atlbase.h>
#include <map>
#include <memory>
#include <string>
#include <vector>

#include "Util.h"

class Mesh final
{
	std::string _pMeshName;

	CComPtr<ID3D11InputLayout> _pVertexLayout;
	CComPtr<ID3D11Buffer> _pVertexBuffer;
	CComPtr<ID3D11Buffer> _pIndexBuffer;
	size_t _pVertexCount = 0;
	size_t _pIndexCount = 0;
	UINT _pStride = 0;

public:
	struct vtnIndex
	{
		int vertexIndex, textureIndex, normalIndex;

	};

	struct vtnIndexHash {
		size_t operator()(const vtnIndex& key) const {
			return std::hash<int>()(key.vertexIndex) ^
				(std::hash<int>()(key.textureIndex) << 1) ^
				(std::hash<int>()(key.normalIndex) << 2);
		}
	};

	Mesh(const std::vector<SimpleVertex>&, const std::vector<WORD>&, const std::string&, const CComPtr<ID3D11Device>&, const CComPtr<ID3D11InputLayout>&);
	Mesh(const Mesh&) = delete;

	Mesh& operator=(const Mesh&) = delete;

	static HRESULT LoadOBJFile(const char*, const CComPtr<ID3D11Device>&, const Shader&, std::map<std::string, std::shared_ptr<Mesh>>&);

	const char* Name() const { return _pMeshName.c_str(); }
	const size_t& indexCount() const { return _pIndexCount; }
	const size_t& vertexCount() const { return _pVertexCount; }

	void bindBuffers(const CComPtr<ID3D11DeviceContext>&) const;
};

bool operator==(const Mesh::vtnIndex& rhs, const Mesh::vtnIndex& lhs);