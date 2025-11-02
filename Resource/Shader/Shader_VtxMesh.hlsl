

/*전역변수 : ID3D11EFFECT객체를 통해 값변경이 가능하지만,
HLSL 안에선 CONST화 되어 값 변경이 불가함 (읽기전용)*/
#include "Default.hlsli"
#include "Shader_Light.hlsli"


vector g_vMaterialAmbient = vector(1.f, 1.f, 1.f, 1.f);
vector g_vMaterialSpecular = vector(1.f, 1.f, 1.f, 1.f);



Texture2D g_DiffuseTexture;
Texture2D g_SpecularTexture;
Texture2D g_AmbientTexture;


Texture2D g_MaskTexture;


////////임시로 정해놓은 오브젝트의 메테리얼값, 실제는 텍스처를 읽어서 처리해야함

struct VS_IN
{
    float3 vPosition : POSITION;
    float3 vNormal : NORMAL;
    float3 vTangent : TANGENT;
    float3 vBinormal : BINORMAL;
    float2 vTexcoord : TEXCOORD0;
};

//픽셀셰이더에서 normal써야함(빛연산 때매) 그니까 normal추가해주자.
struct VS_OUT
{
    float4 vPosition : SV_POSITION;
    float4 vNormal : NORMAL;
    float2 vTexcoord : TEXCOORD0;
    float4 vWorldPos : TEXCOORD1;
};

struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float4 vNormal : NORMAL;
    float2 vTexcoord : TEXCOORD0;
    float4 vWorldPos : TEXCOORD1;
};





/*버텍스 셰이더 단계의 함수
    버텍스 쉐이더 = 정점 갖고놀기
    정점과 행렬의 연산을 수 행*/
VS_OUT VS_MAIN(VS_IN In) 
{
    VS_OUT Out;
    vector vPosition = mul(vector(In.vPosition, 1.f), g_WorldMatrix);
   
    
    vPosition = mul(vPosition, g_ViewProjMatrix);
    
    
    //계산완료된 vPosition(x,y,z,w)중 w는 z값을 보관중이다.
    Out.vPosition = vPosition;
    Out.vNormal = mul(vector(In.vNormal, 0.f), g_WorldMatrix);
    Out.vTexcoord = In.vTexcoord;
    Out.vWorldPos = mul(vector(In.vPosition, 1.f), g_WorldMatrix);
    
    //sdfsdf
   
    return Out;
    

}

//이후 RS단계에서 GPU가 내부적으로 w나누기 수행
float4 PS_MAIN(PS_IN Input) : SV_Target0
{
    float4 fDiffuseColor, fAmbientColor, fSpeculrColor;

    //Maskmap의 흰색부분 = 풀, 검은색 부분 = 흙
    float4 MtrlDiffuseColor =  g_DiffuseTexture.Sample(sampler0, Input.vTexcoord);
 
    //음영값 (diffuse 세기)
    float fShade = Compute_Shade(g_vLightDirection, Input.vNormal);
    
  
    //specular 세기 = 반사벡터를 구해서  카메라 시야벡터 * (-1)와 내적
    float fSpecular = Compute_Specular(g_vLightDirection, Input.vNormal, Input.vWorldPos, g_CamPosition);
    
 
    fDiffuseColor = g_vLightDiffuse * MtrlDiffuseColor * fShade;
    fAmbientColor = g_vLightAmbient * g_vMaterialAmbient * (MtrlDiffuseColor * 0.5f);
    

    float4 MtrlSpecularColor = g_SpecularTexture.Sample(sampler0, Input.vTexcoord);
    fSpeculrColor = g_vLightSpecular * MtrlSpecularColor * fSpecular;

    

    
    //////////////////점 조명에 대한 연산/////////////////////
    for (int i = 0; i < g_PointLightNum; ++i)
    {
        //1.어디방향으로 빛이오는지 계산하자.
        vector vLightDirection = g_vPL_Position[i] - Input.vWorldPos;
        float Distance = length(vLightDirection);
        
        float fShade = Compute_Shade(vLightDirection, Input.vNormal);
        float fAttenuation = Compute_Attenuation(g_vPL_Range[i].r, Distance);
        float fSpecular = Compute_Specular(vLightDirection, Input.vNormal, Input.vWorldPos, g_CamPosition);
        
        
        ///이거 이상함.
        fDiffuseColor += g_vPL_Diffuse[i] * MtrlDiffuseColor * fShade * fAttenuation;
        fAmbientColor += g_vPL_Ambient[i] * g_vMaterialAmbient * fAttenuation;
        fSpeculrColor += g_vPL_Specular[i] * MtrlSpecularColor * fSpecular;
        

    }
  
    
    fAmbientColor *= 0.15f;
    float4 ResultColor = fDiffuseColor + fAmbientColor + fSpeculrColor;
    return saturate(ResultColor);

}


float4 PS_Eye(PS_IN Input) : SV_Target0
{
    return float4(1.f, 0.f, 0.f, 1.f);
    

}
/*렌더링 방법을 정의한다.*/
technique11 DefaultTechnique
{
    /*pass를 정의한다 : pass의 이름을 설정,
    이 pass가 선택되었으면 
    VertexShader는 이걸쓰고, PixelShader는 이걸쓸거에요.*/
    pass Default
    {
        //이 pass가 선택되면 VertexShader는 이렇게 컴파일하세요.
                                //버전 , 진입함수 설정
        VertexShader = compile vs_5_0 VS_MAIN();

        PixelShader = compile ps_5_0 PS_MAIN();

    }


        //패스값 나눠지는지 테스트용
    pass face_low__MI_eye
    {
        VertexShader = compile vs_5_0 VS_MAIN();
        PixelShader = compile ps_5_0 PS_MAIN();

    }

    
}