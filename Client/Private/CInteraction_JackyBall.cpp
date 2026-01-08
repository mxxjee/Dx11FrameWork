#include "CInteraction_JackyBall.h"
#include "CCollider_Base.h"
#include "CInteraction_TriggerBox.h"
#include "CBounding_AABB.h"
#include "CBounding.h"
#include "Client_Defines.h"
#include "CStaticBody.h"

#include "CMonsterDeadState.h"

#include "CPlayer.h"
#include "CM_Jacky.h"

#include "CNavigation.h"
#include "CModel.h"
#include "CLayer.h"
#include "CImGui_Manager.h"
#include "CInteraction_Manager.h"





USING(Client)
CInteraction_JackyBall::CInteraction_JackyBall(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    :CInteractionObject(pDevice, pContext)
{
}

CInteraction_JackyBall::CInteraction_JackyBall(const CInteraction_JackyBall& rhs)
    : CInteractionObject(rhs)
{
}

HRESULT CInteraction_JackyBall::Initialize_Copytype(void* pArg)
{
    if (FAILED(__super::Initialize_Copytype(pArg)))
        return E_FAIL;

    m_pCollider->Set_Trigger(false);
    m_BehaviorType = Interact_Behavior_Type::CARRYABLE;
    m_fDissolveAlpha = 0.f;

    return S_OK;
}

HRESULT CInteraction_JackyBall::Ready_PartObjects(void* pArg)
{
    if (FAILED(__super::Ready_PartObjects(pArg)))
        return E_FAIL;


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

void CInteraction_JackyBall::Update(_float fTimeDelta)
{
    Dead_Behavior(fTimeDelta);

    CheckTrue(m_bDead);
    __super::Update(fTimeDelta);

    if (!CanInteractive)
    {
        m_fTime += fTimeDelta;
        if (m_fTime >= m_fInteractionCoolTime)
        {
            m_fTime = 0.f;
            CanInteractive = true;
        }
           
    }
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

        m_pCollider->Set_Active(false);
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

void CInteraction_JackyBall::Update_Late(_float fTimeDelta)
{
    CheckTrue(m_bDead);
    __super::Update_Late(fTimeDelta);
    m_pTriggerBox->Update_Late(fTimeDelta);
}

bool CInteraction_JackyBall::IsInteratable()
{
    CheckFalseResult(m_bActive, false);
    CheckFalseResult(CanInteractive,false);


    CheckTrueResult(m_bInteraction,false);
    CheckNullResult(m_pTriggerBox, false);
    CheckTrueResult(m_pOwner != nullptr,false);

    bool bResult=m_pTriggerBox->Is_Collision();
    
    if (bResult)
    {
        CGameObject* pOther = m_pTriggerBox->Get_Other();

        //충돌한 사람이있으면
        if (pOther)
        {
             //플레이어와 충돌했으면..
            if (pOther == m_pPlayer)
            {
                return true;
            }
            else

            {
                //아닌데, 몬스터랑충돌했으면
                CM_Jacky* pMonster = dynamic_cast<CM_Jacky*>(pOther);
                if (pMonster)
                {
                    //몬스터잡기함수수행
                    pMonster->Grab(this);
                    return false;
                }

                else
                    return false;
            }

        }

        else
            return false;
    }
  
    return false;
    
}

void CInteraction_JackyBall::Enter_InteractRange()
{
    
    CheckTrue(m_bInteraction);
    CheckFalse( m_pTriggerBox->Get_Other()==m_pPlayer);


 

    _vector ShowPos = MathUtils::WorldToScreen(m_pTransformCom->Get_State(STATE::POSITION),
        m_pGameInstance->Get_ViewMatrix(0), m_pGameInstance->Get_ProjMatrix(0), g_iWinSizeX, g_iWinSizeY);


    m_pGameInstance->BroadCastEvent(L"OnCarryUIShow", (void*)&ShowPos);

}

void CInteraction_JackyBall::Exit_InteractRange()
{
    CheckTrue(m_bInteraction);
    m_pGameInstance->BroadCastEvent(L"OnCarryUIHide", (void*)nullptr);

}

void CInteraction_JackyBall::Enter_Interaction()
{
   
    CheckTrue(m_pPlayer->Get_ActionControl()->m_bCarry);

    //이미 주인이있으면 들기X
    CheckTrue(m_pOwner !=nullptr);
    
    __super::Enter_Interaction();

    m_pTriggerBox->Set_Active(false);
    m_pPlayer->Get_ActionControl()->m_bCarry = true;
    m_pGameInstance->SetActiveGroup(L"Interaction_PopUp_Carry", false);


}

void CInteraction_JackyBall::Stay_Interaction(_float fTimeDelta)
{
}

void CInteraction_JackyBall::Exit_Interaction()
{
    //CheckTrue(m_bCall_Exit_Interaction);
    m_bCall_Exit_Interaction = true;

    if (m_pOwner == m_pPlayer)
    {
        m_pPlayer->Get_ActionControl()->m_bCarry = false;
        m_pTriggerBox->Set_Active(true);
        m_pPlayer->Set_CarryAndThrowState(this);

    }

}

void CInteraction_JackyBall::OnCollisionEnter(_uint iGroup, CCollider_Base* pOther)
{
    CGameObject* pOtherOwner = pOther->Get_Owner();
    CheckNull(pOtherOwner);
    CheckNull(m_pOwner);

    //자신의 오너가 아닌사람과 충돌했다면, 땅에떨어지도록한다.
    if (pOtherOwner != m_pOwner)
    {
        _vector vOtherPos = pOtherOwner->Get_Transform()->Get_State(STATE::POSITION);
        _vector vPos = m_pTransformCom->Get_State(STATE::POSITION);

        //밀려나는방향
        _vector vDir = vPos - vOtherPos;
        _float3 fVdir;
        XMStoreFloat3(&fVdir, vDir);

        m_pTransformCom->AddImpulse(0.4f, fVdir, m_pNavigationCom);
        Set_InteractionMode(false);
    }

}

void CInteraction_JackyBall::Set_EnableCollision(bool b)
{
    __super::Set_EnableCollision(b);

    if (m_pTriggerBox)
        m_pTriggerBox->Set_Active(false);

}

void CInteraction_JackyBall::Set_Dead()
{
    m_bDead = true;

}

void CInteraction_JackyBall::Dead_Behavior(_float fTimeDelta)
{
    CheckFalse(m_bDead);
    m_fDissolveSpeed = 1.f;

    m_pBody->Get_Model()->Set_UpdateAnimation(false);
    //alpha값조절
    m_fDissolveAlpha += fTimeDelta * m_fDissolveSpeed;
    m_pBody->Set_DissolveClipAlph(m_fDissolveAlpha);

    /*완전한 삭제처리*/
    if (m_fDissolveAlpha >= 1.f)
    {
        Set_Active(false);
        Set_EnableCollision(false);
        CInteraction_Manager::GetInstance()->UnRegisterInteractable(this);

        CLayer* pLayer = m_pGameInstance->Find_Layer(m_iSceneID, L"Interaction_Layer");
        pLayer->RequestDestroy(this);

        
#if _DEBUG
        //selectobjec관련들 다 초기화
        CImGui_Manager::GetInstance()->Reset_Window("ObjectDebugWindow");
        CImGui_Manager::GetInstance()->Reset_Window("StateDebugWindow");


#endif
      

    }
}

void CInteraction_JackyBall::Throw()
{
    CheckNull(m_pSocketMatrix);

    _float3 ThrowDir;
    CTransform* pTransform = m_pPlayer->Get_Transform();
    CheckNull(pTransform);


    CheckNull(m_pOwner);
    _vector vDir = (m_pOwner->Get_Transform()->Get_State(STATE::LOOK));

    XMStoreFloat3(&ThrowDir, vDir);

    m_pSocketMatrix = nullptr;

    m_pTransformCom->AddImpulse(0.5f, ThrowDir, nullptr, true);
    m_pNavigationCom->Set_CurrentIdx(pTransform->Get_State(STATE::POSITION));
    m_bPhysics = true;
    XMStoreFloat3(&Pos, m_pTransformCom->Get_State(STATE::POSITION));
}

wstring CInteraction_JackyBall::Print_Owner()
{
    if (m_pOwner == nullptr)
        return L"Owner:NONE";

    else if (m_pOwner == m_pPlayer)
        return L"Owner:PLAYER";

    else
        return L"Owner:JACKY";

}

CInteraction_JackyBall* CInteraction_JackyBall::Create(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pDeviceContext)
{
    CInteraction_JackyBall* pInstance = new CInteraction_JackyBall(_pDevice, _pDeviceContext);
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Create :CInteraction_JackyBall ");
        Safe_Release(pInstance);

    }



    return pInstance;
}

CGameObject* CInteraction_JackyBall::Clone(void* pArg)
{
    CInteraction_JackyBall* pInstance = new CInteraction_JackyBall(*this);
    if (FAILED(pInstance->Initialize_Copytype(pArg)))
    {
        MSG_BOX("Failed to Clone :CInteraction_JackyBall ");
        Safe_Release(pInstance);

    }



    return pInstance;
}

void CInteraction_JackyBall::Free()
{
    __super::Free();
}

void CInteraction_JackyBall::PushOut(_float3 vOutPush)
{
    __super::PushOut(vOutPush);

}

void CInteraction_JackyBall::Set_InteractionMode(bool b)
{
    __super::Set_InteractionMode(b);

    if (!b)
    {
        CheckNull(m_pOwner);
        CM_Jacky* pJackey = dynamic_cast<CM_Jacky*>(m_pOwner);
        if (pJackey)
        {
            pJackey->Get_JackyActionInput()->bLift = false;
           

        }


        Exit_Interaction();
        m_pOwner = nullptr;
        CanInteractive = false;
        
    }
}
