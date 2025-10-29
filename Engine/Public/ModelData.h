#pragma once
#include "Engine_Define.h"
#include "VertexData.h"

//메테리얼 정보

namespace Engine
{
	class CTexture;
}
struct MaterialData
{
	wstring m_MaterialName=L"";

	//Diffuse,   // Texture(SRV 소유)
	map<MaterialMapType, CTexture*>      m_Textures;

	MaterialData() {}
	

};



//메쉬정보
struct MeshData
{
	wstring Name;
	XMFLOAT4X4		Transform;			//변환행렬, 각 파츠당 변환행렬적용

	uint32_t	VertexCount;			//정점개수
	uint32_t	IndexCount;				//인덱스카운트

	vector<VTXMESH>		Vertices;
	vector<uint32_t>		Indices;

	MaterialData		m_MaterialData;
};


struct ModelData
{
	wstring name;
	vector<MeshData>		Meshes;
};