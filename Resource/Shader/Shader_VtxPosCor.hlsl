

/*전역변수 : ID3D11EFFECT객체를 통해 값변경이 가능하지만,
HLSL 안에선 CONST화 되어 값 변경이 불가함 (읽기전용)*/
#include "Default.hlsli"
#include "Shader_Light.hlsli"
#include "Engine_Shader_Defines.hlsli"

float4 g_Color = float4(1.f, 0.f, 0.f, 1.f);

struct VS_IN
{
    float3 vPosition : POSITION;
    float4 vColor : COLOR;
};

struct VS_OUT
{
    float4 vPosition : SV_POSITION;
    float4 vColor : COLOR;
};

struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float4 vColor : COLOR;
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
    Out.vColor = In.vColor;
    
    
   
    return Out;
    

}

//이후 RS단계에서 GPU가 내부적으로 w나누기 수행
float4 PS_MAIN(PS_IN Input) : SV_Target0
{
    return g_Color;
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

 
}