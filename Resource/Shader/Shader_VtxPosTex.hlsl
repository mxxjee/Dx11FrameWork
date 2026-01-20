

/*전역변수 : ID3D11EFFECT객체를 통해 값변경이 가능하지만,
HLSL 안에선 CONST화 되어 값 변경이 불가함 (읽기전용)*/
#include "Default.hlsli"
#include "Shader_Light.hlsli"
#include "Engine_Shader_Defines.hlsli"

float3 fBlueColor = float3(0.2, 0.4, 1.0);

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

struct PS_IN
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
    vPosition = mul(vPosition, g_ViewProjMatrix);
    
    
    //계산완료된 vPosition(x,y,z,w)중 w는 z값을 보관중이다.
    Out.vPosition = vPosition;
    Out.vTexcoord = In.vTexcoord;
    
    
   
    return Out;
    

}

//이후 RS단계에서 GPU가 내부적으로 w나누기 수행
float4 PS_MAIN(PS_IN Input) : SV_Target0
{
    float4 color = texture0.Sample(sampler0, Input.vTexcoord);

    color.a *= g_Alpha;
    return color;
}

float4 PS_Effect(PS_IN Input): SV_Target0
{
    float4 color = texture0.Sample(sampler0, Input.vTexcoord);
    
    
    color *= g_TintColor;
    color.rgb *= color.a;
    
    color *= g_Alpha;

    color.rgb*= g_fIntensity;
    
    
    return color;
}

float4 PS_Trail(PS_IN Input) : SV_Target0
{
    float2 vTexCoord = Input.vTexcoord;
    vTexCoord.x = Input.vTexcoord.y; // 원래 Y를 X로
    vTexCoord.y = Input.vTexcoord.x; // 원래 X를 Y로 
    
    float4 color = texture0.Sample(sampler0, vTexCoord);
    
    
    color *= g_TintColor;
    color.rgb *= color.a;
    
    color *= g_Alpha;
    color *= g_fIntensity;
    

    
    return color;
}

float4 PS_Slash(PS_IN Input) : SV_Target0
{
    float2 vTexCoord = Input.vTexcoord;
    vTexCoord.x = Input.vTexcoord.y; // 원래 Y를 X로
    vTexCoord.y = Input.vTexcoord.x; // 원래 X를 Y로 
    float4 vColor = texture0.Sample(DefaultSampler, vTexCoord);
    
    if ((1 - Input.vTexcoord.x) > g_Progress.x)
        vColor.a = 0.f;
    
    
    vColor*= g_TintColor;
    vColor.rgb *= vColor.a;
    
    vColor *= g_Alpha;

    
    return vColor;
}

float4 PS_RollCut(PS_IN Input) : SV_Target0
{
    float dist = distance(Input.vTexcoord, float2(0.5f, 0.5f));

    
    
    float2 center = float2(0.5f, 0.5f);
    float2 uv = Input.vTexcoord - center;
    
    float cosTheta = cos(g_Progress.x);
    float sinTheta = sin(g_Progress.x);
    
    float2 ResultUV;
    ResultUV.x = uv.x * cosTheta - uv.y * sinTheta;
    ResultUV.y = uv.x * sinTheta + uv.y * cosTheta;
    
    ResultUV += center;
    
   
    float4 vColor = texture0.Sample(DefaultSampler, ResultUV);
    float circleMask = 1.0f - smoothstep(0.4f, 0.5f, dist);
    vColor *= g_TintColor;
    vColor *= circleMask*g_Alpha;

    
    return vColor;
}

float4 PS_ChargeComplete(PS_IN Input) : SV_Target0
{
    float2 centeredUV = Input.vTexcoord - 0.5f; // 중심을 (0,0)으로 이동

    float dist = distance(Input.vTexcoord, float2(0.5f, 0.5f));

    if(dist<g_Progress.x)
        discard;
    
    float4 vColor = texture0.Sample(DefaultSampler, Input.vTexcoord);

    vColor *= g_TintColor;
    vColor *=  g_Alpha;

    
    return vColor;
}

float4 PS_MINIMAP(PS_IN Input) : SV_Target0
{
    float4 color = texture0.Sample(sampler0, Input.vTexcoord);

    return color;
}

float4 PS_BRIGHT(PS_IN Input) : SV_Target0
{
    float4 color = texture0.Sample(sampler0, Input.vTexcoord);
    color.rgb *= g_Brightness;

    return color;
   // return float4(1, 1, 0, 1); //
}

