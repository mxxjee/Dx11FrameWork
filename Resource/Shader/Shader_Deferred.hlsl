

/*전역변수 : ID3D11EFFECT객체를 통해 값변경이 가능하지만,
HLSL 안에선 CONST화 되어 값 변경이 불가함 (읽기전용)*/
#include "Default.hlsli"
#include "Shader_Light.hlsli"
#include "Engine_Shader_Defines.hlsli"


Texture2D   g_Texture;
Texture2D   g_DiffuseTexture;
Texture2D   g_ShadeTexture;
Texture2D   g_NormalTexture;
Texture2D   g_DepthTexture;
Texture2D   g_SpecularTexture;

matrix g_ViewMatrix;
matrix g_ProjMatrix;


matrix g_ViewMatrixInv;
matrix g_ProjMatrixInv;


vector g_MainCamPosition;


vector g_vMtrlAmbient = vector(1.f, 1.f, 1.f, 1.f);
vector g_vMtrlSpecular = vector(1.f, 1.f, 1.f, 1.f);

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
    vector vSpecular : SV_TARGET1;
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
    
    

    

    vector vReflect = reflect(normalize(g_vLightDirection), vNormal);

    

    //픽셀에맞는 z갑 꺼내오기 

    vector vDepthDesc = g_DepthTexture.Sample(DefaultSampler, In.vTexcoord);

    

    vector vWorldPos;

    //specular 구하기

    //1.Look을 구한다(카메라위치에서 worldpos를 바라봄)

    //사각형 버퍼의 PS이므로 월드좌표를 모르기 때문에 다른 타겟으로부터 얻어온다.

    //월드좌표 먼저구하기

    

    

    //>>>>로컬 * 월드 * 뷰*투영*(/1.z) 까지 단계로 만들기<<<<

    //texcoord를 사용하여 NDC공간 상의 점으로변경

    vWorldPos.x = In.vTexcoord.x * 2.f - 1.f;

    vWorldPos.y = In.vTexcoord.y * (-2.f) + 1.f;

    vWorldPos.z = vDepthDesc.x;

    vWorldPos.w = 1.f;

    

    

    

    //>>>>로컬 * 월드 * 뷰*투영 까지 단계로 만들기<<<<

    vWorldPos *= vDepthDesc.y;

    

   //>>>>로컬 * 월드 * 뷰까지 단계로 만들기<<<<

    vWorldPos = mul(vWorldPos, g_ProjMatrixInv);



    

    //>>>>로컬 * 월드 까지 단계로 만들기<<<<

    vWorldPos = mul(vWorldPos, g_ViewMatrixInv);

    

    vector vLook = vWorldPos - g_CamPosition;

    Out.vSpecular =(g_vLightSpecular * g_vMtrlSpecular) * pow(max(dot(normalize(vReflect) * -1.f, normalize(vLook)), 0.f), 50.f);
    

    return Out;
    
}

PS_OUT_LIGHT PS_MAIN_POINT(PS_IN In)
{
    PS_OUT_LIGHT Out;

    float4 vNormalDesc = g_NormalTexture.Sample(DefaultSampler, In.vTexcoord);
    float4 vNormal = float4(vNormalDesc.xyz * 2.f - 1.f, 0.f);
      

    //픽셀에맞는 z갑 꺼내오기 
    vector vDepthDesc = g_DepthTexture.Sample(DefaultSampler, In.vTexcoord);
    
    vector vWorldPos;
    //specular 구하기
    //1.Look을 구한다(카메라위치에서 worldpos를 바라봄)
    //사각형 버퍼의 PS이므로 월드좌표를 모르기 때문에 다른 타겟으로부터 얻어온다.
    //월드좌표 먼저구하기
    
    
    //>>>>로컬 * 월드 * 뷰*투영*(/1.z) 까지 단계로 만들기<<<<
    //texcoord를 사용하여 NDC공간 상의 점으로변경
    vWorldPos.x = In.vTexcoord.x * 2.f - 1.f;
    vWorldPos.y = In.vTexcoord.y * (-2.f) + 1.f;
    vWorldPos.z = vDepthDesc.x;
    vWorldPos.w = 1.f;
    
    
    
    //>>>>로컬 * 월드 * 뷰*투영 까지 단계로 만들기<<<<
    vWorldPos *= vDepthDesc.y;
    
   //>>>>로컬 * 월드 * 뷰까지 단계로 만들기<<<<
    vWorldPos = mul(vWorldPos, g_ProjMatrixInv);

    
    //>>>>로컬 * 월드 까지 단계로 만들기<<<<
    vWorldPos = mul(vWorldPos, g_ViewMatrixInv);
    vector vLook = vWorldPos - g_MainCamPosition;
    
    for (int i = 0; i < g_PointLightNum;++i)
    {
        vector vLightDir = vWorldPos - g_vPL_Position[i];
        float fDistance = length(vLightDir);
        float fAtt = saturate((g_vPL_Range[i] - fDistance) / g_vPL_Range[i]);
        if(fAtt<=0.f)
            continue;
        
        vector vShade = max(dot(normalize(vLightDir) * -1.f, vNormal), 0.f) + (g_vPL_Ambient[i] * g_vMtrlAmbient);
        Out.vShade += g_vPL_Diffuse[i] * saturate(vShade) * fAtt;
        
        vector vReflect = reflect(normalize(vLightDir), vNormal);
       
        Out.vSpecular += fAtt * ((g_vPL_Specular[i] * g_vMtrlSpecular) * pow(max(dot(normalize(vReflect) * -1.f, normalize(vLook)), 0.f), 50.f));
        


    }
       
    return Out;
   
    
}

PS_OUT_BACKBUFFER PS_MAIN_COMBINED(PS_IN In)
{
    PS_OUT_BACKBUFFER Out;
    
    vector vDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexcoord);
   
    
    vector vShade = g_ShadeTexture.Sample(DefaultSampler, In.vTexcoord);
    vector vSpecular = g_SpecularTexture.Sample(DefaultSampler, In.vTexcoord);
    
    
    float4 color = vDiffuse*vShade+vSpecular;
    
    //밝기보정
    color = pow(saturate(color), 1.0 / 1.7);

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
        SetBlendState(BS_Blend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

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
        SetBlendState(BS_Blend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        //이 pass가 선택되면 VertexShader는 이렇게 컴파일하세요.
                                //버전 , 진입함수 설정
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_POINT();

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