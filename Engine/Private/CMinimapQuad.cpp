#include "CMinimapQuad.h"
#include "CShader.h"
#include "CTexture.h"
#include "CVIBuffer_Rect.h"
#include "CGameInstance.h"
#include "CMinimapCamera.h"

CMinimapQuad::CMinimapQuad(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    :CUI(pDevice, pContext)
{

}

CMinimapQuad::CMinimapQuad(const CMinimapQuad& rhs)
    :CUI(rhs)
{

}
HRESULT CMinimapQuad::CreateTexture(const RENDER_TARGET& m_Target)
{
    
    ComPtr<ID3D11Texture2D> pRTTexture;
    ComPtr<ID3D11Texture2D> pCopyTexture;


    //렌더타겟 텍스처화
    ComPtr<ID3D11Resource> pResource = nullptr;
    m_Target.RTV->GetResource(pResource.GetAddressOf());
    pResource.As(&pRTTexture);

    D3D11_TEXTURE2D_DESC desc{};
    pRTTexture->GetDesc(&desc);
    desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.CPUAccessFlags = 0;

    if (FAILED(m_pDevice->CreateTexture2D(&desc, nullptr, pCopyTexture.GetAddressOf())))
        return E_FAIL;

    

    if (!m_pTexture)
    {
        m_pContext->CopyResource(pCopyTexture.Get(), pRTTexture.Get());
        m_pTexture = CTexture::Create(m_pDevice, m_pContext, pCopyTexture);


    }
   


    return S_OK;
}

void CMinimapQuad::CreateBlendState()
{
    D3D11_BLEND_DESC desc;
    memset(&desc, 0, sizeof(desc));
    desc.AlphaToCoverageEnable = false;
    desc.IndependentBlendEnable = false;

    desc.RenderTarget[0].BlendEnable = FALSE;
    desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
    desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
    desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
    desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
    desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
    desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
    desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

    m_pDevice->CreateBlendState(&desc,m_BlendState.GetAddressOf());

}

HRESULT CMinimapQuad::Ready_Resources(void* pArg)
{
    if (m_pMinimapCamera)
    {
        ComPtr< ID3D11ShaderResourceView>  resource = m_pMinimapCamera->Get_RenderTarget().SRV;
        m_pTexture = CTexture::Create(m_pDevice, m_pContext, resource);

    }
    return S_OK;
}


HRESULT CMinimapQuad::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CMinimapQuad::Initialize_Copytype(void* pArg)
{
    if (FAILED(__super::Initialize_Copytype(pArg)))
        return E_FAIL;


    CGameObject* pCamera = m_pGameInstance->Find_Camera(CAMERA_TYPE::MINIMAP);
    m_pMinimapCamera = dynamic_cast<CMinimapCamera*>(pCamera);

    /*리소스관련 세팅*/
    if (FAILED(Ready_Resources(pArg)))
        return E_FAIL;


    g_Brightness = 0.5f;

    CreateBlendState();

    return S_OK;
}




void CMinimapQuad::Update_Priority(_float fTimeDelta)
{

    __super::Update_Priority(fTimeDelta);
}

void CMinimapQuad::Update(_float fTimeDelta)
{
    __super::Update(fTimeDelta);
}

void CMinimapQuad::Update_Late(_float fTimeDelta)
{
    __super::Update_Late(fTimeDelta);
}

void CMinimapQuad::Update_Render(_float fTimeDelta)
{
    __super::Update_Render(fTimeDelta);
}

HRESULT CMinimapQuad::Render()
{
    //////렌더 할때 copydata로 GPU에게 데이터전송
    //if (m_pMinimapCamera)
    //    CreateTexture(m_pMinimapCamera->Get_RenderTarget());

  
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;


    if (FAILED(m_pShader->Begin(m_passName)))
        return E_FAIL;


    if (FAILED(m_pVIBufferCom->Bind_Resource()))
        return E_FAIL;

    m_pContext->OMSetBlendState(m_BlendState.Get(), nullptr, 0xFFFFFFFF);

    if (FAILED(m_pVIBufferCom->Render()))
        return E_FAIL;

 

    return S_OK;
}

CMinimapQuad* CMinimapQuad::Create(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pDeviceContext)
{
    CMinimapQuad* pInstance = new CMinimapQuad(_pDevice, _pDeviceContext);
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Create :CMinimapQuad ");
        Safe_Release(pInstance);

    }


    return pInstance;
}

CGameObject* CMinimapQuad::Clone(void* pArg)
{
    CMinimapQuad* pInstance = new CMinimapQuad(*this);
    if (FAILED(pInstance->Initialize_Copytype(pArg)))
    {
        MSG_BOX("Failed to Cloned :CMinimapQuad ");
        Safe_Release(pInstance);

    }
    return pInstance;
}

void CMinimapQuad::Free()
{
    __super::Free();
}



HRESULT CMinimapQuad::Bind_ShaderResources()
{
    
    if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShader, "g_WorldMatrix")))
        return E_FAIL;

    /*안나오면ㅇ ㅣ거떄문임*/
	if (FAILED(m_pTexture->Bind_ShaderResource(m_pShader, "texture0",0)))
		return E_FAIL;

    if (FAILED(m_pShader->Bind_Float("g_Brightness", 0.5f)))
        return E_FAIL;

    return S_OK;
}