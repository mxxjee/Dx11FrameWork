#include "CPlayer_Shield.h"
#include "CBounding_AABB.h"
#include "CBoxColliderComponent.h"
#include "CGameInstance.h"
#include "CInput_Manager.h"
#include "CBounding_OBB.h"
#include "COBBColliderComponent.h"
#include "Client_Defines.h"


USING(Client)
CPlayer_Shield::CPlayer_Shield(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    :CWeapon(pDevice,pContext)
{
}

CPlayer_Shield::CPlayer_Shield(const CPlayer_Shield& Prototype)
    : CWeapon(Prototype)
{
}

HRESULT CPlayer_Shield::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;


    return S_OK;
}

HRESULT CPlayer_Shield::Initialize_Copytype(void* pArg)
{
    if (FAILED(__super::Initialize_Copytype(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components(pArg)))
        return E_FAIL;

    m_pTransformCom->Set_State(STATE::POSITION, XMVectorSet(0.f, 0.f, -0.5f, 1.f));
     
    Set_Active(false);

    return S_OK;
}

void CPlayer_Shield::Update_Priority(_float fTimeDelta)
{
    __super::Update_Priority(fTimeDelta);
}

void CPlayer_Shield::Update(_float fTimeDelta)
{
    __super::Update(fTimeDelta);


}

void CPlayer_Shield::Update_Late(_float fTimeDelta)
{
    __super::Update_Late(fTimeDelta);
 
}

void CPlayer_Shield::Update_Render(_float fTimeDelta)
{
    __super::Update_Render(fTimeDelta);
}

HRESULT CPlayer_Shield::Render()
{
    __super::Render();

    return S_OK;
}

HRESULT CPlayer_Shield::Ready_Components(void* pArg)
{

    CCollider_Base::COLLIDER_DESC pColliderDesc;
    pColliderDesc.m_eColGroup = ENUM_TO_UINT(COLLISION_GROUP::PLAYER_WEAPON);

    
    CBounding_OBB::BOUNDING_OBB_DESC CollDesc;
    CollDesc.vCenter = { 0.f,0.5f,0.f };
    CollDesc.Extents = { 0.4f,0.4f,0.1f };

    pColliderDesc.m_BoundingDesc = &CollDesc;

    CComponent* pCollider = dynamic_cast<COBBColliderComponent*>(m_pGameInstance->Clone_Prototype(
        PROTOTYPE::COMPONENT,
        0,
        PROTO_COMPONENT_NAME(L"OBBCollider"),
        &pColliderDesc)
        );

    if (FAILED(Add_Component(
        COMPONENT_TYPE::OBB_COLLIDER,
        pCollider,
        reinterpret_cast<CComponent**>(&m_pCollider[0])
    )))
        return E_FAIL;



    return S_OK;
}

CPlayer_Shield* CPlayer_Shield::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
    CPlayer_Shield* pInstance = new CPlayer_Shield(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CPlayer_Shield");
        Safe_Release(pInstance);
    }
    return pInstance;
}  

CGameObject* CPlayer_Shield::Clone(void* pArg)
{
    CPlayer_Shield* pInstance = new CPlayer_Shield(*this);

    if (FAILED(pInstance->Initialize_Copytype(pArg)))
    {
        MSG_BOX("Failed to Cloned : CPlayer_Shield");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CPlayer_Shield::Free()
{
    __super::Free();
}
