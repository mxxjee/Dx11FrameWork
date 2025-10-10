#include "CScreenQuad.h"
#include "CGameInstance.h"
#include "CTexture.h"
#include "CShader.h"
#include "CVIBuffer_Rect.h"

CScreenQuad::CScreenQuad(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    :CGameObject(pDevice,pContext)
{
}

CScreenQuad::CScreenQuad(const CScreenQuad& rhs)
    :CGameObject(rhs)
{
}

HRESULT CScreenQuad::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CScreenQuad::Initialize_Copytype(void* pArg)
{
    if(FAILED(__super::Initialize_Copytype(pArg)))
        return E_FAIL;

    //RectBuffer생성
    CBase* pBuffer = m_pGameInstance->Clone_Prototype(PROTOTYPE::COMPONENT, 0, PROTO_COMPONENT_NAME(L"VIBuffer_Rect"), pArg);
    if (pBuffer)
    {
        m_pVIBufferCom = dynamic_cast<CVIBuffer_Rect*>(pBuffer);

        if (m_pVIBufferCom)
        {
            m_pVIBufferCom->AddRef();
            m_Components.emplace(L"VIBuffer_Rect", m_pVIBufferCom);

        }

    }

    m_pTexShader = m_pGameInstance->Find_Shader(L"Brightness");
    Safe_AddRef(m_pTexShader);

    m_eRenderGroup = RENDERGROUP::NONALPHA;

    CreateRasterizerState();
    CreateSamplerState();
    CreateBlendState();

    m_pTexture = new CTexture(m_pDevice);

    return S_OK;
}

void CScreenQuad::Update_Priority(_float fTimeDelta)
{
    __super::Update_Priority(fTimeDelta);
}

void CScreenQuad::Update(_float fTimeDelta)
{
    __super::Update(fTimeDelta);

}

void CScreenQuad::Update_Late(_float fTimeDelta)
{
    __super::Update_Late(fTimeDelta);
}

void CScreenQuad::Update_Render(_float fTimeDelta)
{
    __super::Update_Render(fTimeDelta);
    m_pGameInstance->Add_RenderObject(m_eRenderGroup, this);

}

HRESULT CScreenQuad::Render()
{
    __super::Render();
  
    //렌더 할때 copydata로 GPU에게 데이터전송
    m_pTexShader->SetMatrix("g_WorldMatrix", m_pTransformCom->Get_World((TransformScope::WORLD)));

    _float4x4 _fViewProj;
    XMStoreFloat4x4(&_fViewProj, m_pGameInstance->GetMulViewProjMatrix(true));
     

    m_pTexShader->SetMatrix("g_ViewProjMatrix", _fViewProj);
    m_pTexShader->SetFloat("g_Brightness", 0.4f);

    m_pTexShader->SetResource("texture0", m_pTexture->GetComPtr());  

    //IA단계
    m_pVIBufferCom->Bind_Resource();
    Set_RasterizerState();
    //VS-PS
    m_pTexShader->Apply();

    //OM단계
    m_pVIBufferCom->Render();      //OM단계
    return S_OK;
}

HRESULT CScreenQuad::CreateRasterizerState()
{
    D3D11_RASTERIZER_DESC desc;
    memset(&desc, 0, sizeof(desc));

    desc.FillMode = D3D11_FILL_SOLID;//WIREFRAME of SOLID
    desc.CullMode = D3D11_CULL_NONE;//CULLMODE: 반시계 컬링
    desc.FrontCounterClockwise = false;
    desc.DepthClipEnable = false;

    HRESULT hr = m_pDevice->CreateRasterizerState(&desc, m_RenderStates._rasterizerState.GetAddressOf());
    CHECK(hr);

    return S_OK;
}

HRESULT CScreenQuad::CreateSamplerState()
{
    D3D11_SAMPLER_DESC desc;
    memset(&desc, 0, sizeof(desc));
    desc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
    desc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
    desc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
    desc.BorderColor[0] = 0.0f;
    desc.BorderColor[1] = 0.0f;
    desc.BorderColor[2] = 0.0f;
    desc.BorderColor[3] = 0.0f;


    desc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
    desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    desc.MaxAnisotropy = 16;
    desc.MaxLOD = FLT_MAX;
    desc.MinLOD = FLT_MIN;
    desc.MipLODBias = 0.0f;

    m_pDevice->CreateSamplerState(&desc, m_RenderStates._samplerState.GetAddressOf());

    return S_OK;
}

HRESULT CScreenQuad::CreateBlendState()
{
    D3D11_BLEND_DESC desc;
    memset(&desc, 0, sizeof(desc));
    desc.AlphaToCoverageEnable = false;
    desc.IndependentBlendEnable = false;

    desc.RenderTarget[0].BlendEnable = false;
    desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
    desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
    desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
    desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
    desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
    desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
    desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

    HRESULT hr = m_pDevice->CreateBlendState(&desc, m_RenderStates._BlendState.GetAddressOf());
    CHECK(hr);
    return S_OK;
}

void CScreenQuad::Set_ScreenTexture(UINT iFlag)
{
    ComPtr<ID3D11Texture2D> pBackBuffer;
    ComPtr<ID3D11Texture2D> pCopiedTex;

    m_pGameInstance->Get_Buffer(&pBackBuffer,0);

    //텍스처생성
    D3D11_TEXTURE2D_DESC desc{};
    pBackBuffer->GetDesc(&desc);
    desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.CPUAccessFlags = 0;

    m_pDevice->CreateTexture2D(&desc, nullptr, pCopiedTex.GetAddressOf());


    //GPU복사
    m_pContext->CopyResource(pCopiedTex.Get(), pBackBuffer.Get());

    m_pTexture->CreateResourceViewByTex(pCopiedTex);



}

void CScreenQuad::Set_RasterizerState()
{
    m_pContext.Get()->RSSetState(m_RenderStates._rasterizerState.Get());

}

void CScreenQuad::Set_BlendState()
{
    m_pContext.Get()->OMSetBlendState(m_RenderStates._BlendState.Get(), nullptr, 0xFFFFFFFF);

}

CScreenQuad* CScreenQuad::Create(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pDeviceContext)
{
    CScreenQuad* pInstance = new CScreenQuad(_pDevice, _pDeviceContext);
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Create :CScreenQuad ");
        Safe_Release(pInstance);

    }



    return pInstance;
}

CGameObject* CScreenQuad::Clone(void* pArg)
{
    CScreenQuad* pInstance = new CScreenQuad(*this);
    if (FAILED(pInstance->Initialize_Copytype(pArg)))
    {
        MSG_BOX("Failed to Cloned :CScreenQuad ");
        Safe_Release(pInstance);

    }
    return pInstance;
}

void CScreenQuad::Free()
{
    __super::Free();
    Safe_Release(m_pTexShader);
    Safe_Release(m_pTexture);
    Safe_Release(m_pVIBufferCom);
}
