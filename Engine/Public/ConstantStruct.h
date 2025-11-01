#pragma once
#include "Engine_Define.h"

struct ConstantBufferStruct
{
};

struct CameraBuffer :public ConstantBufferStruct
{
	_float4x4 g_ViewProjMatrix;
	_float4 g_CamPosition;

};

struct DirectionLightBuffer :public ConstantBufferStruct
{
	_float4 g_vLightDirection = _float4(1.f, -1.f, 1.f, 0.f);
	_float4 g_vLightDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
	_float4 g_vLightAmbient = _float4(1.f, 1.f, 1.f, 1.f);
	_float4 g_vLightSpecular = _float4(1.f, 1.f, 1.f, 1.f);

};


struct PointLightBuffer :public ConstantBufferStruct
{

	_float4 g_vPL_Position[16];
	_float4 g_vPL_Diffuse[16];
	_float4 g_vPL_Ambient[16];
	_float4 g_vPL_Specular[16];
	_float4 g_vPL_Range[16];


};