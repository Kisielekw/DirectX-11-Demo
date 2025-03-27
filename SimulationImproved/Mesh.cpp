#include "Mesh.h"

#include <fstream>
#include <sstream>
#include <unordered_map>

bool operator==(const Mesh::vtnIndex& rhs, const Mesh::vtnIndex& lhs)
{
	return lhs.normalIndex == rhs.normalIndex && lhs.textureIndex == rhs.textureIndex && lhs.vertexIndex == rhs.vertexIndex;
}

HRESULT Mesh::LoadOBJFile(const char* file, const CComPtr<ID3D11Device>& deviceContext, const Shader& shader, std::map<std::string, std::shared_ptr<Mesh>>& meshList)
{
	struct protoMesh
	{
		std::vector<SimpleVertex> simpleVertices;
		std::vector<ComplexVertex> complexVertices;
		std::vector<WORD> indices;
		std::string meshName;
	};

	std::ifstream fin(file);

	if (!fin)
		return HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND);

	std::vector<DirectX::XMFLOAT3> vertices;
	std::vector<DirectX::XMFLOAT3> normals;
	std::vector<DirectX::XMFLOAT2> uvs;
	std::unordered_map<vtnIndex, size_t, vtnIndexHash> vtnMap;

	std::string tag;
	std::vector<protoMesh> protoMeshes;

	protoMesh* meshPtr = nullptr;

	while (fin >> tag)
	{
		if (tag == "o")
		{
			protoMeshes.emplace_back();
			meshPtr = &protoMeshes.back();
			fin >> meshPtr->meshName;
		}
		else if (tag == "v" && meshPtr)
		{
			float x, y, z;
			fin >> x >> y >> z;
			vertices.emplace_back(x, y, z);
		}
		else if (tag == "vt" && meshPtr)
		{
			float u, v;
			fin >> u >> v;
			uvs.emplace_back(u, v);
		}
		else if (tag == "vn" && meshPtr)
		{
			float x, y, z;
			fin >> x >> y >> z;
			normals.emplace_back(x, y, z);
		}
		else if (tag == "f" && meshPtr)
		{

			std::string face;
			std::getline(fin, face);
			std::istringstream faceIn(face);

			while (!faceIn.eof())
			{
				//very ugly code

				vtnIndex index;
				char c;

				faceIn >> index.vertexIndex >> c >> index.textureIndex >> c >> index.normalIndex;
				auto i = vtnMap.find(index);
				if (i == vtnMap.end())
				{
					meshPtr->simpleVertices.emplace_back(vertices[index.vertexIndex - 1], normals[index.normalIndex - 1],
					   uvs[index.textureIndex - 1]);
					meshPtr->indices.push_back(static_cast<WORD>(meshPtr->simpleVertices.size() - 1));
					vtnMap.emplace(index, meshPtr->simpleVertices.size() - 1);
					continue;
				}

				meshPtr->indices.push_back(static_cast<WORD>(i->second));
			}
		}
		else
		{
			std::string line;
			std::getline(fin, line);
		}
	}

	for (auto& proto_mesh : protoMeshes)
	{
		meshList.try_emplace(proto_mesh.meshName, std::make_shared<Mesh>(proto_mesh.simpleVertices, proto_mesh.indices, proto_mesh.meshName, deviceContext, shader.mInputLayout));
	}
	return S_OK;
}

Mesh::Mesh(const std::vector<SimpleVertex>& Vertices, const std::vector<WORD>& indices, const std::string& meshName, const CComPtr<ID3D11Device>& device, const CComPtr<ID3D11InputLayout>& vertexLayout) :
	_pMeshName(meshName), _pVertexLayout(vertexLayout), _pVertexCount(Vertices.size()), _pIndexCount(indices.size()), _pStride(static_cast<UINT>(sizeof(SimpleVertex)))
{
	D3D11_BUFFER_DESC bd = {};
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = static_cast<UINT>(_pStride * _pVertexCount);
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA InitData = {};
	InitData.pSysMem = Vertices.data();
	auto hr = device->CreateBuffer(&bd, &InitData, &_pVertexBuffer);
	if (FAILED(hr))
		throw std::exception("Vertex buffer was unable to be created");

	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = static_cast<UINT>(sizeof(WORD) * _pIndexCount);
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = 0;
	InitData.pSysMem = indices.data();
	hr = device->CreateBuffer(&bd, &InitData, &_pIndexBuffer);
	if (FAILED(hr))
		throw std::exception("Index buffer was unable to be created");
}

void Mesh::bindBuffers(const CComPtr<ID3D11DeviceContext>& immediateContext) const
{
	auto offset = static_cast<UINT>(0);
	immediateContext->IASetVertexBuffers(0, 1, &_pVertexBuffer.p, &_pStride, &offset);
	immediateContext->IASetIndexBuffer(_pIndexBuffer, DXGI_FORMAT_R16_UINT, 0);
	immediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}
