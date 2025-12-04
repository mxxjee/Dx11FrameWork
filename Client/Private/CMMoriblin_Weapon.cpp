#include "CMMoriblin_Weapon.h"
#include "Client_Defines.h"
#include "CCollider_Base.h"
#include "CSphereColliderComponent.h"
#include "CBounding_Sphere.h"
#include "CM_MoriblinSword.h"



USING(Client)
CMMoriblin_Weapon::CMMoriblin_Weapon(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    :CWeapon(pDevice,pContext)
{
}

CMMoriblin_Weapon::CMMoriblin_Weapon(const CMMoriblin_Weapon& Prototype)
    :CWeapon(Prototype)
{
}

HRESULT CMMoriblin_Weapon::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CMMoriblin_Weapon::Initialize_Copytype(void* pArg)
{
 
    if (FAILED(__super::Initialize_Copytype(pArg)))
        return E_FAIL;
    
    MORIBLIN_WEAPON* pDesc = static_cast<MORIBLIN_WEAPON*>(pArg);
    m_fRadius = pDesc->fRadius;
    m_vOffSet = pDesc->vOffSet;
    m_AnimKey = pDesc->AnimKey;


    if(FAILED(Ready_Components(pArg)))
        return E_FAIL;

 
    m_pMoriblin = dynamic_cast<CM_MoriblinSword*>(m_pOwner);





    return S_OK;
}

void CMMoriblin_Weapon::Update_Priority(_float fTimeDelta)
{
    __super::Update_Priority(fTimeDelta);
}

void CMMoriblin_Weapon::Update(_float fTimeDelta)
{
    __super::Update(fTimeDelta);
}

void CMMoriblin_Weapon::Update_Late(_float fTimeDelta)
{
    __super::Update_Late(fTimeDelta);
}

void CMMoriblin_Weapon::Update_Render(_float fTimeDelta)
{
    __super::Update_Render(fTimeDelta);
}

HRESULT CMMoriblin_Weapon::Render()
{
    __super::Render();

    return S_OK;
}

HRESULT CMMoriblin_Weapon::Ready_Components(void* pArg)
{
    CCollider_Base::COLLIDER_DESC pColliderDesc;
    pColliderDesc.m_eColGroup = ENUM_TO_UINT(COLLISION_GROUP::MONSTER_WEAPON);
    pColliderDesc.pOwner = this;

	CBounding_Sphere::BOUNDING_SPHERE_DESC      CollDesc{};
    CollDesc.fRadius = m_fRadius;
	CollDesc.vCenter = m_vOffSet;

	pColliderDesc.m_BoundingDesc = &CollDesc;



	CComponent* pCollider = dynamic_cast<CSphereColliderComponent*>(m_pGameInstance->Clone_Prototype(
		PROTOTYPE::COMPONENT,
		0,
		PROTO_COMPONENT_NAME(L"SphereCollider"),
		&pColliderDesc)
		);

	m_pCollider[0]=dynamic_cast<CCollider_Base*>(pCollider);
	pCollider->Set_Owner(this);




    return S_OK;
}

CMMoriblin_Weapon* CMMoriblin_Weapon::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
    CMMoriblin_Weapon* pInstance = new CMMoriblin_Weapon(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CMMoriblin_Weapon");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CMMoriblin_Weapon::Clone(void* pArg)
{
    CMMoriblin_Weapon* pInstance = new CMMoriblin_Weapon(*this);

    if (FAILED(pInstance->Initialize_Copytype(pArg)))
    {
        MSG_BOX("Failed to Cloned : CMMoriblin_Weapon");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CMMoriblin_Weapon::Free()
{
    __super::Free();

}

void CMMoriblin_Weapon::OnCollisionEnter(_uint iGroup, CCollider_Base* pOther)
{
    CGameObject* pOwner = pOther->Get_Owner();

    CheckNull(pOwner);
    switch (COLLISION_GROUP(iGroup))
    {
    case Client::COLLISION_GROUP::PLAYER_WEAPON:
        m_pMoriblin->Set_Guard(true,m_AnimKey);
        _float3 vDir;
        XMStoreFloat3(&vDir, m_pOwner->Get_Transform()->Get_State(STATE::LOOK));
        m_pTransformCom->AddImpulse(-2.f, vDir);
        break;
    case Client::COLLISION_GROUP::PLAYER:

        break;
   


    }
}

void CMMoriblin_Weapon::OnCollisionStay(_uint iGroup, CCollider_Base* pOther)
{
}

void CMMoriblin_Weapon::OnCollisionExit(_uint iGroup, CCollider_Base* pOther)
{
}
