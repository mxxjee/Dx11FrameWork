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

NS_END