

/*전역변수 : ID3D11EFFECT객체를 통해 값변경이 가능하지만,
HLSL 안에선 CONST화 되어 값 변경이 불가함 (읽기전용)*/
#include "Default.hlsli"
#include "Shader_Light.hlsli"


struct VS_IN
{
    float3 vPosition : POSITION;
   
    
    row_major float4x4 TransformationMatrix : WORLD; //행렬을 사용할 수 있는 시멘틱
    
    
    float2 vLifeTime : TEXCOORD0;
    

};

struct VS_OUT
{
    float4 vPosition : POSITION;
    float2 vPSize : PSIZE;
    float2 vLifeTime : TEXCOORD0;
    
};

///GS단계,,고로 VS _ OUT까지는 투영전단계

/*버텍스 셰이더 단계의 함수
    버텍스 쉐이더 = 정점 갖고놀기
    정점과 행렬의 연산을 수 행*/

VS_OUT VS_MAIN(VS_IN In) 
{
    VS_OUT Out;
    vector vPosition = mul(vector(In.vPosition, 1.f), In.TransformationMatrix);
    
    Out.vPosition = mul(vPosition, g_WorldMatrix);
    
    //픽셀사이즈 : 가로 행 길이 x 세로행 길이
    Out.vPSize = float2(length(In.TransformationMatrix._11_12_13), length(In.TransformationMatrix._21_22_23));
    Out.vLifeTime = In.vLifeTime;
    
    return Out;
    

}

struct GS_IN
{
    float4 vPosition : POSITION;
    float2 vPSize : PSIZE; 
    float2 vLifeTime : TEXCOORD0;
    
};

//투영이후의 점
struct GS_OUT
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
    float2 vLifeTime : TEXCOORD1;
    
};

//점1개씩 꺼내서 삼각형을 만든다.
[maxvertexcount(6)]
void GS_MAIN(point GS_IN In[1], inout TriangleStream<GS_OUT> OutStream)
{
    GS_OUT Out[4];
    
    float3 vLook = (g_CamPosition - In[0].vPosition).xyz;
    float3 vRight = normalize(cross(float3(0.f, 1.f, 0.f), vLook)) * In[0].vPSize.x;
    float3 vUp = normalize(cross(vLook, vRight)) * In[0].vPSize.y;
    
    matrix matVP = g_ViewProjMatrix;
    
    Out[0].vPosition = mul(vector(In[0].vPosition.xyz + vRight + vUp, 1.f), matVP);
    Out[0].vTexcoord = float2(0.f, 0.f);
    Out[0].vLifeTime = In[0].vLifeTime;
    
    Out[1].vPosition = mul(vector(In[0].vPosition.xyz - vRight + vUp, 1.f), matVP);
    Out[1].vTexcoord = float2(1.f, 0.f);
    Out[1].vLifeTime = In[0].vLifeTime;
    
    
    Out[2].vPosition = mul(vector(In[0].vPosition.xyz  - vRight - vUp, 1.f), matVP);
    Out[2].vTexcoord = float2(1.f, 1.f);
    Out[2].vLifeTime = In[0].vLifeTime;
    
    
    Out[3].vPosition = mul(vector(In[0].vPosition.xyz + vRight - vUp, 1.f), matVP);
    Out[3].vTexcoord = float2(0.f, 1.f);
    Out[3].vLifeTime = In[0].vLifeTime;
    
    
    OutStream.Append(Out[0]);
    OutStream.Append(Out[1]);
    OutStream.Append(Out[2]);
    OutStream.RestartStrip();
    
    
    OutStream.Append(Out[0]);
    OutStream.Append(Out[2]);
    OutStream.Append(Out[3]);
    OutStream.RestartStrip();
    

}

struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
    float2 vLifeTime : TEXCOORD1;
};

struct PS_OUT
{
    vector vColor : SV_TARGET0;
    
};

//이후 RS단계에서 GPU가 내부적으로 w나누기 수행
PS_OUT PS_MAIN(PS_IN Input) : SV_Target0
{
    PS_OUT Out;
    float4 color = texture0.Sample(sampler0, Input.vTexcoord);

    //alphatest
    if(color.a<=0.3f)
        discard;
    
    color.a = saturate(Input.vLifeTime.y - Input.vLifeTime.x);
    color.rgb = (1.f - color.rgb) + (1.f - color.a);
    Out.vColor = color;
    
    
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
        //이 pass가 선택되면 VertexShader는 이렇게 컴파일하세요.
                                //버전 , 진입함수 설정
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_MAIN();
        PixelShader = compile ps_5_0 PS_MAIN();

    }

}