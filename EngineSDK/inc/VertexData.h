#pragma once
#include "Engine_Define.h"

NS_BEGIN(Engine)
struct ENGINE_DLL VertexColorData
{
	_float3 position = { 0,0,0 };
	_float4 color = { 0,0,0,0 };


	static vector<D3D11_INPUT_ELEMENT_DESC> desc;

};


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

NS_END