#pragma once
#include "Engine_Define.h"

NS_BEGIN(Engine)
typedef struct ENGINE_DLL VertexColorData
{
	_float3 vPosition = { 0,0,0 };
	_float4 vColor = { 0,0,0,0 };


	static vector<D3D11_INPUT_ELEMENT_DESC> desc;

}VTXPOSCOR;


struct ENGINE_DLL VertexTextureData
{
	_float3 position = { 0,0,0 };
	_float2 uv = { 0,0 };


	static vector<D3D11_INPUT_ELEMENT_DESC> desc;

};

//////////////////Vertex Struct////////////////////////
typedef struct ENGINE_DLL  tagVertexPositionTexcoord
{
	XMFLOAT3		vPosition;
	XMFLOAT2		vTexcoord;

	static vector<D3D11_INPUT_ELEMENT_DESC>    desc;
	
}VTXPOSTEX;


typedef struct ENGINE_DLL  tagVertexPositionNormalTexcoord
{
	XMFLOAT3		vPosition;
	XMFLOAT3		vNormal;
	XMFLOAT2		vTexcoord;

	static vector<D3D11_INPUT_ELEMENT_DESC>    desc;

}VTXNORTEX;

typedef struct ENGINE_DLL  tagVertexMesh
{
	XMFLOAT3		vPosition;
	XMFLOAT3		vNormal;
	XMFLOAT3		Tangent;
	XMFLOAT3		BiNormal;
	XMFLOAT2		vTexcoord;

	static vector<D3D11_INPUT_ELEMENT_DESC>    desc;

}VTXMESH;

typedef struct ENGINE_DLL  tagVertexAnimationMesh
{
	XMFLOAT3		vPosition;
	XMFLOAT3		vNormal;
	XMFLOAT3		Tangent;
	XMFLOAT3		BiNormal;
	XMFLOAT2		vTexcoord;

	XMUINT4			vBlendIndex;		//이 정점이 영향을 받고있는 뼈에 대한 인덱스 , 최대 4개까지 지정가능
	XMFLOAT4		vBlendWeight;		//그 뼈에 대한 가중치(얼마나 영향을받고있는지, BlendIndex와 1:1대응)
	
	static const unsigned int		iNumElements = { 7 };
	static vector<D3D11_INPUT_ELEMENT_DESC>    desc;

}VTXMESH;

typedef struct ENGINE_DLL  tagEditMesh
{
	XMFLOAT3		vPosition;
	XMFLOAT2		vTexcoord;

	static vector<D3D11_INPUT_ELEMENT_DESC>    desc;

}EDITMESH;


NS_END