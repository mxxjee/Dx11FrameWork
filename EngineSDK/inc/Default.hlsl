struct VS_INPUT
{
    float4 position:POSITION;
    float2 uv:TEXCOORD;
};


struct VS_OUTPUT
{
    float4 position : SV_POSITION;
    float2 uv:TEXCOORD;
};

//register : (<공간><번호>)
// 셰이더 리소스를 어느 슬롯에 바인딩할지 지정

//공간 종류
//  b# : ConstBuffer(상수버퍼)
//	t# : Texture/SRV
//	s# : Sampler
//	u# : UAV(Unordered Access View)

//번호 : 슬롯번호를 의미
cbuffer TransformData : register(b0)
{
    row_major Matrix matworld;
    row_major Matrix view;
    row_major Matrix proj;
}

VS_OUTPUT VS_Main(VS_INPUT input)
{
    VS_OUTPUT output;
    //float4 position = mul(input.position, matworld);
    //position = mul(position, view);
    //position = mul(position, proj);
    
    ////버퍼멤버들은 전역처럼 공유되므로, . 연산자 없이 바로사용가능
    //output.position = position;
    //output.uv = input.uv;

    output.position = input.position;
    output.uv = input.uv;
    
    return output;

}


Texture2D texture0 : register(t0); //texture0번 슬롯을 사용하겠음
SamplerState sampler0 : register(s0);


// RS 단계 수행이후..
//PS 함수
float4 PS(VS_OUTPUT input) : SV_Target
{
	//uv좌표값에 따라 텍스처 픽셀 설정
    float4 color = texture0.Sample(sampler0, input.uv);

    return color;

}


