#include "CM_Jacky.h"
#include "CNavigation.h"
#include "CMonster.h"

#include "CCollider_Base.h"
#include "CBounding_AABB.h"
#include "CBoxColliderComponent.h"

#include "CMonster_Body.h"

///////////STates////
#include "CMJackyIdleState.h"
#include "CMJackyRunState.h"

#include "CGameManager.h"
#include "CAnimation.h"
#include "CModel.h"

#include "CPlayer.h"
#include "CInteraction_JackyBall.h"
#include "CMJackyLiftState.h"
#include "CMJackyThrowState.h"
#include "CInteraction_TriggerBox.h"
#include "CMJackyEscapeState.h"

USING(Client)

CM_Jacky::CM_Jacky(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    :CMonster(pDevice,pContext)
{
}

CM_Jacky::CM_Jacky(const CM_Jacky& Prototype)
    : CMonster(Prototype)
{
}

HRESULT CM_Jacky::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CM_Jacky::Initialize_Copytype(void* pArg)
{
    if (FAILED(__super::Initialize_Copytype(pArg)))
        return E_FAIL;

    Register_Anim();

    
    if (FAILED(Ready_States()))
        return E_FAIL;

    if (m_pNavigationCom)
    {
        m_pNavigationCom->Set_CurrentIdx(m_pTransformCom->Get_State(STATE::POSITION));

    }


    Change_State(ENUM_TO_UINT(CMonster::MONSTER_BASE_STATE::IDLE));
    m_pPlayer = CGameManager::GetInstance()->Get_MainPlayer();


    return S_OK;
}

void CM_Jacky::Update_Priority(_float fTimeDelta)
{
    __super::Update_Priority(fTimeDelta);

}

void CM_Jacky::Update(_float fTimeDelta)
{
    Setting_Target();
    if (m_bRotate)
    {
        if (m_pTarget)
            m_pTransformCom->LookAtSmooth_Quaternion(m_vTargetPos, XM_PI * 0.8f, fTimeDelta);
    }

    AIState_Change(fTimeDelta);

   if (m_pCurState)
        m_pCurState->Update(this, fTimeDelta);


    __super::Update(fTimeDelta);
    
}

void CM_Jacky::Update_Late(_float fTimeDelta)
{
    __super::Update_Late(fTimeDelta);
}

void CM_Jacky::Update_Render(_float fTimeDelta)
{
    __super::Update_Render(fTimeDelta);
}

HRESULT CM_Jacky::Render()
{
    __super::Render();



    return S_OK;
}

void CM_Jacky::Register_Anim()
{
    if (m_pMonsterBody)
    {
        m_pMonsterBody->Register_AnimKey(ENUM_TO_UINT(CMonster::MONSTER_BASE_STATE::IDLE), L"wait");

        m_pMonsterBody->Register_AnimKey(ENUM_TO_UINT(CM_Jacky::JackyState::LIFTING), L"lifting_st");

        m_pMonsterBody->Register_AnimKey(ENUM_TO_UINT(CMonster::MONSTER_BASE_STATE::DAMAGE), L"damage");



        m_pMonsterBody->Register_AnimKey(ENUM_TO_UINT(CMonster::MONSTER_BASE_STATE::DIE), L"dead");

        m_pMonsterBody->Register_AnimKey(ENUM_TO_UINT(CMonster::MONSTER_BASE_STATE::RUN), L"move");
        m_pMonsterBody->Register_AnimKey(ENUM_TO_UINT(CM_Jacky::JackyState::LIFTING_MOVE), L"lifting_move");
        m_pMonsterBody->Register_AnimKey(ENUM_TO_UINT(CM_Jacky::JackyState::THROW), L"throw");

        m_pMonsterBody->Register_AnimKey(ENUM_TO_UINT(CM_Jacky::JackyState::ESCAPE), L"escape");



    }



    //animnotify추가
    GameEvent Event;
    EventPayload payload;
    Event.Payload = payload;

    Event.Payload.Ptrs["Player"] = CGameManager::GetInstance()->Get_MainPlayer();
    CAnimation* pAnim = m_pBody->Get_Model()->Find_Animation(L"move");
    CAnimation* pCarryAnim = m_pBody->Get_Model()->Find_Animation(L"lifting_move");

    if (pAnim)
    {
        Event.Name = "MoveBegin";
        pAnim->AddNotify(4, Event);
        pCarryAnim->AddNotify(4, Event);

        Event.Name = "RotateBegin";
        pAnim->AddNotify(4, Event);
        pCarryAnim->AddNotify(4, Event);

    }

    m_pGameInstance->RegisterListners("MoveBegin", [this](const GameEvent& event)
        {
            m_bCanMove = true;

        });

    m_pGameInstance->RegisterListners("RotateBegin", [this](const GameEvent& event)
        {
            m_bRotate = true;

        });

    if (pAnim)
    {
        Event.Name = "MoveEnd";
        pAnim->AddNotify(20, Event);
        pCarryAnim->AddNotify(27, Event);
    }

    CAnimation* pThrowAnim = m_pBody->Get_Model()->Find_Animation(L"throw");
    if (pThrowAnim)
    {
        Event.Name = "Throw";
        pThrowAnim->AddNotify(7, Event);
    }

    m_pGameInstance->RegisterListners("MoveEnd", [this](const GameEvent& event)
        {
            m_bCanMove = false;

        });

    m_pGameInstance->RegisterListners("Throw", [this](const GameEvent& event)
        {
            Throw();
        });


    ////speed
    m_pMonsterBody->Set_Animation_Speed(L"move", 65.f);
    m_pMonsterBody->Set_Animation_Speed(L"wait", 50.f);
    m_pMonsterBody->Set_Animation_Speed(L"lifting_move", 65.f);
    m_pMonsterBody->Set_Animation_Speed(L"throw", 50.f);
    m_pMonsterBody->Set_Animation_Speed(L"escape", 50.f);

}



HRESULT CM_Jacky::Ready_Components(void* pArg)
{
    if(FAILED(__super::Ready_Components(pArg)))
        return E_FAIL;


    //////////////Boxcollider추가
    CCollider_Base::COLLIDER_DESC pColliderDesc;
    pColliderDesc.m_eColGroup = ENUM_TO_UINT(COLLISION_GROUP::MONSTER);
    pColliderDesc.m_iLevelID = ENUM_TO_UINT(LEVEL_ID::BOSS);

    CBounding_AABB::BOUNDING_AABB_DESC CollDesc;
    CollDesc.vCenter = { 0.f,0.5f,0.f };
    CollDesc.Extents = { 0.8f,0.8f,0.8f };
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

HRESULT CM_Jacky::Ready_States()
{

   m_States.emplace(CMonster::MONSTER_BASE_STATE::IDLE, CMJackyIdleState::Create());
   m_States.emplace(CMonster::MONSTER_BASE_STATE::WALK, CMJackyRunState::Create());
   m_States.emplace(CM_Jacky::JackyState::LIFTING, CMJackyLiftState::Create());
   m_States.emplace(CM_Jacky::JackyState::THROW, CMJackyThrowState::Create());
   
   m_States.emplace(CM_Jacky::JackyState::ESCAPE, CMJackyEscapeState::Create());





    return S_OK;
}

void CM_Jacky::Enter_State(int newState)
{
    switch (newState)
    {
    case ENUM_TO_UINT(CMonster::MONSTER_BASE_STATE::IDLE):
    {
        m_pGameInstance->Invoke(2.f, false, false, false, [this]()
            {
                //대기 후 1초뒤 양옆으로이동
                m_ActionControl.m_bMove = true;
            }, this);
    }
    break;

    case ENUM_TO_UINT(CMonster::MONSTER_BASE_STATE::WALK):
    {


    }
    break;

    case ENUM_TO_UINT(CM_Jacky::JackyState::LIFTING):
    {
        /*처음들어왔을때 시간갱신*/
        m_pInteractionObject = m_pChaseTarget;
        m_fThrowTargetTime = rand() % 4 + 3.f;

    }
       
        break;

    case ENUM_TO_UINT(CM_Jacky::JackyState::ESCAPE):
    {
        /*처음들어왔을때 방향갱신*/

    }

    break;
    default:
        break;
    }

}

void CM_Jacky::Exit_State(int newState)
{
}

void CM_Jacky::AIState_Change(_float fTimeDelta)
{
    Update_DeadState(fTimeDelta);

    CheckTrue(m_ActionControl.m_bDead);

    switch (m_iState)
    {
    case Client::CMonster::MONSTER_BASE_STATE::IDLE:
        //sphere감지 되면 idle->movest 
        Idle_Behavior(fTimeDelta);
        break;

    case Client::CMonster::MONSTER_BASE_STATE::WALK:
        Walk_Behavior(fTimeDelta);
        break;
    case Client::CMonster::MONSTER_BASE_STATE::RUN:
        break;
    case Client::CMonster::MONSTER_BASE_STATE::JUMP:
        break;
    case Client::CMonster::MONSTER_BASE_STATE::ATTACK:

        break;
    case Client::CMonster::MONSTER_BASE_STATE::DAMAGE:
        break;
    case Client::CMonster::MONSTER_BASE_STATE::DIE:
        break;
    case Client::CMonster::MONSTER_BASE_STATE::END:
        break;

    case Client::CM_Jacky::JackyState::ESCAPE:
        Escape_Behavior(fTimeDelta);
        break;
    default:
        break;
    }


}

void CM_Jacky::Update_Movement(_float fTimeDelta)
{
}

CM_Jacky* CM_Jacky::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
    CM_Jacky* pInstance = new CM_Jacky(pDevice, pContext);
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Create :CM_Jacky ");
        Safe_Release(pInstance);

    }
    return pInstance;

}

