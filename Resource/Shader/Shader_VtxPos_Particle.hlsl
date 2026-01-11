

/*전역변수 : ID3D11EFFECT객체를 통해 값변경이 가능하지만,
HLSL 안에선 CONST화 되어 값 변경이 불가함 (읽기전용)*/
#include "Default.hlsli"
#include "Shader_Light.hlsli"
#include "Engine_Shader_Defines.hlsli"

Texture2D g_DiffuseTexture;
Texture2D g_NoiseTexture;

struct VS_IN
{
    float3 vPosition : POSITION;
   
    
    row_major float4x4 TransformationMatrix : WORLD; //행렬을 사용할 수 있는 시멘틱
    
    
    float2 vLifeTime : TEXCOORD0;
    float4 vColor : TEXCOORD1;
    float4 vUV_Info : TEXCOORD2;

};

struct VS_OUT
{
    float4 vPosition : POSITION;
    float2 vPSize : PSIZE;
    float2 vLifeTime : TEXCOORD0;
    float4 vColor : TEXCOORD1;
    float4 vUV_Info : TEXCOORD2;
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
    Out.vColor = In.vColor;
    Out.vUV_Info = In.vUV_Info;
    return Out;
    

}

struct GS_IN
{
    float4 vPosition : POSITION;
    float2 vPSize : PSIZE; 
    float2 vLifeTime : TEXCOORD0;
    float4 vColor : TEXCOORD1;
    float4 vUV_Info : TEXCOORD2;
};

//투영이후의 점
struct GS_OUT
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
    float2 vLifeTime : TEXCOORD1;
    float4 vColor : COLOR0;
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
    
    float2 uvOffset = In[0].vUV_Info.xy;
    float2 uvScale = In[0].vUV_Info.zw;
    
    Out[0].vPosition = mul(vector(In[0].vPosition.xyz + vRight + vUp, 1.f), matVP);
    Out[0].vTexcoord = uvOffset + float2(0.f, 0.f) * uvScale;
    Out[0].vLifeTime = In[0].vLifeTime;
    Out[0].vColor = In[0].vColor; //색상
    
    Out[1].vPosition = mul(vector(In[0].vPosition.xyz - vRight + vUp, 1.f), matVP);
    Out[1].vTexcoord = uvOffset + float2(1.f, 0.f) * uvScale;
    Out[1].vLifeTime = In[0].vLifeTime;
    Out[1].vColor = In[0].vColor; //색상
    
    
    Out[2].vPosition = mul(vector(In[0].vPosition.xyz  - vRight - vUp, 1.f), matVP);
    Out[2].vTexcoord = uvOffset + float2(1.f, 1.f) * uvScale;
    Out[2].vLifeTime = In[0].vLifeTime;
    Out[2].vColor = In[0].vColor; //색상
    
    
    Out[3].vPosition = mul(vector(In[0].vPosition.xyz + vRight - vUp, 1.f), matVP);
    Out[3].vTexcoord = uvOffset + float2(0.f, 1.f) * uvScale;
    Out[3].vLifeTime = In[0].vLifeTime;
    Out[3].vColor = In[0].vColor;
    
    
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
    float4 vColor : COLOR0;
};

struct PS_OUT
{
    vector vColor : SV_TARGET0;
    
};

//이후 RS단계에서 GPU가 내부적으로 w나누기 수행
PS_OUT PS_MAIN(PS_IN Input) 
{
    PS_OUT Out;
    float4 color = g_DiffuseTexture.Sample(DefaultSampler, Input.vTexcoord);
    color.a = color.r;
    
    
    color *= Input.vColor;
    if (color.a <= 0.01f)
        discard;
    
    color.rgb *= g_TintColor;
    color.rgb *= g_fIntensity;
    
    Out.vColor = color;
    
    return Out;
}

PS_OUT PS_Smoke(PS_IN Input)
{
    PS_OUT Out;

    // 1. 노이즈 및 왜곡 (기존과 동일)
    float2 noiseUV = Input.vTexcoord + float2(g_Time * 0.1f, g_Time * 0.2f);
    float4 noiseColor = g_NoiseTexture.Sample(DefaultSampler, noiseUV);
    float2 distortedUV = Input.vTexcoord + (noiseColor.rg - 0.5f) * 0.1f;
    
    // 2. 원본 구름 모양 샘플링
    float4 baseColor = g_DiffuseTexture.Sample(DefaultSampler, distortedUV);
    float shapeAlpha = baseColor.r; // 혹은 baseColor.r (텍스처에 따라 다름)

    // --------------------------------------------------------------------
    // [핵심 변경] Dissolve (침식) 로직
    // --------------------------------------------------------------------
    
    // C++에서 보내주는 알파값 (1.0 -> 0.0으로 줄어듦)
    float particleLife = Input.vColor.a;

    // 침식 기준값 (Threshold) 
    // 파티클이 죽어갈수록(Life가 0이 될수록) 기준값이 1로 높아져서 더 많이 깎아먹음
    float dissolveThreshold = 1.0f - particleLife;

    // [중요] smoothstep(최소, 최대, 값)
    // 노이즈 값(noiseColor.r)이 dissolveThreshold보다 작으면 투명해지고(0),
    // 크면 불투명해집니다(1). +0.1f는 경계면을 부드럽게 만들기 위함입니다.
    float erosionAlpha = smoothstep(dissolveThreshold, dissolveThreshold + 0.15f, noiseColor.r);

    // --------------------------------------------------------------------

    // 3. 최종 알파 조합
    // 원본 모양(shapeAlpha) * 침식된 결과(erosionAlpha)
    // 이렇게 해야 사각형 테두리는 안 보이고, 안쪽에서만 구멍이 뚫립니다.
    Out.vColor = baseColor;
    Out.vColor.a = shapeAlpha * erosionAlpha;

    // 4. 색상 적용
    Out.vColor.rgb *= g_TintColor;
    Out.vColor.rgb *= g_fIntensity;
    Out.vColor.rgb *= Input.vColor.rgb;

    // 5. 알파 테스트
    if (Out.vColor.a <= 0.01f)
        discard;

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
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        //이 pass가 선택되면 VertexShader는 이렇게 컴파일하세요.
                                //버전 , 진입함수 설정
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_MAIN();
        PixelShader = compile ps_5_0 PS_MAIN();

    }

    pass Smoke
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Alpha, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        //이 pass가 선택되면 VertexShader는 이렇게 컴파일하세요.
                                //버전 , 진입함수 설정
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_MAIN();
        PixelShader = compile ps_5_0 PS_Smoke();

    }

}