#ifndef _COMMON_FX_
#define _COMMON_FX_



row_major matrix g_WorldMatrix;

////////
float g_Brightness = 1.0f;
float g_Alpha = 1.0f;


int g_PointLightNum; //점 조명의 개수

// 샘플러 및 공통 텍스처
Texture2D texture0 : register(t0);
SamplerState sampler0 : register(s0);

//블러효과를 위한변수들
float g_BlurValue =10.f;
float2 OffSet = float2(2, 2);

float4 g_TintColor = float4(1.f, 1.f, 1.f, 1.f);

float g_Time;
float g_Speed;

float4 m_bHasNormalMap = float4(1.f, 0.f, 0.f, 0.f);


#endif