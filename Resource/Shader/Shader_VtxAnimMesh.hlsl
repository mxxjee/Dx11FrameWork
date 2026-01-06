

/*전역변수 : ID3D11EFFECT객체를 통해 값변경이 가능하지만,
HLSL 안에선 CONST화 되어 값 변경이 불가함 (읽기전용)*/
#include "Default.hlsli"
#include "Shader_Light.hlsli"
#include "Engine_Shader_Defines.hlsli"

vector g_vMaterialAmbient = vector(1.f, 1.f, 1.f, 1.f);
vector g_vMaterialSpecular = vector(1.f, 1.f, 1.f, 1.f);



Texture2D g_DiffuseTexture;
Texture2D g_NormalTexture;
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
    float4 vTangent : TANGENT;
    float4 vBinormal : BINORMAL;
    float2 vTexcoord : TEXCOORD0;
    float4 vWorldPos : TEXCOORD1;
    float4 vProjPos : TEXCOORD2;        //따로 W나누기를 수행하지 않으므로 투영행렬만 곱한결과 저장
};

struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float4 vNormal : NORMAL;
    float4 vTangent : TANGENT;
    float4 vBinormal : BINORMAL;
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

    // 1. 애니메이션 뼈 가중치 계산
    float fWeightW = 1.f - (In.vBlendWeight.x + In.vBlendWeight.y + In.vBlendWeight.z);
    
    matrix BoneMatrix = g_BoneMatrices[In.vBlendIndex.x] * In.vBlendWeight.x +
                        g_BoneMatrices[In.vBlendIndex.y] * In.vBlendWeight.y +
                        g_BoneMatrices[In.vBlendIndex.z] * In.vBlendWeight.z +
                        g_BoneMatrices[In.vBlendIndex.w] * fWeightW;

    // 2. 포지션 변환 (Local -> Bone -> World -> ViewProj)
    vector vPosition = mul(vector(In.vPosition, 1.f), BoneMatrix);
    vector vNormal = mul(vector(In.vNormal, 0.f), BoneMatrix);
    
    matrix matWVP;
    matWVP = mul(g_WorldMatrix, g_ViewProjMatrix);
    

    // 3. 노멀/탄젠트/바이노멀 변환 (Bone과 World를 모두 곱해야 함)
    // 방향 벡터이므로 w는 0으로 처리하여 이동값은 무시하고 회전/스케일만 적용
   
    Out.vPosition = mul(vPosition, matWVP);
    Out.vNormal = normalize(mul(vNormal, g_WorldMatrix));
    Out.vTangent = normalize(mul(vector(In.vTangent.xyz, 0.f), g_WorldMatrix));
    
    float handedness = (dot(cross(In.vNormal, In.vTangent), In.vTangent) < 0.f) ? -1.f : 1.f;
    Out.vTangent = float4(normalize(In.vTangent), handedness);
    
    
    Out.vBinormal = normalize(mul(vector(In.vBinormal.xyz, 0.f), g_WorldMatrix));
    Out.vTexcoord = In.vTexcoord;
    Out.vWorldPos = mul(vector(In.vPosition, 1.f), g_WorldMatrix);
    Out.vProjPos = Out.vPosition;
    return Out;
    

}

