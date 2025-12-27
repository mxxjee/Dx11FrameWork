#include "Cinteraction_Statue.h"
#include "CCollider_Base.h"
#include "CInteraction_TriggerBox.h"
#include "CCollider_Base.h"
#include "CBounding_AABB.h"
#include "CBoxColliderComponent.h"

#include "CNavigation.h"
#include "CPlayer.h"
#include "CBody.h"
#include "CInteraction_Manager.h"

USING(Client)
Cinteraction_Statue::Cinteraction_Statue(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    :CInteractionObject(pDevice,pContext)
{
}

Cinteraction_Statue::Cinteraction_Statue(const Cinteraction_Statue& rhs)
    : CInteractionObject(rhs)
{
}

HRESULT Cinteraction_Statue::Initialize_Copytype(void* pArg)
{
    CInteractionObject::Interaction_DESC Desc;
    Desc.eInteractionType = ENUM_TO_UINT(InteractionType::OBJECT);
    Desc.eInteract_Object_Type = InteractionType::OBJECT;
    Desc.m_iLevelID = ENUM_TO_UINT(LEVEL_ID::ROOM);


    Desc.eRenderGroup = ENUM_TO_UINT(RENDERGROUP::NONALPHA);
    Desc.ModelName = L"Workbench";

    Desc.SceneName = "RichardHouse_room";
    Desc.bAnimated = false;
    Desc.ObjTag = Desc.ModelName + to_wstring(0);

    Desc.m_bUseNavMesh = false;


    CBoxColliderComponent::COLLIDER_DESC ColDesc;
    ColDesc.m_eColGroup = ENUM_TO_UINT(COLLISION_GROUP::INTERACTION);
    CBounding_AABB::BOUNDING_AABB_DESC aabbDesc;
    aabbDesc.vCenter = _float3(0.f,0.f,0.f);
    aabbDesc.Extents = _float3(1.f, 1.f, 1.f);
    ColDesc.m_BoundingDesc = &aabbDesc;
    ColDesc.pOwner = this;
    ColDesc.m_iLevelID = m_iSceneID;
    Desc.pColliderComp = &ColDesc;


    if (FAILED(__super::Initialize_Copytype(&Desc)))
        return E_FAIL;


    m_pCollider->Set_Trigger(false);
    m_BehaviorType = Interact_Behavior_Type::PUSHABLE;
 
    CInteraction_Manager::GetInstance()->RegisterInteractable(this);

    
    return S_OK;
}

HRESULT Cinteraction_Statue::Ready_PartObjects(void* pArg)
{

    if (FAILED(__super::Ready_PartObjects(pArg)))
        return E_FAIL;

    Interaction_DESC* pDesc = static_cast<Interaction_DESC*>(pArg);


    CInteraction_TriggerBox::tagInteractionTriggerBoxDesc TriggerDesc;
    TriggerDesc.ObjTag = pDesc->ObjTag + L"Trigger_Box";
    TriggerDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
    TriggerDesc.pOwner = this;
    TriggerDesc.m_iLevelID = m_iSceneID;

    CBounding_AABB* pAABB = dynamic_cast<CBounding_AABB*>(m_pCollider->Get_Bounding());

    if (pAABB)
    {
        BoundingBox* pBoundingBox = pAABB->Get_OrignialDesc();
        TriggerDesc.vCenter = pBoundingBox->Center;

        XMStoreFloat3(&TriggerDesc.vExtents, XMLoadFloat3(&pBoundingBox->Extents) + XMVectorSet(0.25f, 0.25f, 0.25f, 0.f));
        if (FAILED(__super::Add_PartObject(0, PROTO_OBJ_NAME(L"Interaction_TriggerBox"), L"Part_TriggerBox", &TriggerDesc)))
            return E_FAIL;

        m_pTriggerBox = dynamic_cast<CInteraction_TriggerBox*>(Find_PartObject(L"Part_TriggerBox"));

    }
    return S_OK;

}

void Cinteraction_Statue::Update(_float fTimeDelta)
{
    __super::Update(fTimeDelta);
    CheckTrue(m_bReach);
    if (m_bOpen)
    {
        m_bReach=m_pTransformCom->Chase(m_vOpenPos, fTimeDelta, nullptr,1.f);
    }

}



bool Cinteraction_Statue::IsInteratable()
{
    CheckNullResult(m_pTriggerBox, false);
    return m_pTriggerBox->Is_Collision();
    return false;

}

void Cinteraction_Statue::Enter_InteractRange()
{
    CheckTrue(m_bInteraction);

    _vector ShowPos = MathUtils::WorldToScreen(m_pTransformCom->Get_State(STATE::POSITION),
        m_pGameInstance->Get_ViewMatrix(0), m_pGameInstance->Get_ProjMatrix(0), g_iWinSizeX, g_iWinSizeY);


    m_pGameInstance->BroadCastEvent(L"OnSeeUIShow", (void*)&ShowPos);
}

void Cinteraction_Statue::Exit_InteractRange()
{

    CheckTrue(m_bInteraction);
    m_pGameInstance->BroadCastEvent(L"OnSeeUIHide", (void*)nullptr);

}

void Cinteraction_Statue::Enter_Interaction()
{
   /* CheckTrue(m_pPlayer->Get_ActionControl()->m_bCarry);
    m_bCall_Exit_Interaction = false;*/

    wstring str = L"마녀가 했던 말이 떠오른다.\n가루는 숨은 것을 드러내지.";

    m_pGameInstance->BroadCastEvent(L"UpdateSeeDescText", &str);
    m_pGameInstance->BroadCastEvent(L"OnSeeDescUIShow", nullptr);


}

void Cinteraction_Statue::Stay_Interaction(_float fTimeDelta)
{
   
}

void Cinteraction_Statue::Exit_Interaction()
{
    m_pGameInstance->BroadCastEvent(L"OnSeeDescUIHide", nullptr);

}



Cinteraction_Statue* Cinteraction_Statue::Create(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pDeviceContext)
{
    Cinteraction_Statue* pInstance = new Cinteraction_Statue(_pDevice, _pDeviceContext);
    if (FAILED(pInstance->Initialize_Copytype(nullptr)))
    {
        MSG_BOX("Failed to Create :Cinteraction_Statue ");
        Safe_Release(pInstance);

    }



    return pInstance;
}

CGameObject* Cinteraction_Statue::Clone(void* pArg)
{
    Cinteraction_Statue* pInstance = new Cinteraction_Statue(*this);
    if (FAILED(pInstance->Initialize_Copytype(pArg)))
    {
        MSG_BOX("Failed to Clone :Cinteraction_Statue ");
        Safe_Release(pInstance);

    }



    return pInstance;
}

void Cinteraction_Statue::Free()
{
    __super::Free();
}

void Cinteraction_Statue::OnCollisionEnter(_uint iGroup, CCollider_Base* pOther)
{
    CGameObject* pOwner = pOther->Get_Owner();
    CheckNull(pOwner);

    switch (COLLISION_GROUP(iGroup))
    {
    case COLLISION_GROUP::PARTICLE:
    {
        CheckTrue(m_bOpen);
        m_bOpen = true;
        m_pTriggerBox->Set_Active(false);
        m_vOpenPos = m_pTransformCom->Get_State(STATE::POSITION) + XMVectorSet(2.5f, 0.f, 0.f, 0.f);


    }

    break;
    }
}

void Cinteraction_Statue::OnCollisionStay(_uint iGroup, CCollider_Base* pOther)
{
}

void Cinteraction_Statue::OnCollisionExit(_uint iGroup, CCollider_Base* pOther)
{
}
