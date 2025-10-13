#include "CCameraComponent.h"
#include "CTransform.h"
#include "CGameObject.h"

CCameraComponent::CCameraComponent(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    :CComponent(pDevice, pContext), m_matProj{}, m_matView{}
{
}

CCameraComponent::CCameraComponent(const CCameraComponent& Prototype)
    :CComponent(Prototype),
    m_matProj(Prototype.m_matProj),m_matView(Prototype.m_matView),m_vUp(Prototype.m_vUp)
{
}

HRESULT CCameraComponent::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CCameraComponent::Initialize_Copytype(void* pArg)
{
    if (FAILED(__super::Initialize_Copytype(pArg)))
        return E_FAIL;

    //수정필요!!
    COMPONENT_DESC* pCompDesc = static_cast<COMPONENT_DESC*>(pArg);
    CAMERACOMP_DESC* pDesc = static_cast<CAMERACOMP_DESC*>(pCompDesc->CameraDesc);

    m_vUp = pDesc->vUp;

    m_fNear = pDesc->fNear;
    m_fFar = pDesc->fFar;

    m_vOffSet = pDesc->vOffset;
    m_pTarget = pDesc->pTarget;

    m_bDynamic = pDesc->m_bDynamic;

    Update_ViewMatrix(0.f);

    return S_OK;
}

void CCameraComponent::Update_ViewMatrix(_float fTimeDelta)
{ 
    CheckFalse(m_bDynamic);

    CTransform* pTransform = dynamic_cast<CTransform*>(Get_Owner()->Get_Component(COMPONENT_TYPE::TRANSFORM));
    if (!pTransform) return;


    vEye = pTransform->Get_State(STATE::POSITION);
    vAt = vEye + XMVector3Normalize(pTransform->Get_State(STATE::LOOK));
    

    //if (m_pTarget)
    //{
    //    CTransform* pTargetTransform = dynamic_cast<CTransform*>(m_pTarget->Get_Component(L"Transform"));
    //    if (pTargetTransform)
    //    {
    //        vAt = pTargetTransform->Get_State(STATE::POSITION);
    //    }
    //}
    //타겟설정되었다면, 오프셋만큼 떨어져서보기.
  /*  if (m_pTarget)
    {
        CTransform* pTargetTransform = dynamic_cast<CTransform*>(m_pTarget->Get_Component(L"Transform"));
        vAt = pTargetTransform->Get_State(STATE::POSITION);
        
        _vector vQuat = pTransform->Get_SRT(SRTType::ROTATION);
        _matrix matRot = XMMatrixRotationQuaternion(vQuat);

        _vector vRotatedoffset = XMVector3TransformCoord(XMLoadFloat3(&m_vOffSet), matRot);


  
        vEye = vAt + vRotatedoffset;
        pTransform->Set_State(STATE::POSITION, vEye);
    }*/

    XMStoreFloat4x4(&m_matView, XMMatrixLookAtLH(vEye, vAt, XMLoadFloat3(&m_vUp)));
}

_matrix CCameraComponent::Get_MulViewProjMatrix()
{
    _matrix ProjMat = XMLoadFloat4x4(&m_matProj);
    _matrix ViewMat = XMLoadFloat4x4(&m_matView);

    return XMMatrixMultiply(ViewMat, ProjMat);
}


CCameraComponent* CCameraComponent::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
    CCameraComponent* pInstance = new CCameraComponent(pDevice, pContext);
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Create : CCameraComponent");
        Safe_Release(pInstance);
    }


    return pInstance;
}

CComponent* CCameraComponent::Clone(void* pArg)
{
    CCameraComponent* pInstance = new CCameraComponent(*this);
    if (FAILED(pInstance->Initialize_Copytype(pArg)))
    {
        MSG_BOX("Failed to Cloned : CCameraComponent");
        Safe_Release(pInstance);
    }


    return pInstance;
}

void CCameraComponent::Free()
{
    __super::Free();
}
