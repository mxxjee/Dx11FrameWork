#pragma once
#include "Engine_Define.h"
#include "VertexData.h"

//메쉬정보
struct MeshData
{
	wstring Name;
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