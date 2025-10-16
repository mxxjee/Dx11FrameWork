#include "CScreenQuad.h"
#include "CGameInstance.h"
#include "CTexture.h"
#include "CShader.h"
#include "CVIBuffer_Rect.h"

CScreenQuad::CScreenQuad(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    :CUI(pDevice,pContext)
{
}

CScreenQuad::CScreenQuad(const CScreenQuad& rhs)
    : CUI(rhs)
{
}

HRESULT CScreenQuad::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;


    return S_OK;
}

HRESULT CScreenQuad::Initialize_Copytype(void* pArg)
{
    if(FAILED(__super::Initialize_Copytype(pArg)))
        return E_FAIL;

    CreateBlendState();
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
}

HRESULT CScreenQuad::Render()
{

   
    ////렌더 할때 copydata로 GPU에게 데이터전송
    m_pGameInstance->Get_RenderShader()->Bind_Matrix("g_WorldMatrix", m_pTransformCom->Get_World((TransformScope::WORLD)));
    m_pGameInstance->Get_RenderShader()->Bind_Float("g_Brightness", 0.4f);

    if (m_pTexture)
        m_pTexture->Bind_ShaderResource(m_pGameInstance->Get_RenderShader(), "texture0", 0);

       

    ////IA단계
    m_pVIBufferCom->Bind_Resource();
  
  
    ////VS-PS
    m_pGameInstance->Get_RenderShader()->Begin("Brightness");

    ////OM단계
    Set_BlendState();
    m_pVIBufferCom->Render();      //OM단계
    return S_OK;
}



void CScreenQuad::Make_ScreenShot(CTexture* pTex)
{
    m_pTexture = pTex;
    Safe_AddRef(m_pTexture);
}

HRESULT CScreenQuad::CreateBlendState()
{
    D3D11_BLEND_DESC desc;
    memset(&desc, 0, sizeof(desc));
    desc.AlphaToCoverageEnable = false;
    desc.IndependentBlendEnable = false;

    //blendable =false여야지 완전히 덮어그림(블렌드하지않음)
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


void CScreenQuad::Set_BlendState()
{
    m_pContext.Get()->OMSetBlendState(m_RenderStates._BlendState.Get(), nullptr, 0xFFFFFFFF);

}

//void CScreenQuad::Set_ScreenTexture(UINT Flag)
//{
//    ComPtr<ID3D11Texture2D> pBackBuffer;
//    ComPtr<ID3D11Texture2D> pCopiedTex;
//
//    m_pGameInstance->Get_Buffer(&pBackBuffer, 0);
//
//    //텍스처생성
//    D3D11_TEXTURE2D_DESC desc{};
//    pBackBuffer->GetDesc(&desc);
//    desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
//    desc.Usage = D3D11_USAGE_DEFAULT;
//    desc.CPUAccessFlags = 0;
//
//    m_pDevice->CreateTexture2D(&desc, nullptr, pCopiedTex.GetAddressOf());
//
//
//    //GPU복사
//    m_pContext->CopyResource(pCopiedTex.Get(), pBackBuffer.Get());
//    
//    m_pTexture = new CTexture(m_pDevice) ;
//                m_pTexture->CreateResourceViewByTex(pCopiedTex);
//    Safe_AddRef(m_pTexture);
//}

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

}
