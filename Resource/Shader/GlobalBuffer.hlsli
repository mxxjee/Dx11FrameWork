
#define MAX_POINT_LIGHTS 16

// 상수버퍼 및 전역 변수
cbuffer CameraBuffer : register(b0)
{
    row_major matrix g_ViewProjMatrix;
    vector g_CamPosition;
    
}

cbuffer DirectionLightBuffer : register(b1)
{
    //////임시로 정의해놓은 조명의 값(Directional light)//////////
    vector g_vLightDirection = vector(1.f, -1.f, 1.f, 0.f);
    vector g_vLightDiffuse = vector(1.f, 1.f, 1.f, 1.f);
    vector g_vLightAmbient = vector(1.f, 1.f, 1.f, 1.f);
    vector g_vLightSpecular = vector(1.f, 1.f, 1.f, 1.f);
}


cbuffer PointLightBuffer : register(b2)
{
   
    float4 g_vPL_Position[16];
    float4 g_vPL_Diffuse[16];
    float4 g_vPL_Ambient[16];
    float4 g_vPL_Specular[16];
    float4 g_vPL_Range[16];


    
}


cbuffer BoneBuffer : register(b3)
{
    row_major matrix g_BoneMatrices[512];
}