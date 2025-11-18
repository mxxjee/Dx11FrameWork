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

typedef struct tagKeyFrame
{
	_float3		vScale;
	_float4		vRotation;
	_float3		vTranslation;
	float		fTrackPosition;
}KEYFRAME;

struct BoneData
{
	wstring		BoneName;
	XMFLOAT4X4	TransformationMatrix;
	int			BoneIndex = -1;

	_float4x4	m_OffsetMatrix;


	BoneData()
	{
		XMStoreFloat4x4(&TransformationMatrix, XMMatrixIdentity());
		XMStoreFloat4x4(&m_OffsetMatrix, XMMatrixIdentity());

	}

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

	//현재 이 메쉬에게 영향을 주는 본들을 모아놓는다. 모델안에 정의된 전체 본에서 인덱스를 찾아 저장
	vector<_uint>		m_BoneIndices;

	//저장한 본들의 초기설정을 위한 행렬,,?
	vector<_float4x4>           m_OffsetMatrices;
	


};



struct ModelData
{
	wstring name = L"";
	wstring ResourcePath = L"";
	vector<MeshData>		Meshes;
	vector<BoneData>		Bones;
};



/////////////////////////////Anim///////////////////////////