///////////////지햄의 여러 쉐이더 응용///////////
//밝기
float4 PS_SoftBright(PS_IN In) : SV_Target0
{
    float4 color = texture0.Sample(sampler0, In.vTexcoord);
    float fAv = ( color.r + color.g + color.b) / 4.f;
    
    
    if(fAv<=0.3f)
        color.rgb *= 2.f;
    
    return color;

}

//반전효과
float4 PS_InvertColor(PS_IN In) : SV_Target0
{
    float4 color = texture0.Sample(sampler0, In.vTexcoord);
    color.rgb = 1 - color.rgb;
    return color;

}

//거리에 따른 방사형밝기효과
float4 PS_RadialBright(PS_IN In) : SV_Target0
{
    float4 color = texture0.Sample(sampler0, In.vTexcoord);
    

    float fValue = distance(In.vTexcoord, float2(0.5f, 0.5f));
    
    color.rgb *= ((0.5f - fValue) * 3.f);
    return color;
    
}

//bloom 효과
float4 PS_THRESHOLD(PS_IN In) : SV_Target0
{
    float4 color = texture0.Sample(sampler0, In.vTexcoord);
    float fAv = (color.r + color.g + color.b) / 4.f;
    
    if(fAv>=0.5f)
        color = 1.f;
    

    return color;
}




float4 PS_GuasianBlur(PS_IN In) : SV_Target0
{
    float4 ColorSum = float4(0, 0, 0, 0);
    float WeightSum=0;
    
    float Weight = 0;
    float4 Color = 0;
    
    //상하좌우의 tex좌표 가져오기.. ( 블러할 범위정도..)
    float2 Location[8] =
    {
        float2(In.vTexcoord.x, In.vTexcoord.y - (OffSet.y / 720.f)), //상
       float2(In.vTexcoord.x, In.vTexcoord.y + (OffSet.y / 720.f)), //하
        float2(In.vTexcoord.x - (OffSet.x / 1280.f), In.vTexcoord.y), //좌
        float2(In.vTexcoord.x + (OffSet.x / 1280.f), In.vTexcoord.y), //우
        
        //대각선
        float2(In.vTexcoord.x - (OffSet.x / 1280.f), In.vTexcoord.y - (OffSet.y / 720.f)), //좌상단
        float2(In.vTexcoord.x - (OffSet.x / 1280.f), In.vTexcoord.y + (OffSet.y / 720.f)), //좌하단
        float2(In.vTexcoord.x + (OffSet.x / 1280.f), In.vTexcoord.y - (OffSet.y / 720.f)), //우상단
        float2(In.vTexcoord.x + (OffSet.x / 1280.f), In.vTexcoord.y + (OffSet.y / 720.f)) //우하단
        
    };
    

    for (int i = 0; i < 8;++i)
    {
        float2 Distance = In.vTexcoord - Location[i];
        Weight = exp(-((pow(Distance.x, 2) + pow(Distance.y, 2)) / (2 * pow(g_BlurValue, 2))));

        Color = texture0.Sample(sampler0, Location[i]) * Weight;
       
        ColorSum += Color;
        WeightSum += Weight;

    }
    
    return (ColorSum / WeightSum) * g_Brightness * g_TintColor;
    
 
}

float4 PS_Selected(PS_IN In) : SV_Target0
{
    float4 color = texture0.Sample(sampler0, In.vTexcoord);
    
    
    return color * float4(1.f, 0.f, 0.f, 0.9f);
    
}

//타이틀로고->알파값,마스크적용
float4 PS_Logo(PS_IN In) : SV_Target0
{
    float4 color = texture0.Sample(sampler0, In.vTexcoord);
    float WhiteLevel = saturate((color.r + color.g + color.b) / 3.f);
    
    
    float alpha = 1.f;

    alpha = 1-WhiteLevel;
    
    //시간기반 알파값(경계부분이 여기들어감)
    if(color.a>0.1f)
    {
        alpha *= g_Alpha;
        color.rgb = fBlueColor;
        color.a = alpha;
    }



    
    return color;
    
}

float4 PS_SaveSlot(PS_IN In) : SV_Target0
{
    float4 color = texture0.Sample(DefaultSampler, In.vTexcoord);
    float WhiteLevel = saturate((color.r + color.g + color.b) / 3.f);
    
    
    if(WhiteLevel<0.05f)
        color.a = 0.f;

    
    
    return color;
}
                       
