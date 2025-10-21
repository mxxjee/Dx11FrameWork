

/*전역변수 : ID3D11EFFECT객체를 통해 값변경이 가능하지만,
HLSL 안에선 CONST화 되어 값 변경이 불가함 (읽기전용)*/
#include "Default.hlsli"


vector g_CamPosition;

//////임시로 정의해놓은 조명의 값//////////
vector g_vLightDirection = vector(1.f, -1.f, 1.f, 0.f);
vector g_vLightDiffuse = vector(1.f, 1.f, 1.f, 1.f);
vector g_vLightAmbient = vector(1.f, 1.f, 1.f, 1.f);
vector g_vLightSpecular = vector(1.f, 1.f, 1.f, 1.f);


Texture2D g_DiffuseTexture;
vector g_vMaterialAmbient = vector(0.3f, 0.3f, 0.3f, 1.f);
vector g_vMaterialSpecular = vector(1.f, 1.f, 1.f, 1.f);



////////임시로 정해놓은 오브젝트의 메테리얼값, 실제는 텍스처를 읽어서 처리해야함

struct VS_IN
{
    float3 vPosition : POSITION;
    float3 vNormal : NORMAL;
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
    Out.vWorldPos = Out.vPosition;
    //sdfsdf
   
    return Out;
    

}

//이후 RS단계에서 GPU가 내부적으로 w나누기 수행
float4 PS_MAIN(PS_IN Input) : SV_Target0
{
    //DiffuseColor
    float4 DiffuseColor = g_DiffuseTexture.Sample(sampler0, Input.vTexcoord);
    float fShade = max(dot(normalize(g_vLightDirection) * (-1.f), Input.vNormal), 0); //0이하이면 0, 1이상이면 1값으로 보정
    
    //specular 세기 = 반사벡터를 구해서  카메라 시야벡터 * (-1)와 내적
    vector vLook = Input.vWorldPos - g_CamPosition;
    vector Reflect = reflect(normalize(g_vLightDirection), normalize(Input.vNormal));
    float fSpecular = pow(max(dot(Reflect, normalize(vLook) * (-1.f)), 0),50);
    

       
    return g_vLightDiffuse * DiffuseColor * //diffuse속성끼리 계산
    saturate(fShade + (g_vLightAmbient * g_vMaterialAmbient)) + //음영 + 최소음영 
     (g_vLightSpecular * g_vMaterialSpecular) * fSpecular; //specular계산
    
  
    
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

    
}