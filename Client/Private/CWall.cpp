#include "CWall.h"
#include "CCollider_Base.h" 
#include "CGameInstance.h"
#include "Client_Defines.h"
#include "CBounding_AABB.h"
#include "CBoxColliderComponent.h"


USING(Client)
CWall::CWall(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    :CGameObject(pDevice,pContext)
{
}

CWall::CWall(const CWall& Prototype)
    : CGameObject(Prototype)
{
}

HRESULT CWall::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CWall::Initialize_Copytype(void* pArg)
{

    WALL_DESC* pDesc = static_cast<WALL_DESC*>(pArg);

    if (FAILED(__super::Initialize_Copytype(pArg)))
        return E_FAIL;


    if (FAILED(Ready_Component(pArg)))
        return E_FAIL;

    return S_OK;
}

void CWall::Update_Priority(_float fTimeDelta)
{
    __super::Update_Priority(fTimeDelta);
}

void CWall::Update(_float fTimeDelta)
{


}

void CWall::Update_Late(_float fTimeDelta)
{
    CheckNull(m_pCollider);
    m_pCollider->Update_Collider(XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr()));

}

void CWall::Update_Render(_float fTimeDelta)
{
    m_pGameInstance->Add_RenderObject(ENUM_TO_UINT(RENDERGROUP::NONALPHA), this);

//#ifdef _DEBUG
//    if (FAILED(m_pGameInstance->Add_DebugComponent(m_pCollider)))
//        return;
//#endif
}

HRESULT CWall::Render()
{
#ifdef _DEBUG
    if (m_pCollider)
        m_pCollider->Render();
#endif // _DEBUG


    return S_OK;
}

void CWall::OnCollisionEnter(_uint iGroup, CCollider_Base* pOther)
{
}

void CWall::OnCollisionStay(_uint iGroup, CCollider_Base* pOther)
{
}

void CWall::OnCollisionExit(_uint iGroup, CCollider_Base* pOther)
{
}

void CWall::Set_Active(bool _b)
{
    __super::Set_Active(_b);
    m_pCollider->Set_Active(false);

    m_pGameInstance->UnRegister_Collider(m_pCollider, ENUM_TO_UINT(LEVEL_ID::BOSS));
}

HRESULT CWall::Ready_Component(void* pArg)
{
    WALL_DESC* pDesc = static_cast<WALL_DESC*>(pArg);

    CCollider_Base::COLLIDER_DESC pColliderDesc;
    pColliderDesc.m_eColGroup = ENUM_TO_UINT(COLLISION_GROUP::WALL);
    pColliderDesc.pOwner = this;
    pColliderDesc.m_iLevelID = pDesc->m_iLevelID;

    CBounding_AABB::BOUNDING_AABB_DESC      CollDesc{};
    CollDesc.Extents = pDesc->vExtents;
    pColliderDesc.m_iLevelID = pDesc->m_iLevelID;
    pColliderDesc.m_BoundingDesc = &CollDesc;


    CComponent* pCollider = dynamic_cast<CBoxColliderComponent*>(m_pGameInstance->Clone_Prototype(
        PROTOTYPE::COMPONENT,
        0,
        PROTO_COMPONENT_NAME(L"BoxCollider"),
        &pColliderDesc)
        );

    if (FAILED(Add_Component(
        COMPONENT_TYPE::BOX_COLLIDER,
        pCollider,
        reinterpret_cast<CComponent**>(&m_pCollider)
    )))
        return E_FAIL;






    CheckNullResult(m_pCollider,E_FAIL);
    m_pCollider->Set_Trigger(false);


    return S_OK;
}

CWall* CWall::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
    CWall* pInstance = new CWall(pDevice, pContext);
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Create :CWall ");
        Safe_Release(pInstance);

    }



    return pInstance;
}

CGameObject* CWall::Clone(void* pArg)
{
    CWall* pInstance = new CWall(*this);
    if (FAILED(pInstance->Initialize_Copytype(pArg)))
    {
        MSG_BOX("Failed to Clone :CWall ");
        Safe_Release(pInstance);

    }



    return pInstance;
}

void CWall::Free()
{

    Safe_Release(m_pCollider);
    __super::Free();


}