float4 PS_GrayScale(PS_IN In):SV_Target0
{
    float4 color = texture0.Sample(DefaultSampler, In.vTexcoord);

    float3 weight = float3(0.299f, 0.587f, 0.114f);
    float gray = dot(color.rgb, weight);
    
    color.rgb = gray;
    
  
    return color;
}

float4 PS_Loading(PS_IN In) : SV_Target0
{
    float4 color = texture0.Sample(DefaultSampler, In.vTexcoord);
    //높이 = 1-In.vTexcoord.y
    
    if ((In.vTexcoord.x) > g_Progress.x)
        color. a = 0.f;
    
    
    
 
  
    return color;
}


/*렌더링 방법을 정의한다.*/
technique11 DefaultTechnique
{
    /*pass를 정의한다 : pass의 이름을 설정,
    이 pass가 선택되었으면 
    VertexShader는 이걸쓰고, PixelShader는 이걸쓸거에요.*/
    pass Default
    {
        SetRasterizerState(RS_UI);
        SetDepthStencilState(DSS_UI, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        //이 pass가 선택되면 VertexShader는 이렇게 컴파일하세요.
                                //버전 , 진입함수 설정
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();

    }

    pass Effect
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Alpha, 0);
        SetBlendState(BS_Blend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        //이 pass가 선택되면 VertexShader는 이렇게 컴파일하세요.
                                //버전 , 진입함수 설정
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_Effect();

    }

    pass Trail
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Alpha, 0);
        SetBlendState(BS_Blend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        //이 pass가 선택되면 VertexShader는 이렇게 컴파일하세요.
                                //버전 , 진입함수 설정
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_Trail();

    }

    pass RollCut
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Alpha, 0);
        SetBlendState(BS_Blend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        //이 pass가 선택되면 VertexShader는 이렇게 컴파일하세요.
                                //버전 , 진입함수 설정
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_RollCut();

    }

    pass ChargeComplete
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Alpha, 0);
        SetBlendState(BS_Blend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        //이 pass가 선택되면 VertexShader는 이렇게 컴파일하세요.
                                //버전 , 진입함수 설정
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_Effect();

    }

    pass Slash
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Alpha, 0);
        SetBlendState(BS_Blend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        //이 pass가 선택되면 VertexShader는 이렇게 컴파일하세요.
                                //버전 , 진입함수 설정
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_Slash();

    }

    pass Grid
    {
        SetRasterizerState(RS_WireFrame);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        //이 pass가 선택되면 VertexShader는 이렇게 컴파일하세요.
                                //버전 , 진입함수 설정
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();

    }



    pass SaveSlot
    {
        SetRasterizerState(RS_UI);
        SetDepthStencilState(DSS_UI, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_SaveSlot();
    }

    pass Logo
    {
        SetRasterizerState(RS_UI);
        SetDepthStencilState(DSS_UI, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_Logo();
    }

    pass Brightness
    {
                 
        SetRasterizerState(RS_UI);
        SetDepthStencilState(DSS_UI, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_BRIGHT();

    }


    pass Minimap
    {
        SetRasterizerState(RS_UI);
        SetDepthStencilState(DSS_UI, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

                                                               
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MINIMAP();

    }
    

//////////////////
    pass softBright
    {
        SetRasterizerState(RS_UI);
        SetDepthStencilState(DSS_UI, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_SoftBright();
    }

    pass Invert
    {
        SetRasterizerState(RS_UI);
        SetDepthStencilState(DSS_UI, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_InvertColor();
    }

    pass RadialBright
    {
        SetRasterizerState(RS_UI);
        SetDepthStencilState(DSS_UI, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_RadialBright();
    }

    pass Threshold
    {
        SetRasterizerState(RS_UI);
        SetDepthStencilState(DSS_UI, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_THRESHOLD();
    }

    pass Blur
    {
        SetRasterizerState(RS_UI);
        SetDepthStencilState(DSS_UI, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_GuasianBlur();
    }

    pass Select
    {
        SetRasterizerState(RS_UI);
        SetDepthStencilState(DSS_UI, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_Selected();
    }
    
    pass GrayScale
    {
        SetRasterizerState(RS_UI);
        SetDepthStencilState(DSS_UI, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_GrayScale();
    }

    pass Loading
    {
        SetRasterizerState(RS_UI);
        SetDepthStencilState(DSS_UI, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_Loading();
    }
}