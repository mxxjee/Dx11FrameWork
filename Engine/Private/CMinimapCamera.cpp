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

    GAMEOBJECT_DESC* pDesc = static_cast<GAMEOBJECT_DESC*>(pArg);

    m_pCameraCom = dynamic_cast<COrthographicCameraComponent*>(m_pGameInstance->Clone_Prototype
    (PROTOTYPE::COMPONENT, 0, PROTO_COMPONENT_NAME(L"OrthographicCamera"), pDesc));

    CheckNullResult(m_pCameraCom, E_FAIL);

    Safe_AddRef(m_pCameraCom);
    m_Components.emplace(COMPONENT_TYPE::ORTHOGRAPHIC_CAM, m_pCameraCom);

    if (FAILED(Create_RenderTagetview()))
        return E_FAIL;

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
}

void CMinimapCamera::Update_Late(_float fTimeDelta)
{
    __super::Update_Late(fTimeDelta);
    m_pCameraCom->Update_ViewMatrix(fTimeDelta);
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
    m_pCameraCom->Set_Target(m_pTarget);

}


void CMinimapCamera::Follow_Target(_float fTimeDelta)
{
    if (!m_pTarget)
        return;

    CTransform* pTargetTransform = static_cast<CTransform*>(m_pTarget->Get_Component(COMPONENT_TYPE::TRANSFORM));
    if (!m_pTransformCom || !pTargetTransform)
        return;

    const _vector TargetPos = pTargetTransform->Get_State(STATE::POSITION, TransformScope::WORLD);
    const _float3 Offset = m_pCameraCom->Get_OffSet();

    m_pTransformCom->Set_State(STATE::POSITION, TargetPos + XMLoadFloat3(&Offset));
    m_pTransformCom->LookAtWithUpVector(TargetPos,XMVectorSet(0.f,0.f,1.f,1.f));
    
    _float3 vNewUp;
    XMStoreFloat3(&vNewUp, XMVector3Normalize(m_pTransformCom->Get_State(STATE::UP)));
    m_pCameraCom->Set_Up(vNewUp);
}

HRESULT CMinimapCamera::Create_RenderTagetview()
{
    //렌더타겟 = (색상버퍼) + (깊이버퍼)
    //색상 버퍼 생성
    D3D11_TEXTURE2D_DESC texDesc{};
    texDesc.Width = 512;
    texDesc.Height = 512;
    texDesc.MipLevels = 1;
    texDesc.ArraySize = 1;
    texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    texDesc.SampleDesc.Count = 1;
    texDesc.Usage = D3D11_USAGE_DEFAULT;
    texDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;

    m_pDevice->CreateTexture2D(&texDesc, nullptr, m_tRenderTarget.pColorTex.GetAddressOf());
    m_pDevice->CreateRenderTargetView(m_tRenderTarget.pColorTex.Get(), nullptr, m_tRenderTarget.RTV.GetAddressOf());


    //깊이 텍스처 생성
    texDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    texDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    m_pDevice->CreateTexture2D(&texDesc, nullptr, m_tRenderTarget.pDepthTex.GetAddressOf());
    m_pDevice->CreateDepthStencilView(m_tRenderTarget.pDepthTex.Get(), nullptr, m_tRenderTarget.pDSV.GetAddressOf());

    return S_OK;
}

HRESULT CMinimapCamera::Bind_RenderTarget()
{
    if (FAILED(__super::Bind_RenderTarget()))
        return E_FAIL;

    //이전 상태 백업
    m_pContext->OMGetRenderTargets(1, m_tRenderTarget.PreRTV.GetAddressOf(), m_tRenderTarget.PreDSV.GetAddressOf());


    //카메라 렌더타겟 바인딩
    ID3D11RenderTargetView* pRTV = m_tRenderTarget.RTV.Get();
    ID3D11DepthStencilView* pDSV = m_tRenderTarget.pDSV.Get();

    m_pContext->OMSetRenderTargets(1, &pRTV, pDSV);

    return S_OK;
}

HRESULT CMinimapCamera::UnBind_RenderTarget()
{
    if (FAILED(__super::Bind_RenderTarget()))
        return E_FAIL;

    //이전 렌더타겟으로 다시복원.
    ID3D11RenderTargetView* pPreRTV = m_tRenderTarget.PreRTV.Get();
    ID3D11DepthStencilView* pPreDSV = m_tRenderTarget.PreDSV.Get();
    m_pContext->OMSetRenderTargets(1, &pPreRTV, pPreDSV);

    
    m_tRenderTarget.PreRTV.Reset();
    m_tRenderTarget.PreDSV.Reset();


    return S_OK;
}

HRESULT CMinimapCamera::Clear_RenderTargeView(const _float4* pClearColor)
{
    m_pContext->ClearRenderTargetView(m_tRenderTarget.RTV.Get(), reinterpret_cast<const _float*>(pClearColor));
    m_pContext->ClearDepthStencilView(m_tRenderTarget.pDSV.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
    
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
