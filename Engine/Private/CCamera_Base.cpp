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
   m_tRenderTarget(rhs.m_tRenderTarget),
    m_RenderMask(rhs.m_RenderMask)
{
}

HRESULT CCamera_Base::Initialize_Prototype()
{
  
    return S_OK;
}

HRESULT CCamera_Base::Initialize_Copytype(void* pArg)
{
    CAMERABASE_DESC* pDesc = static_cast<CAMERABASE_DESC*>(pArg);

    if(FAILED(__super::Initialize_Copytype(pArg)))
        return E_FAIL;

    /*DEsc값을 이용하여 멤버변수 채우기 ,렌더타겟뷰 생성*/
    if (FAILED(Ready_Resource(pArg)))
        return E_FAIL;

    m_pTransformCom->Set_State(STATE::POSITION, XMVectorSetW(XMLoadFloat3(&pDesc->vPosition), 1.f));
    m_pTransformCom->LookAt(XMVectorSetW(XMLoadFloat3(&pDesc->vAt), 1.f));
    
    return S_OK;
}


void CCamera_Base::Update_PipeLine()
{
    //pipeline에게 정보업데이트
    m_pGameInstance->Set_Transform(ENUM_TO_UINT(m_eCameraType), D3DTS::VIEW, m_pTransformCom->Get_WorldInverse(TransformScope::WORLD));

    if (m_bPerspective)
        m_pGameInstance->Set_Transform(ENUM_TO_UINT(m_eCameraType), D3DTS::PROJ, XMMatrixPerspectiveFovLH(XMConvertToRadians(m_fFovy), (m_fWidth/m_fHeight), m_fNearZ, m_fFarZ));

    else
        m_pGameInstance->Set_Transform(ENUM_TO_UINT(m_eCameraType), D3DTS::PROJ, XMMatrixOrthographicLH(m_fWidth, m_fHeight, m_fNearZ, m_fFarZ));

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



HRESULT CCamera_Base::Ready_Resource(void* pArg)
{
    m_RenderGroupMax = m_pGameInstance->Get_RenderGroupCount();
    m_RenderMask.resize(m_RenderGroupMax);

    for (int i = 0; i < m_RenderGroupMax; ++i)
        m_RenderMask[i] = true;


    CAMERABASE_DESC* pDesc = static_cast<CAMERABASE_DESC*>(pArg);

    m_eCameraType = pDesc->eCameraType;
    m_eCameraFlag = pDesc->eCameraFlag;

    m_fFovy = pDesc->fFovy;

    m_fWidth = pDesc->fWidth;
    m_fHeight = pDesc->fHeight;

    m_fNearZ = pDesc->fNear;
    m_fFarZ = pDesc->fFar;
    m_vOffset = pDesc->vOffset;

    m_vInitOffset = m_vOffset;


    m_bUseNewRenderTarget = pDesc->m_bCreateNewRenderTarget;

    if (FAILED(Create_RenderTagetview(pDesc->m_bCreateNewRenderTarget)))
        return E_FAIL;

    return S_OK;    
}

void CCamera_Base::Free()
{
    __super::Free();
    
    m_tRenderTarget.SRV.Reset();
    m_tRenderTarget.pColorTex.Reset();
    m_tRenderTarget.DSV.Reset();
  
    m_tRenderTarget.RTV.Reset();



}