CGameObject* CM_Jacky::Clone(void* pArg)
{

    CM_Jacky* pInstance = new CM_Jacky(*this);
    if (FAILED(pInstance->Initialize_Copytype(pArg)))
    {
        MSG_BOX("Failed to Cloned :CM_Jacky ");
        Safe_Release(pInstance);

    }
    return pInstance;
}

void CM_Jacky::Free()
{
    __super::Free();
}

void CM_Jacky::Idle_Behavior(_float fTimeDelta)
{
    //임시대기시간 이후에 바로 walk상태
    
    //인트로 이후 N초뒤에 쳐다보게할건지?
    //일단 바로쳐다보게해.

  /*  if (m_pTarget)
        m_pTransformCom->LookAtSmooth_Quaternion(m_pTarget->Get_Transform()->Get_State(STATE::POSITION), XM_PI * 1.5f,fTimeDelta);*/
    
    if (m_pTarget)
        m_vTargetPos = m_pTarget->Get_Transform()->Get_State(STATE::POSITION);


}

void CM_Jacky::Walk_Behavior(_float fTimeDelta)
{
    /*던지기상태 설정*/
    m_fThrowTime += fTimeDelta;
    if (m_fThrowTime >= m_fThrowTargetTime && Is_Carrying())
    {
        //상태변경!! 실제던지기는 애님노티파이
        m_JackyActionInput.bThrow = true;
        m_fThrowTime = 0.f;
        return;
    }
    CheckNull(m_pTarget);

    if (m_bCanMove)
    {
        _vector vChasePos = m_pTarget->Get_Transform()->Get_State(STATE::POSITION);

        //m_pTransformCom->Chase(vChasePos, fTimeDelta, m_pNavigationCom, 2.f);
        
        XMVECTOR vMyPos = m_pTransformCom->Get_State(STATE::POSITION);

        //거리 계산
        XMVECTOR vDistVec = vChasePos - vMyPos;
        _float fDistance = XMVectorGetX(XMVector3Length(vDistVec));

        if (fDistance >1.5f)
        {
            m_pTransformCom->Move(DIRECTION::FORWARD, fTimeDelta, Space::Local, m_pNavigationCom);

        }
    }



}

