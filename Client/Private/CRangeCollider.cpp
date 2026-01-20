#include "CRangeCollider.h"
#include "CSphereColliderComponent.h"
#include "CGameInstance.h"
#include "Client_Defines.h"
#include "CBounding_Sphere.h"



USING(Client)
CRangeCollider::CRangeCollider(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    :CPartObject(pDevice,pContext)
{
}

CRangeCollider::CRangeCollider(const CRangeCollider& Prototype)
    : CPartObject(Prototype)
{
}

HRESULT CRangeCollider::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CRangeCollider::Initialize_Copytype(void* pArg)
{
    RANGE_COLLIDER_DESC* pDesc = static_cast<RANGE_COLLIDER_DESC*>(pArg);


    if (FAILED(__super::Initialize_Copytype(pArg)))
        return E_FAIL;


    if (FAILED(Ready_Component(pArg)))
        return E_FAIL;



    return S_OK;
}

void CRangeCollider::Update_Priority(_float fTimeDelta)
{
    __super::Update_Priority(fTimeDelta);
}

void CRangeCollider::Update(_float fTimeDelta)
{
    __super::SetUp_CombinedWorldMatrix(XMLoadFloat4x4(m_pParentMatrix));

}

void CRangeCollider::Update_Late(_float fTimeDelta)
{
    CheckNull(m_pCollider);
    m_pCollider->Update_Collider(XMLoadFloat4x4(&m_CombinedWorldMatrix));

}

void CRangeCollider::Update_Render(_float fTimeDelta)
{
    m_pGameInstance->Add_RenderObject(ENUM_TO_UINT(RENDERGROUP::NONALPHA), this);

#ifdef _DEBUG
    if (FAILED(m_pGameInstance->Add_DebugComponent(m_pCollider)))
        return;

#endif
}

HRESULT CRangeCollider::Render()
{
    return S_OK;
}

void CRangeCollider::Register_Colliders(_uint iLevelIdx)
{

}

HRESULT CRangeCollider::Ready_Component(void* pArg)
{
    RANGE_COLLIDER_DESC* pDesc = static_cast<RANGE_COLLIDER_DESC*>(pArg);

    CCollider_Base::COLLIDER_DESC pColliderDesc;
    pColliderDesc.m_eColGroup = ENUM_TO_UINT(COLLISION_GROUP::TRIGGER);
    pColliderDesc.pOwner = this;
    pColliderDesc.m_iLevelID = pDesc->m_iLevelID;

    CBounding_Sphere::BOUNDING_SPHERE_DESC      CollDesc{};
    CollDesc.fRadius = pDesc->fRadius;
    pColliderDesc.m_iLevelID = pDesc->m_iLevelID;
   
    pColliderDesc.m_BoundingDesc = &CollDesc;



    m_pCollider = dynamic_cast<CSphereColliderComponent*>(m_pGameInstance->Clone_Prototype(
        PROTOTYPE::COMPONENT,
        0,
        PROTO_COMPONENT_NAME(L"SphereCollider"),
        &pColliderDesc)
        );

 
    return S_OK;
}

void CRangeCollider::Set_Active(bool _b)
{
    m_pCollider->Set_Active(_b);

    if (_b == false)
    {
        m_bCollision = false;

    }
}

void CRangeCollider::Set_Radius(_float radius)
{
    m_pCollider->Set_Radius(radius);
}

void CRangeCollider::OnCollisionEnter(_uint iGroup, CCollider_Base* pOther)
{
    CGameObject* pOwner = pOther->Get_Owner();
    CheckNull(pOwner);

    switch (COLLISION_GROUP(iGroup))
    {
    case COLLISION_GROUP::PLAYER:
    {
        m_bCollision = true;
    }

    break;
    }
}

void CRangeCollider::OnCollisionStay(_uint iGroup, CCollider_Base* pOther)
{
}

void CRangeCollider::OnCollisionExit(_uint iGroup, CCollider_Base* pOther)
{
}

CRangeCollider* CRangeCollider::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
    CRangeCollider* pInstance = new CRangeCollider(pDevice, pContext);
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Create :CRangeCollider ");
        Safe_Release(pInstance);

    }



    return pInstance;
}

CGameObject* CRangeCollider::Clone(void* pArg)
{
    CRangeCollider* pInstance = new CRangeCollider(*this);
    if (FAILED(pInstance->Initialize_Copytype(pArg)))
    {
        MSG_BOX("Failed to Clone :Cinteraction_Statue ");
        Safe_Release(pInstance);

    }

    return pInstance;
}

void CRangeCollider::Free()
{
    Safe_Release(m_pCollider);
    __super::Free();


}
