

/*전역변수 : ID3D11EFFECT객체를 통해 값변경이 가능하지만,
HLSL 안에선 CONST화 되어 값 변경이 불가함 (읽기전용)*/
#include "Default.hlsli"
#include "Shader_Light.hlsli"
#include "Engine_Shader_Defines.hlsli"

vector g_vMaterialAmbient = vector(1.f, 1.f, 1.f, 1.f);
vector g_vMaterialSpecular = vector(1.f, 1.f, 1.f, 1.f);



Texture2D g_DiffuseTexture;
Texture2D g_SpecularTexture;
Texture2D g_AmbientTexture;


Texture2D g_MaskTexture;

float4 g_DamageColor = float4(1.f, 0.f, 0.f, 1.f);
float b_Damage = false;



////////임시로 정해놓은 오브젝트의 메테리얼값, 실제는 텍스처를 읽어서 처리해야함

struct VS_IN
{
    float3 vPosition : POSITION;
    float3 vNormal : NORMAL;
    float3 vTangent : TANGENT;
    float3 vBinormal : BINORMAL;
    float2 vTexcoord : TEXCOORD0;
    uint4 vBlendIndex : BLENDINDEX;
    float4 vBlendWeight : BLENDWEIGHT;
};

//픽셀셰이더에서 normal써야함(빛연산 때매) 그니까 normal추가해주자.
struct VS_OUT
{
    float4 vPosition : SV_POSITION;     // 얘는 PS단계 전 W나누기를 자동으로 수행한다.(완전한 NDC공간)
    float4 vNormal : NORMAL;
    float2 vTexcoord : TEXCOORD0;
    float4 vWorldPos : TEXCOORD1;
    float4 vProjPos : TEXCOORD2;        //따로 W나누기를 수행하지 않으므로 투영행렬만 곱한결과 저장
};

struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float4 vNormal : NORMAL;
    float2 vTexcoord : TEXCOORD0;
    float4 vWorldPos : TEXCOORD1;
    float4 vProjPos : TEXCOORD2;
    
};


struct PS_OUT
{
    vector vDiffuse : SV_TARGET0;
    vector vNormal : SV_TARGET1;
    vector vDepth : SV_TARGET2;
};

/*버텍스 셰이더 단계의 함수
    버텍스 쉐이더 = 정점 갖고놀기
    정점과 행렬의 연산을 수 행*/
VS_OUT VS_MAIN(VS_IN In) 
{
    VS_OUT Out;
    
    float fWeightW = 1.f - (In.vBlendWeight.x + In.vBlendWeight.y + In.vBlendWeight.z);
    
    
    matrix BoneMatrix = g_BoneMatrices[In.vBlendIndex.x] * In.vBlendWeight.x +
        g_BoneMatrices[In.vBlendIndex.y] * In.vBlendWeight.y +
        g_BoneMatrices[In.vBlendIndex.z] * In.vBlendWeight.z +
        g_BoneMatrices[In.vBlendIndex.w] * fWeightW;
    
    vector vPosition = mul(vector(In.vPosition, 1.f), BoneMatrix);
    
    vPosition = mul(vPosition, g_WorldMatrix);
    vPosition = mul(vPosition, g_ViewProjMatrix);
    
    
    //계산완료된 vPosition(x,y,z,w)중 w는 z값을 보관중이다.
    Out.vPosition = vPosition;
    
    Out.vNormal = mul(vector(In.vNormal, 0.f), BoneMatrix); 
    Out.vNormal = mul(Out.vNormal, g_WorldMatrix);
    Out.vNormal = normalize(mul(vector(In.vNormal, 0.f), g_WorldMatrix));

    Out.vTexcoord = In.vTexcoord;
    Out.vWorldPos = mul(vector(In.vPosition, 1.f), g_WorldMatrix);
    Out.vProjPos = Out.vPosition;
    
    
    //sdfsdf
    
    return Out;
    

}

