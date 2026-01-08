#include "CInteraction_Rock.h"
#include "CCollider_Base.h"
#include "CInteraction_TriggerBox.h"
#include "CCollider_Base.h"
#include "CBounding_AABB.h"
#include "CBoxColliderComponent.h"

#include "CNavigation.h"
#include "CPlayer.h"
#include "CBody.h"
#include "CInventory_Manager.h"

USING(Client)
CInteraction_Rock::CInteraction_Rock(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    :CInteractionObject(pDevice,pContext)
{
}

CInteraction_Rock::CInteraction_Rock(const CInteraction_Rock& rhs)
    : CInteractionObject(rhs)
{
}

HRESULT CInteraction_Rock::Initialize_Copytype(void* pArg)
{
    if(FAILED(__super::Initialize_Copytype(pArg)))
        return E_FAIL;

    m_pCollider->Set_Trigger(false);
    m_BehaviorType = Interact_Behavior_Type::CARRYABLE;

    m_pInventory_Manager = CInventory_Manager::GetInstance();

    return S_OK;
}

HRESULT CInteraction_Rock::Ready_PartObjects(void* pArg)
{
    if(FAILED(__super::Ready_PartObjects(pArg)))
        return E_FAIL;


    /// <summary>
  /// //////이벤트감지용 트리거
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

void CInteraction_Rock::Update(_float fTimeDelta)
{
    __super::Update(fTimeDelta);

    if (m_bInteraction && m_pSocketMatrix)
    {
        _matrix SocketMatrix = XMLoadFloat4x4(m_pSocketMatrix);
        SocketMatrix.r[0] = XMVectorSet(1.f, 0.f, 0.f, 0.f);
        SocketMatrix.r[1] = XMVectorSet(0.f, 1.f, 0.f, 0.f);
        SocketMatrix.r[2] = XMVectorSet(0.f, 0.f, 1.f, 0.f);


        //따라가려는 소켓매트릭스 x 원래 parent매트릿그
        _matrix ParentMatrix = SocketMatrix * XMLoadFloat4x4(m_pParentMatrix);
        _float4x4 WorldMat;

        XMStoreFloat4x4(&WorldMat, ParentMatrix);
        m_pTransformCom->Set_WorldMatrix(WorldMat);

    }

    else
    {
       
        _matrix ParentMatrix = XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr());
        _float4x4 WorldMat;

        XMStoreFloat4x4(&WorldMat, ParentMatrix);
        m_pTransformCom->Set_WorldMatrix(WorldMat);

        m_pBody->SetUp_CombinedWorldMatrix(ParentMatrix);
    }
        
    
   
  
}

bool CInteraction_Rock::IsInteratable()
{
    CheckFalseResult(m_bActive, false);
    CheckNullResult(m_pTriggerBox, false);
    bool bResult= m_pTriggerBox->Is_Collision();

    if (bResult)
    {
        if (m_pTriggerBox->Get_Other() == m_pPlayer)
            return true;

        else
            bResult = false;
    }
    return bResult;
}

void CInteraction_Rock::Enter_InteractRange()
{
    CheckTrue(m_bInteraction);

	_vector ShowPos = MathUtils::WorldToScreen(m_pTransformCom->Get_State(STATE::POSITION),
		m_pGameInstance->Get_ViewMatrix(0), m_pGameInstance->Get_ProjMatrix(0), g_iWinSizeX, g_iWinSizeY);


	m_pGameInstance->BroadCastEvent(L"OnCarryUIShow", (void*)&ShowPos);

}

void CInteraction_Rock::Exit_InteractRange()
{
    CheckTrue(m_bInteraction);
   m_pGameInstance->BroadCastEvent(L"OnCarryUIHide", (void*)nullptr);
}

void CInteraction_Rock::Enter_Interaction()
{
    InvenSlot* pSlot = m_pInventory_Manager->Get_XSlot();
    if (pSlot == nullptr || pSlot->ItemType != ItemType::POWER_BRACELET)
    {
        wstring wstr = L"아직 들기엔 무거운거같은데..";
        m_pGameInstance->BroadCastEvent(L"UpdateEmptySlotText", &wstr);
        m_pGameInstance->BroadCastEvent(L"OnEmptySlotUIShow", nullptr);
        
        m_bCanCarry = false;
        return;
    }


    CheckTrue(m_pPlayer->Get_ActionControl()->m_bCarry);
    m_bCall_Exit_Interaction = false;


    m_pPlayer->Get_ActionControl()->m_bCarry = true;
    m_pGameInstance->SetActiveGroup(L"Interaction_PopUp_Carry", false);
    m_bCanCarry = true;

    //anim notify에 의해 들게된다..

}

void CInteraction_Rock::Stay_Interaction(_float fTimeDelta)
{
}

void CInteraction_Rock::Exit_Interaction()
{
    //인터렉션 취소(A한번 더누름)
    if (!m_bCanCarry)
    {
        m_pGameInstance->BroadCastEvent(L"OnEmptySlotUIHide", nullptr);
        return;
    }
    CheckTrue(m_bCall_Exit_Interaction);
    m_bCall_Exit_Interaction = true;

   m_pPlayer->Get_ActionControl()->m_bCarry = false;
    m_pPlayer->Set_CarryAndThrowState(this);
    m_pGameInstance->SetActiveGroup(L"Interaction_PopUp_Carry", false);

}


CInteraction_Rock* CInteraction_Rock::Create(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pDeviceContext)
{
    CInteraction_Rock* pInstance = new CInteraction_Rock(_pDevice, _pDeviceContext);
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Create :CInteraction_Rock ");
        Safe_Release(pInstance);

    }



    return pInstance;
}

CGameObject* CInteraction_Rock::Clone(void* pArg)
{
    CInteraction_Rock* pInstance = new CInteraction_Rock(*this);
    if (FAILED(pInstance->Initialize_Copytype(pArg)))
    {
        MSG_BOX("Failed to Clone :CInteraction_Rock ");
        Safe_Release(pInstance);

    }



    return pInstance;
}

void CInteraction_Rock::Free()
{
    __super::Free();
}
