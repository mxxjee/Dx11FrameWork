#pragma once
#include "Engine_Define.h"
#include "VertexData.h"

//메쉬정보
struct MeshData
{
	wstring Name;
	_uint ParentIndex = UINT32_MAX;		//부모 노드 인덱스
	XMFLOAT4X4		Transform;			//변환행렬, 각 파츠당 변환행렬적용

	uint32_t	VertexCount;			//정점개수
	uint32_t	IndexCount;				//인덱스카운트

	vector<VTXMESH>		Vertices;
	vector<uint32_t>		Indices;


};


struct ModelData
{
	wstring name;
	vector<MeshData>		Meshes;
};