#include "CCamera_Base.h"
#include "CGameInstance.h"
#include "CCameraComponent.h"
#include "CShader.h"


CCamera_Base::CCamera_Base(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    :CGameObject(pDevice,pContext)
{

}

CCamera_Base::CCamera_Base(const CCamera_Base& rhs)
    :CGameObject(rhs),
    m_pMainShader(rhs.m_pMainShader),m_pCameraCom(rhs.m_pCameraCom),
    m_GlobalViewProj(rhs.m_GlobalViewProj),m_tRenderTarget(rhs.m_tRenderTarget),
    m_RenderMask(rhs.m_RenderMask)
{
}

HRESULT CCamera_Base::Initialize_Prototype()
{
  
    return S_OK;
}

HRESULT CCamera_Base::Initialize_Copytype(void* pArg)
{
    if(FAILED(__super::Initialize_Copytype(pArg)))
        return E_FAIL;

    m_RenderGroupMax = m_pGameInstance->Get_RenderGroupCount();
    m_RenderMask.resize(m_RenderGroupMax);
    for (int i = 0; i < m_RenderGroupMax; ++i)
        m_RenderMask[i] = true;


    CAMERABASE_DESC* pDesc = static_cast<CAMERABASE_DESC*>(pArg);

    m_eCameraType = pDesc->eCameraType;
    m_eCameraFlag = pDesc->eCameraFlag;

    m_pMainShader = pDesc->pMainShader;
    Safe_AddRef(m_pMainShader);

    m_PassName = pDesc->PassName;

    m_GlobalViewProj = m_pMainShader->Get_ShaderInfo().m_GlobalViewProj;
    m_bUseNewRenderTarget = pDesc->m_bCreateNewRenderTarget;

    if(FAILED(Create_RenderTagetview(pDesc->m_bCreateNewRenderTarget)))
        return E_FAIL;

    return S_OK;
}


void CCamera_Base::Bind_ViewProjMatrix()
{
    //카메라 view/투영 세팅
    if (m_pMainShader)
    {
        _float4x4 viewproj;
        XMStoreFloat4x4(&viewproj, m_pCameraCom->Get_MulViewProjMatrix());
        m_GlobalViewProj->AsMatrix()->SetMatrix((float*)viewproj.m);
    }
}

HRESULT CCamera_Base::Create_RenderTagetview(bool bCreateRenderTarget)
{
    if (!bCreateRenderTarget)
    {
        //기존의 렌더타겟으로 세팅하기.
        m_tRenderTarget.RTV = m_pGameInstance->Get_BackBuffer_RTV();
        m_tRenderTarget.DSV = m_pGameInstance->Get_BackBuffer_DSV();


        //기존의 렌더타겟에서  colorTex가져오기
        ComPtr<ID3D11Resource> pResource = nullptr;
        m_tRenderTarget.RTV->GetResource(pResource.GetAddressOf());
        
        if(FAILED(pResource.As(&m_tRenderTarget.pColorTex)))
            return E_FAIL;



        //셰이더리소스뷰 만들기
        ComPtr<ID3D11Texture2D> pBackBuffer;
        if(FAILED(m_pGameInstance->Get_Buffer(&pBackBuffer, 0)))
            return E_FAIL;

        D3D11_TEXTURE2D_DESC desc;
        pBackBuffer->GetDesc(&desc);
        desc.BindFlags |= D3D11_BIND_SHADER_RESOURCE;
        desc.Usage = D3D11_USAGE_DEFAULT;

        ComPtr<ID3D11Texture2D> copyTex;
        m_pDevice->CreateTexture2D(&desc, nullptr, &copyTex);
        m_pContext->CopyResource(copyTex.Get(), pBackBuffer.Get());


        if(FAILED(m_pDevice->CreateShaderResourceView(copyTex.Get(), nullptr, m_tRenderTarget.SRV.GetAddressOf())))
            return E_FAIL;


    }

    return S_OK;
}

HRESULT CCamera_Base::Bind_RenderTarget()
{

    //카메라 렌더타겟 바인딩
    m_pContext->OMSetRenderTargets(1, m_tRenderTarget.RTV.GetAddressOf(), m_tRenderTarget.DSV.Get());


    return S_OK;
}

HRESULT CCamera_Base::UnBind_RenderTarget()
{


    if (!m_bUseNewRenderTarget) 
        return S_OK;

    
    ID3D11RenderTargetView* nullRTV[1] = { nullptr };
    m_pContext->OMSetRenderTargets(1, nullRTV, nullptr);
    m_pContext->OMSetRenderTargets(0, nullptr, nullptr);
    
    return S_OK;
}




HRESULT CCamera_Base::Clear_RenderTargetView(const _float4* pClearColor)
{
   
    if (!m_bUseNewRenderTarget)
        return S_OK; 

    m_pContext->ClearDepthStencilView(m_tRenderTarget.DSV.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
    m_pContext->ClearRenderTargetView(m_tRenderTarget.RTV.Get(), reinterpret_cast<const _float*>(pClearColor));
   

    return S_OK;
}

void CCamera_Base::Set_RenderAllRenderMask(bool bRender)
{
    for (int i = 0; i < m_RenderGroupMax; ++i)
        m_RenderMask[i] = bRender;

}



void CCamera_Base::Free()
{
    __super::Free();
    
    m_tRenderTarget.SRV.Reset();
    m_tRenderTarget.pColorTex.Reset();
    m_tRenderTarget.DSV.Reset();
  
    m_tRenderTarget.RTV.Reset();

    Safe_Release(m_pCameraCom);
    Safe_Release(m_pMainShader);

}
