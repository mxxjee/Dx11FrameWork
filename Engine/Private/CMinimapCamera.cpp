#include "CMinimapCamera.h"
#include "COrthographicCameraComponent.h"
#include "CGameInstance.h"
#include "ColorUtils.h"


CMinimapCamera::CMinimapCamera(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    :CCamera_Base(pDevice,pContext)
{
}

CMinimapCamera::CMinimapCamera(const CMinimapCamera& rhs)
    :CCamera_Base(rhs)
{
}

HRESULT CMinimapCamera::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;


    return S_OK;
}

HRESULT CMinimapCamera::Initialize_Copytype(void* pArg)
{
    if (FAILED(__super::Initialize_Copytype(pArg)))
        return E_FAIL;

    m_bPerspective = false;

    return S_OK;
}

void CMinimapCamera::Update_Priority(_float fTimeDelta)
{
    __super::Update_Priority(fTimeDelta);
}

void CMinimapCamera::Update(_float fTimeDelta)
{
    __super::Update(fTimeDelta);

    Follow_Target(fTimeDelta);
    Update_PipeLine();
}

void CMinimapCamera::Update_Late(_float fTimeDelta)
{
    __super::Update_Late(fTimeDelta);
  
}

void CMinimapCamera::Update_Render(_float fTimeDelta)
{
    __super::Update_Render(fTimeDelta);
}

HRESULT CMinimapCamera::Render()
{
    __super::Render();


    return S_OK;
}

void CMinimapCamera::Set_Target(CGameObject* pTarget)
{
    m_pTarget = pTarget;

}


void CMinimapCamera::Follow_Target(_float fTimeDelta)
{
    if (!m_pTarget)
        return;

    CTransform* pTargetTransform = static_cast<CTransform*>(m_pTarget->Get_Component(COMPONENT_TYPE::TRANSFORM));
    if (!m_pTransformCom || !pTargetTransform)
        return;

    const _vector TargetPos = pTargetTransform->Get_State(STATE::POSITION, TransformScope::WORLD);

    m_pTransformCom->Set_State(STATE::POSITION, TargetPos + XMLoadFloat3(&m_vOffset));
    m_pTransformCom->LookAtWithUpVector(TargetPos,XMVectorSet(0.f,0.f,1.f,1.f));
    
    _float3 vNewUp;
    XMStoreFloat3(&vNewUp, XMVector3Normalize(m_pTransformCom->Get_State(STATE::UP)));
  //  m_pCameraCom->Set_Up(vNewUp);
}

HRESULT CMinimapCamera::Create_RenderTagetview(bool bUseDefault)
{

    if(FAILED(__super::Create_RenderTagetview(bUseDefault)))
        return E_FAIL;


    //렌더타겟 = (색상버퍼) + (깊이버퍼)
    //색상 버퍼 생성
    D3D11_TEXTURE2D_DESC texDesc{};
    texDesc.Width = m_pGameInstance->Get_EngineDesc().iWinSizeX;
    texDesc.Height = m_pGameInstance->Get_EngineDesc().iWinSizeY;
    texDesc.MipLevels = 1;
    texDesc.ArraySize = 1;
    texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    texDesc.SampleDesc.Count = 1;
    texDesc.Usage = D3D11_USAGE_DEFAULT;
    texDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;

    if(FAILED(m_pDevice->CreateTexture2D(&texDesc, nullptr, m_tRenderTarget.pColorTex.GetAddressOf())))
        return E_FAIL;

    if (FAILED(m_pDevice->CreateRenderTargetView(m_tRenderTarget.pColorTex.Get(), nullptr, m_tRenderTarget.RTV.GetAddressOf())))
        return E_FAIL;


    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.Format = texDesc.Format;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MostDetailedMip = 0;
    srvDesc.Texture2D.MipLevels = 1;

            //셰이더리소스뷰 만들기
    if (FAILED(m_pDevice->CreateShaderResourceView(m_tRenderTarget.pColorTex.Get(), &srvDesc, m_tRenderTarget.SRV.GetAddressOf())))
        return E_FAIL;



    //깊이 텍스처 생성
    ComPtr<ID3D11Texture2D>     m_DepthTex;
     
    D3D11_TEXTURE2D_DESC depthDesc = texDesc;
    depthDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

    if (FAILED(m_pDevice->CreateTexture2D(&depthDesc, nullptr, m_DepthTex.GetAddressOf())))
        return E_FAIL;

    if (FAILED(m_pDevice->CreateDepthStencilView(m_DepthTex.Get(), nullptr, m_tRenderTarget.DSV.GetAddressOf())))
        return E_FAIL;





 
  


    return S_OK;
}





CMinimapCamera* CMinimapCamera::Create(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pDeviceContext)
{
    CMinimapCamera* pInstance = new CMinimapCamera(_pDevice, _pDeviceContext);
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Create :CMinimapCamera ");
        Safe_Release(pInstance);

    }
    return pInstance;
}

CGameObject* CMinimapCamera::Clone(void* pArg)
{
    CMinimapCamera* pInstance = new CMinimapCamera(*this);
    if (FAILED(pInstance->Initialize_Copytype(pArg)))
    {
        MSG_BOX("Failed to Cloned :CMinimapCamera ");
        Safe_Release(pInstance);

    }
    return pInstance;
}

void CMinimapCamera::Free()
{
    __super::Free();
}
