#include "CInteraction_Mushroom.h"
#include "CCollider_Base.h"
#include "CInteraction_TriggerBox.h"

#include "CNavigation.h"
#include "CPlayer.h"
#include "CBody.h"
#include "CBounding_AABB.h"

#include "CInventory_Manager.h"



USING(Client)

CInteraction_Mushroom::CInteraction_Mushroom(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    :CInteractionObject(pDevice, pContext)
{
}

CInteraction_Mushroom::CInteraction_Mushroom(const CInteraction_Mushroom& rhs)
    : CInteractionObject(rhs)
{
}

HRESULT CInteraction_Mushroom::Initialize_Copytype(void* pArg)
{
    if (FAILED(__super::Initialize_Copytype(pArg)))
        return E_FAIL;

    m_BehaviorType = Interact_Behavior_Type::ITEMABLE;

    return S_OK;
}

HRESULT CInteraction_Mushroom::Ready_PartObjects(void* pArg)
{
    if (FAILED(__super::Ready_PartObjects(pArg)))
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

void CInteraction_Mushroom::Update(_float fTimeDelta)
{
    __super::Update(fTimeDelta);

}

void CInteraction_Mushroom::Update_Render(_float fTimeDelta)
{
    __super::Update_Render(fTimeDelta);
}

HRESULT CInteraction_Mushroom::Render()
{
    __super::Render();

    return S_OK;
}



bool CInteraction_Mushroom::IsInteratable()
{
    CheckNullResult(m_pTriggerBox, false);
    CheckTrueResult(!m_pTriggerBox->Is_Active(),false);


    return m_pTriggerBox->Is_Collision();
    return false;
}

void CInteraction_Mushroom::Enter_InteractRange()
{
    CheckTrue(m_bInteraction);
    CheckFalse(m_bActive);

    _vector ShowPos = MathUtils::WorldToScreen(m_pTransformCom->Get_State(STATE::POSITION),
        m_pGameInstance->Get_ViewMatrix(0), m_pGameInstance->Get_ProjMatrix(0), g_iWinSizeX, g_iWinSizeY);


    m_pGameInstance->BroadCastEvent(L"OnGetUIShow", (void*)&ShowPos);

}

void CInteraction_Mushroom::Exit_InteractRange()
{
    CheckTrue(m_bInteraction);
    CheckFalse(m_bActive);


    m_pGameInstance->BroadCastEvent(L"OnGetUIHide", (void*)nullptr);

}

void CInteraction_Mushroom::Enter_Interaction()
{
    CheckFalse(m_bActive);
    CheckTrue(m_pPlayer->Get_ActionControl()->m_bCarry);
    m_bCall_Exit_Interaction = false;

    //인벤에 템추가
    CInventory_Manager::GetInstance()->Request_Add_To_Inven(ItemType::MUSHROOM, 1);
    m_pGameInstance->BroadCastEvent(L"OnGetUIHide", (void*)nullptr);

    Set_Active(false);
    m_pTriggerBox->Set_Active(false);
   // m_pPlayer->Get_ActionControl()->m_bCarry = true;
  //  m_pGameInstance->SetActiveGroup(L"Interaction_PopUp_Carry", false);

}

void CInteraction_Mushroom::Stay_Interaction(_float fTimeDelta)
{
}

void CInteraction_Mushroom::Exit_Interaction()
{
}

CInteraction_Mushroom* CInteraction_Mushroom::Create(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pDeviceContext)
{
    CInteraction_Mushroom* pInstance = new CInteraction_Mushroom(_pDevice, _pDeviceContext);
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Create :CInteraction_Mushroom ");
        Safe_Release(pInstance);

    }



    return pInstance;
}

CGameObject* CInteraction_Mushroom::Clone(void* pArg)
{
    CInteraction_Mushroom* pInstance = new CInteraction_Mushroom(*this);
    if (FAILED(pInstance->Initialize_Copytype(pArg)))
    {
        MSG_BOX("Failed to Clone :CInteraction_Mushroom ");
        Safe_Release(pInstance);

    }



    return pInstance;
}

void CInteraction_Mushroom::Free()
{
    __super::Free();
}
