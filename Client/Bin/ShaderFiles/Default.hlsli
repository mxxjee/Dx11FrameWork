#ifndef _COMMON_FX_
#define _COMMON_FX_


// 상수버퍼 및 전역 변수
row_major matrix g_WorldMatrix;
row_major matrix g_ViewProjMatrix;
////////
float g_Brightness = 1.0f;


// 샘플러 및 공통 텍스처
Texture2D texture0 : register(t0);
SamplerState sampler0 : register(s0);

//블러효과를 위한변수들
float g_BlurValue =10.f;
float2 OffSet = (2, 2);



#endif