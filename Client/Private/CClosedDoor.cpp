#include "CClosedDoor.h"
#include "CBoxColliderComponent.h"
#include "CCollider_Base.h"
#include "CBounding_AABB.h"
#include "Client_Defines.h"

#include "CStaticBody.h"
#include "CModel.h"
#include "CGameInstance.h"

USING(Client)

CClosedDoor::CClosedDoor(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    :CStaticModelObject(pDevice, pContext)
{
}

CClosedDoor::CClosedDoor(const CClosedDoor& rhs)
    : CStaticModelObject(rhs)
{
}

HRESULT CClosedDoor::Initialize_Prototype()
{

    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;


    return S_OK;
}

HRESULT CClosedDoor::Initialize_Copytype(void* pArg)
{
    if (FAILED(__super::Initialize_Copytype(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components(pArg)))
        return E_FAIL;

    if (FAILED(Ready_PartObjects(pArg)))
        return E_FAIL;

    m_vInitPos = m_pTransformCom->Get_State(STATE::POSITION);
    m_vClosePos = m_vInitPos + XMVectorSet(0.f, -0.6f, 0.f, 0.f);

    m_pGameInstance->RegisterListners("CloseDoor", [this](const GameEvent& event)
        {
            Close();

        });

    m_pGameInstance->RegisterListners("OpenDoor", [this](const GameEvent& event)
        {
            Open();

        });

    return S_OK;
}

void CClosedDoor::Update_Priority(_float fTimeDelta)
{
    __super::Update_Priority(fTimeDelta);
}

void CClosedDoor::Update(_float fTimeDelta)
{
    __super::Update(fTimeDelta);
    if (m_bClosedUpdate)
    {
        m_bOpenUpdate = false;
        m_pTransformCom->MoveLerp(m_vClosePos, 5.f, fTimeDelta);
    }

    else if (m_bOpenUpdate)
    {
        m_bClosedUpdate = false;
        m_pTransformCom->MoveLerp(m_vInitPos, 5.f, fTimeDelta);

    }

}

void CClosedDoor::Update_Late(_float fTimeDelta)
{

    __super::Update_Late(fTimeDelta);

    m_pCollider->Update_Collider(XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr()));

}

void CClosedDoor::Update_Render(_float fTimeDelta)
{
    __super::Update_Render(fTimeDelta);
}

HRESULT CClosedDoor::Render()
{
#ifdef _DEBUG
    m_pCollider->Render();

#endif // _DEBUG

    return S_OK;
}

HRESULT CClosedDoor::Ready_Components(void* pArg)
{
    //////////////Boxcollider√ﬂ∞°

    CCollider_Base::COLLIDER_DESC pColliderDesc;
    pColliderDesc.m_eColGroup = ENUM_TO_UINT(COLLISION_GROUP::WALL);
    pColliderDesc.m_iLevelID = ENUM_TO_UINT(LEVEL_ID::BOSS);

    CBounding_AABB::BOUNDING_AABB_DESC CollDesc;
    CollDesc.vCenter = { 0.f,0.9f,-0.3f };
    CollDesc.Extents = { 1.f,1.f,0.6f };
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

    m_pCollider->Set_Trigger(false);


    return S_OK;
}

HRESULT CClosedDoor::Ready_PartObjects(void* pArg)
{
    CheckNullResult(pArg, E_FAIL);
    MODELOBJECT_DESC* pModelDesc = static_cast<MODELOBJECT_DESC*>(pArg);
    if (pModelDesc)
    {
        CStaticBody::BODY_DESC pBodyDesc;

        pBodyDesc.eRenderGroup = ENUM_TO_UINT(RENDERGROUP::NONALPHA);
        pBodyDesc.modelName = L"ClosedDoor";
        pBodyDesc.pOwner = this;
        pBodyDesc.m_iLevelID = ENUM_TO_UINT(LEVEL_ID::STATIC);
        pBodyDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();


        if (FAILED(__super::Add_PartObject(0, PROTO_OBJ_NAME(L"StaticBody"), L"Part_Body", &pBodyDesc)))
            return E_FAIL;

        m_pBody = dynamic_cast<CBody*>(Find_PartObject(L"Part_Body"));

        if (m_pBody)
            m_pStaticBody = dynamic_cast<CStaticBody*>(m_pBody);
    }

    return S_OK;
}

void CClosedDoor::OnCollisionEnter(_uint iGroup, CCollider_Base* pOther)
{
}

void CClosedDoor::Close()
{
    m_bClosedUpdate = true;

  
  

}

void CClosedDoor::Open()
{

    m_bOpenUpdate = true;
}

CClosedDoor* CClosedDoor::Create(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pDeviceContex)
{
    CClosedDoor* pInstance = new CClosedDoor(_pDevice, _pDeviceContex);
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Create :CClosedDoor ");
        Safe_Release(pInstance);

    }

    return pInstance;
}

CGameObject* CClosedDoor::Clone(void* pArg)
{
    CClosedDoor* pInstance = new CClosedDoor(*this);
    if (FAILED(pInstance->Initialize_Copytype(pArg)))
    {
        MSG_BOX("Failed to Create :CClosedDoor ");
        Safe_Release(pInstance);

    }

    return pInstance;
}

void CClosedDoor::Free()
{
    __super::Free();
    Safe_Release(m_pCollider);

}
