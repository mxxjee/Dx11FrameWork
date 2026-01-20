
sampler DefaultSampler = sampler_state
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = wrap;
    AddressV = wrap;

};

sampler PointSampler = sampler_state
{
    Filter = MIN_MAG_MIP_POINT;
    AddressU = wrap;
    AddressV = wrap;

};

sampler MirrorSampler = sampler_state
{
    Filter = MIN_MAG_MIP_POINT;
    AddressU = mirror;
    AddressV = mirror;

};

SamplerState ShadowSampler
{
    Filter = MIN_MAG_MIP_POINT; // 혹은 MIN_MAG_MIP_LINEAR (부드럽게)
    
    // 범위 밖은 BorderColor를 사용해라!
    AddressU = Border;
    AddressV = Border;
    AddressW = Border;
    
    // 범위 밖은 무조건 1.0 (가장 먼 거리) -> "그림자 아님"으로 판정됨
    BorderColor = float4(1.0f, 1.0f, 1.0f, 1.0f);
};
///////RS//////
RasterizerState RS_Default
{
    FillMode = Solid;
    CullMode = NONE;
};

RasterizerState RS_UI
{
    FillMode = Solid;
    CullMode = NONE;
    DepthClipEnable = false;
};

RasterizerState RS_WireFrame
{
    FillMode = WIREFRAME;
    CullMode = NONE;
};

RasterizerState RS_Sky
{
    FillMode = Solid;
    CullMode = FRONT;
};



//////////////DSS/////////////
DepthStencilState DSS_Default
{
    DepthEnable = true;
    DepthWriteMask = all;
    DepthFunc = LESS_EQUAL;

};

DepthStencilState DSS_Alpha
{
    DepthEnable = true;
    DepthWriteMask = zero;
    

};

DepthStencilState DSS_NONE
{
    DepthEnable = false;
    DepthWriteMask = zero;
    

};


DepthStencilState DSS_UI
{
    DepthEnable = false;
    DepthWriteMask = zero;
    

};

DepthStencilState DSS_Sky
{
    DepthEnable = false;
    DepthWriteMask = all;
    

};


////////////////////BlendState////////////
BlendState BS_Default
{
    BlendEnable[0] = false;

};

BlendState BS_AlphaBlend
{
    BlendEnable[0] = true;
    BlendEnable[1] = true;

    SrcBlend = src_alpha;
    DestBlend = inv_src_alpha;
    BlendOp = add;
};

BlendState BS_Blend
{
    BlendEnable[0] = true;
    BlendEnable[1] = true;

    SrcBlend[0] = ONE;
    DestBlend[0] = ONE;
    BlendOp[0] = ADD;

    SrcBlend[1] = ONE;
    DestBlend[1] = ONE;
    BlendOp[1] = ADD;
};
