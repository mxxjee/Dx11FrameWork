#define MAX_POINT_LIGHTS 16

//////임시로 정의해놓은 조명의 값(Directional light)//////////
vector g_vLightDirection = vector(1.f, -1.f, 1.f, 0.f);
vector g_vLightDiffuse = vector(1.f, 1.f, 1.f, 1.f);
vector g_vLightAmbient = vector(1.f, 1.f, 1.f, 1.f);
vector g_vLightSpecular = vector(1.f, 1.f, 1.f, 1.f);



/////////점조명 관련////////////
int g_PointLightNum; //점 조명의 개수
float4 g_vPL_Position[MAX_POINT_LIGHTS];
float g_vPL_Range[MAX_POINT_LIGHTS];
float4 g_vPL_Diffuse[MAX_POINT_LIGHTS];
float4 g_vPL_Ambient[MAX_POINT_LIGHTS];
float4 g_vPL_Specular[MAX_POINT_LIGHTS];


float Compute_Shade(float4 LightDirection, float4 Normal)
{
    return max(dot(normalize(g_vLightDirection) * -1.f, normalize(Normal)), 0.f); //0이하이면 0, 1이상이면 1값으로 보정
  
}

float Compute_Specular(float4 LightDirection, float4 Normal, float4 WorldPos, float4 CamPosition)
{
    vector vLook = WorldPos - CamPosition;
    vector Reflect = reflect(normalize(g_vLightDirection), normalize(Normal));
    float fSpecular = pow(max(dot(Reflect, normalize(vLook) * (-1.f)), 0),50);
    
    return fSpecular;
    
}


float Compute_Attenuation(float Range, float Distance)
{
    //거리가 멀수록 0에 가깝게.
    //감쇠식 다른걸로 수정해도될듯?
    float rangeAtt = saturate(1 - (Distance * Distance) / (Range * Range));

    return rangeAtt;
}