//이후 RS단계에서 GPU가 내부적으로 w나누기 수행
PS_OUT PS_MAIN(PS_IN Input) 
{
    
    PS_OUT Out;

    float4 albedo = g_DiffuseTexture.Sample(DefaultSampler, Input.vTexcoord);
    if (albedo.a < 0.3f)
        discard;
    
  
    

    // 1. 노멀맵 (Tangent Space)
    float3 nTS;
    float3 normalTex = g_NormalTexture.Sample(DefaultSampler, Input.vTexcoord).xyz;
    nTS.xy = normalTex.xy * 2.0f - 1.0f;
    nTS.z = sqrt(saturate(1.0f - dot(nTS.xy, nTS.xy)));

    // 2. 정점 기준 벡터 (World Space)
    float3 N = normalize(Input.vNormal.xyz);
    float3 T = normalize(Input.vTangent.xyz);

    // Binormal은 절대 입력값 쓰지 말 것
    float3 B = -normalize(cross(N, T));

    //만약 여전히 반 갈라지면 여기 한 줄만 토글
    // B *= -1.0f;

    // 3. TBN
    float3x3 TBN = float3x3(T, B, N);

    // 4. Tangent → World
    float3 nWS = normalize(mul(nTS, TBN));

    // GBuffer 출력
    Out.vDiffuse = albedo * 1.15f;
    Out.vNormal = float4(nWS * 0.5f + 0.5f, 0.f);
    Out.vDepth = float4(
        Input.vProjPos.z / Input.vProjPos.w,
        Input.vProjPos.w,
        0.f, 0.f
    
    );
    
    float3 Tmpcolor = saturate(Out.vDiffuse.rgb + float3(2.5, 0.0, 0.0));
    Out.vDiffuse.rgb = lerp(Out.vDiffuse.rgb, Tmpcolor, b_Damage);
    
    float flash = abs(sin(g_Time * 20));
    if (flash > 0.5)
        discard;


    return Out;
}

float4 PS_Eye(PS_IN Input) : SV_Target0
{
    return float4(1.f, 0.f, 0.f, 1.f);
    

}

PS_OUT PS_NonNormal(PS_IN Input)
{
    PS_OUT Out;
    vector vMtrlDiffuse = g_DiffuseTexture.Sample(DefaultSampler, Input.vTexcoord);
    if (vMtrlDiffuse.a < 0.5f)
        discard;
    
    vector vNormalDesc = vector(0.5f, 0.5f, 1.0f, 0.f);
    float2 vNormalXY = vNormalDesc.rg * 2.0f - 1.0f;
    float fNormalZ = sqrt(saturate(1.0f - dot(vNormalXY, vNormalXY)));
    
    vector vNormal = vector(vNormalXY.x, vNormalXY.y, fNormalZ, 0.0f);
    
    
    float3x3 WorldMatrix = float3x3(Input.vTangent.xyz, Input.vBinormal.xyz , Input.vNormal.xyz);
    vNormal = vector(mul(vNormal.xyz, WorldMatrix), 0.f);
    
    //밝기보정
    Out.vDiffuse = vMtrlDiffuse * 1.5f;
    Out.vNormal = vector(vNormal.xyz * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(Input.vProjPos.z / Input.vProjPos.w,
                        Input.vProjPos.w, 0.f, 0.f);
    
    
       
    float3 Tmpcolor = saturate(Out.vDiffuse.rgb + float3(2.5, 0.0, 0.0));
    Out.vDiffuse.rgb = lerp(Out.vDiffuse.rgb, Tmpcolor, b_Damage);
    
    float flash = abs(sin(g_Time * 20));
    if (flash > 0.5)
        discard;
    
    return Out;
    
}


PS_OUT PS_Alpha(PS_IN In)
{
    PS_OUT Out;
    
    float4 fDiffuseColor, fAmbientColor, fSpeculrColor;
    Out.vDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexcoord);
    
    
                        //0~1값으로 치환
    Out.vNormal = float4(In.vNormal.xyz * 0.5f + 0.5f, 0.f);

  
    
    float3 Tmpcolor = saturate(Out.vDiffuse.rgb + float3(2.5, 0.0, 0.0));
    Out.vDiffuse.a = g_Alpha;
    
    /*Depth_RenderTarget에 기록하자*/
    
    Out.vDepth = float4(In.vProjPos.z / In.vProjPos.w, ///0~1사이 
                        In.vPosition.w, //월드 z, 
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
        PixelShader = compile ps_5_0 PS_NonNormal();

    }

    
}