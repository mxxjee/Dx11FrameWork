#include "CDungeonBlock.h"
#include "CBoxColliderComponent.h"
#include "CCollider_Base.h"
#include "CBounding_AABB.h"
#include "Client_Defines.h"

#include "CStaticBody.h"
#include "CModel.h"
#include "CGameInstance.h"


USING(Client)

CDungeonBlock::CDungeonBlock(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    :CStaticModelObject(pDevice, pContext)
{
}

CDungeonBlock::CDungeonBlock(const CDungeonBlock& rhs)
    : CStaticModelObject(rhs)
{
}

HRESULT CDungeonBlock::Initialize_Prototype(void* pArg)
{

      if (FAILED(__super::Initialize_Copytype(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components(pArg)))
        return E_FAIL;

    if (FAILED(Ready_PartObjects(pArg)))
        return E_FAIL;


    return S_OK;
}

void CDungeonBlock::Update_Priority(_float fTimeDelta)
{
    __super::Update_Priority(fTimeDelta);
}

void CDungeonBlock::Update(_float fTimeDelta)
{
    __super::Update(fTimeDelta);
    if (m_bDropUpdate)
    {
        m_pTransformCom->UpdateImpulse(fTimeDelta, nullptr,true);

    }
}

void CDungeonBlock::Update_Late(_float fTimeDelta)
{
    __super::Update_Late(fTimeDelta);

    m_pCollider->Update_Collider(XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr()));
}

void CDungeonBlock::Update_Render(_float fTimeDelta)
{
    __super::Update_Render(fTimeDelta);
#ifdef _DEBUG
    if (CGameInstance::m_bDrawDebug)
    {
        m_pGameInstance->Add_DebugComponent(m_pCollider);
    }
#endif // 

}

HRESULT CDungeonBlock::Render()
{
    return S_OK;
}

HRESULT CDungeonBlock::Ready_Components(void* pArg)
{
    //////////////Boxcollider√ﬂ∞°

    CCollider_Base::COLLIDER_DESC pColliderDesc;
    pColliderDesc.m_eColGroup = ENUM_TO_UINT(COLLISION_GROUP::OBJECT);
    pColliderDesc.m_iLevelID = ENUM_TO_UINT(LEVEL_ID::STATIC);

    CBounding_AABB::BOUNDING_AABB_DESC CollDesc;
    CollDesc.vCenter = { 0.f,0.f,0.f };
    CollDesc.Extents = { 0.5f,0.5f,0.5f };
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

HRESULT CDungeonBlock::Ready_PartObjects(void* pArg)
{
    CheckNullResult(pArg, E_FAIL);
    MODELOBJECT_DESC* pModelDesc = static_cast<MODELOBJECT_DESC*>(pArg);
    if (pModelDesc)
    {
        CStaticBody::BODY_DESC pBodyDesc;

        pBodyDesc.eRenderGroup = ENUM_TO_UINT(RENDERGROUP::NONALPHA);
        pBodyDesc.modelName = L"DungeonBlock";
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

void CDungeonBlock::OnCollisionEnter(_uint iGroup, CCollider_Base* pOther)
{

}

void CDungeonBlock::Drop()
{
    m_bDropUpdate = true;

    _float3 vDir;
    XMStoreFloat3(&vDir, m_pTransformCom->Get_State(STATE::UP));

    m_pTransformCom->AddImpulse(-0.5f, vDir);

}

CDungeonBlock* CDungeonBlock::Create(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pDeviceContex, void* pArg)
{
    CDungeonBlock* pInstance = new CDungeonBlock(_pDevice, _pDeviceContex);
    if (FAILED(pInstance->Initialize_Prototype(pArg)))
    {
        MSG_BOX("Failed to Create :CWeatherCock ");
        Safe_Release(pInstance);

    }

    return pInstance;
}

void CDungeonBlock::Free()
{
    __super::Free();
    Safe_Release(m_pCollider);
}