//이후 RS단계에서 GPU가 내부적으로 w나누기 수행
PS_OUT PS_MAIN(PS_IN Input) 
{
    PS_OUT Out;
    
    float4 fDiffuseColor, fAmbientColor, fSpeculrColor;
    Out.vDiffuse = g_DiffuseTexture.Sample(DefaultSampler, Input.vTexcoord);
    
    
                        //0~1값으로 치환
    Out.vNormal = float4(Input.vNormal.xyz * 0.5f + 0.5f, 0.f);

  
    
    float3 Tmpcolor = saturate(Out.vDiffuse.rgb + float3(2.5, 0.0, 0.0));
    Out.vDiffuse.rgb = lerp(Out.vDiffuse.rgb, Tmpcolor, b_Damage);
    
    float flash = abs(sin(g_Time * 20));
    if (flash > 0.5)
        discard;
    
    /*Depth_RenderTarget에 기록하자*/
    
    Out.vDepth = float4(Input.vProjPos.z / Input.vProjPos.w, ///0~1사이 
                        Input.vPosition.w, //월드 z, 
                        0.f, 0.f);
   
    return Out;
    
    
    ////음영값 (diffuse 세기)
    //float fShade = Compute_Shade(g_vLightDirection, Input.vNormal);
    
  
    ////specular 세기 = 반사벡터를 구해서  카메라 시야벡터 * (-1)와 내적
    //float fSpecular = Compute_Specular(g_vLightDirection, Input.vNormal, Input.vWorldPos, g_CamPosition,80);
    
 
    //fDiffuseColor = g_vLightDiffuse * MtrlDiffuseColor * fShade;
    //fAmbientColor = g_vLightAmbient * g_vMaterialAmbient * (MtrlDiffuseColor * 0.5f);
    

    //float4 MtrlSpecularColor = g_SpecularTexture.Sample(DefaultSampler, Input.vTexcoord);
    //fSpeculrColor = g_vLightSpecular * MtrlSpecularColor * fSpecular * 0.5f;

    
    

    
    //////////////////점 조명에 대한 연산/////////////////////
    //for (int i = 0; i < g_PointLightNum; ++i)
    //{
    //    //1.어디방향으로 빛이오는지 계산하자.
    //    vector vLightDirection = g_vPL_Position[i] - Input.vWorldPos;
    //    float Distance = length(vLightDirection);
        
    //    float fShade = Compute_Shade(vLightDirection, Input.vNormal);
    //    float fAttenuation = Compute_Attenuation(g_vPL_Range[i].r, Distance);
    //    float fSpecular = Compute_Specular(vLightDirection, Input.vNormal, Input.vWorldPos, g_CamPosition, 80);
        
        
    //    ///이거 이상함.
    //    fDiffuseColor += g_vPL_Diffuse[i] * MtrlDiffuseColor * fShade * fAttenuation;
    //    //fAmbientColor += g_vPL_Ambient[i] * g_vMaterialAmbient * fAttenuation;
    //   // fSpeculrColor += g_vPL_Specular[i] * MtrlSpecularColor * fSpecular;
        

    //}
     
    float4 ResultColor = fDiffuseColor + fAmbientColor + fSpeculrColor;
    ResultColor = saturate(ResultColor);
    
    //fAmbientColor *= 0.8f;
        //데미지 여부에 따른 피격효과\
    
    //float3 Tmpcolor = saturate(ResultColor.rgb + float3(2.5, 0.0, 0.0));
    //ResultColor.rgb = lerp(ResultColor.rgb, Tmpcolor, b_Damage);
    
    //float flash = abs(sin(g_Time*20));
    //if(flash>0.5)
    //    discard;
    
    
    //return ResultColor;

}

float4 PS_Eye(PS_IN Input) : SV_Target0
{
    return float4(1.f, 0.f, 0.f, 1.f);
    

}

PS_OUT PS_Alpha(PS_IN Input)
{
    PS_OUT Out;
    
    float4 fDiffuseColor, fAmbientColor, fSpeculrColor;
    Out.vDiffuse = g_DiffuseTexture.Sample(DefaultSampler, Input.vTexcoord);
    
    
                        //0~1값으로 치환
    Out.vNormal = float4(Input.vNormal.xyz * 0.5f + 0.5f, 0.f);

  
    
    float3 Tmpcolor = saturate(Out.vDiffuse.rgb + float3(2.5, 0.0, 0.0));
    Out.vDiffuse.a = g_Alpha;
    
    /*Depth_RenderTarget에 기록하자*/
    
    Out.vDepth = float4(Input.vProjPos.z / Input.vProjPos.w, ///0~1사이 
                        Input.vPosition.w, //월드 z, 
                        0.f, 0.f);
   
    return Out;
}
/*렌더링 방법을 정의한다.*/
technique11 DefaultTechnique
{
    /*pass를 정의한다 : pass의 이름을 설정,
    이 pass가 선택되었으면 
    VertexShader는 이걸쓰고, PixelShader는 이걸쓸거에요.*/
    pass Default
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        //이 pass가 선택되면 VertexShader는 이렇게 컴파일하세요.
                                //버전 , 진입함수 설정
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();

    }

    pass Alpha
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Alpha, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        //이 pass가 선택되면 VertexShader는 이렇게 컴파일하세요.
                                //버전 , 진입함수 설정
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_Alpha();

    }

    pass NonNormal
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        //이 pass가 선택되면 VertexShader는 이렇게 컴파일하세요.
                                //버전 , 진입함수 설정
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();

    }

    
}