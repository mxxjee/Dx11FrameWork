

/*전역변수 : ID3D11EFFECT객체를 통해 값변경이 가능하지만,
HLSL 안에선 CONST화 되어 값 변경이 불가함 (읽기전용)*/
#include "Default.hlsli"
#include "Shader_Light.hlsli"
#include "Engine_Shader_Defines.hlsli"


Texture2D   g_Texture;
Texture2D   g_DiffuseTexture;
Texture2D   g_ShadeTexture;
Texture2D   g_NormalTexture;

matrix g_ViewMatrix;
matrix g_ProjMatrix;


vector g_vMtrlAmbient = vector(1.f, 1.f, 1.f, 1.f);

struct VS_IN
{
    float3 vPosition : POSITION;
    float2 vTexcoord : TEXCOORD0;
};

struct VS_OUT
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
};

/*버텍스 셰이더 단계의 함수
    버텍스 쉐이더 = 정점 갖고놀기
    정점과 행렬의 연산을 수 행*/
VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out;
    vector vPosition = mul(vector(In.vPosition, 1.f), g_WorldMatrix);
    vPosition = mul(vPosition, g_ViewMatrix);
    vPosition = mul(vPosition, g_ProjMatrix);
    
    
    //계산완료된 vPosition(x,y,z,w)중 w는 z값을 보관중이다.
    Out.vPosition = vPosition;
    Out.vTexcoord = In.vTexcoord;
    
    
   
    return Out;
    

}


struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
};

struct PS_OUT_BACKBUFFER
{
    vector vColor : SV_TARGET0;
};


struct PS_OUT_LIGHT
{
    vector vShade : SV_TARGET0;
  
};


PS_OUT_BACKBUFFER PS_MAIN_DEBUG(PS_IN In)
{
    PS_OUT_BACKBUFFER Out;
    Out.vColor = g_Texture.Sample(DefaultSampler, In.vTexcoord);
    return Out;
    

}

///Shade계산 시 쓰는 pass
PS_OUT_LIGHT PS_MAIN_DIRECTIONAL(PS_IN In)
{
    PS_OUT_LIGHT Out;
    
    float4 vNormalDesc = g_NormalTexture.Sample(DefaultSampler, In.vTexcoord);
    
    /* 0~1 -> -1~1 */
    float4 vNormal = float4(vNormalDesc.xyz * 2.f - 1.f, 0.f);
      

    vector vShade = max(dot(normalize(g_vLightDirection) * -1.f, vNormal), 0.f) + (g_vLightAmbient * g_vMtrlAmbient);
    Out.vShade = g_vLightDiffuse * saturate(vShade);
    
    return Out;
    
}

PS_OUT_BACKBUFFER PS_MAIN_COMBINED(PS_IN In)
{
    PS_OUT_BACKBUFFER Out;
    
    vector vDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexcoord);
   
    
    vector vShade = g_ShadeTexture.Sample(DefaultSampler, In.vTexcoord);
    
    float4 color = vDiffuse*vShade;
    color = pow(saturate(color), 1.0 / 1.2);

    Out.vColor = color;
    
    return Out;
    
}

                 
/*렌더링 방법을 정의한다.*/
technique11 DefaultTechnique
{
    /*pass를 정의한다 : pass의 이름을 설정,
    이 pass가 선택되었으면 
    VertexShader는 이걸쓰고, PixelShader는 이걸쓸거에요.*/
    pass Debug
    {
        SetRasterizerState(RS_UI);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        //이 pass가 선택되면 VertexShader는 이렇게 컴파일하세요.
                                //버전 , 진입함수 설정
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_DEBUG();

    }

    pass DirectionalLight
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_NONE, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        //이 pass가 선택되면 VertexShader는 이렇게 컴파일하세요.
                                //버전 , 진입함수 설정
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_DIRECTIONAL();

    }

    pass PointLight
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_NONE, 0); //Shade연산으로 그린 사각형 버퍼 이후에 NONLIGHT를 그려야하므로, 이 패스사용시에는 깊이버퍼를 기록하지않는다.
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        //이 pass가 선택되면 VertexShader는 이렇게 컴파일하세요.
                                //버전 , 진입함수 설정
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_DIRECTIONAL();

    }


    pass Combined
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_NONE, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        //이 pass가 선택되면 VertexShader는 이렇게 컴파일하세요.
                                //버전 , 진입함수 설정
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_COMBINED();

    }
}