void CM_Jacky::Escape_Behavior(_float fTimeDelta)
{

}

void CM_Jacky::Setting_Target()
{
    if (m_iState != ENUM_TO_UINT(CM_Jacky::JackyState::ESCAPE))
    {
        if (!Is_Carrying())
            m_pTarget = m_pChaseTarget;

        else
            m_pTarget = m_pPlayer;

    }

    //도망치고있다면.. 
    else
    {
        
    }
}

void CM_Jacky::Grab(CInteractionObject* pObj)
{
    CheckTrue(m_JackyActionInput.bLift);

    m_JackyActionInput.bLift = true;
    pObj->Set_InteractionMode(true);
    pObj->Set_ParentMatrix(m_pTransformCom->Get_WorldMatrixPtr());
    pObj->Set_SocketMatrix(m_pBody->Get_SocketMatrix("attach_L"));
    m_pChaseTarget->Set_Owner(this);
    m_pChaseTarget->Set_Call_Exit_Interaction(false);

    //바로던져서..잡으면 초기화
    m_fThrowTime = 0.f;


    m_bRotate = false;
}

void CM_Jacky::Throw()
{
    m_pChaseTarget->Throw();
    m_pInteractionObject = nullptr;
    m_JackyActionInput.bThrow = false;
    m_JackyActionInput.bLift = false;

}



string CM_Jacky::Convert_String_To_Enum(_uint eState)
{
    string StateDebugStr = "";

    if (eState == 0)
        return "NONE";


    else
    {
        if (eState == ENUM_TO_UINT(CMonster::MONSTER_BASE_STATE::INTRO))
            StateDebugStr += "INTRO ";

        if (eState == ENUM_TO_UINT(CMonster::MONSTER_BASE_STATE::IDLE))
            StateDebugStr += "IDLE ";

        if (eState == ENUM_TO_UINT(CMonster::MONSTER_BASE_STATE::WALK))
            StateDebugStr += "WALK";

        if (eState == ENUM_TO_UINT(CMonster::MONSTER_BASE_STATE::RUN))
            StateDebugStr += "RUN";

        if (eState == ENUM_TO_UINT(CMonster::MONSTER_BASE_STATE::JUMP))
            StateDebugStr += "JUMP ";

        if (eState == ENUM_TO_UINT(CMonster::MONSTER_BASE_STATE::ATTACK))
            StateDebugStr += "ATTACK ";

        if (eState == ENUM_TO_UINT(CMonster::MONSTER_BASE_STATE::DAMAGE))
            StateDebugStr += "DAMAGE ";

        if (eState == ENUM_TO_UINT(CMonster::MONSTER_BASE_STATE::DIE))
            StateDebugStr += "DIE";


        if (eState == ENUM_TO_UINT(JackyState::ESCAPE))
            StateDebugStr += "ESCAPE";

        if (eState == ENUM_TO_UINT(JackyState::LIFTING))
            StateDebugStr += "LIFTING";

        if (eState == ENUM_TO_UINT(JackyState::THROW))
            StateDebugStr += "THROW";

    }



    return StateDebugStr;
}
