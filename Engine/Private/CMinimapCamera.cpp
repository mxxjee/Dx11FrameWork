#include "CMinimapCamera.h"
#include "COrthographicCameraComponent.h"
#include "CGameInstance.h"


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
