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
	map<aiTextureType, CTexture*>      m_Textures;

	MaterialData() {}
	

};

struct BoneData
{
	wstring		BoneName;
	XMFLOAT4X4	TransformationMatrix;
	int			BoneIndex = -1;


};


struct MeshData
{
	wstring Name;
	XMMATRIX		Transform;			//변환행렬, 각 파츠당 변환행렬적용

	uint32_t	VertexCount;			//정점개수
	uint32_t	IndexCount;				//인덱스카운트
	vector<VTXMESH>			Vertices;
	vector<uint32_t>		Indices;
	MaterialData		m_MaterialData;
	int					m_iNumBones = 0;
};

struct ModelData
{
	wstring name = L"";
	wstring ResourcePath = L"";
	vector<MeshData>		Meshes;
	vector<BoneData>		Bones;
};



/////////////////////////////Anim///////////////////////////

