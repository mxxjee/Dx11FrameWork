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
#include "CMonsterDamageState.h"
#include "CMonsterDeadState.h"


#include "CGameManager.h"
#include "CAnimation.h"
#include "CModel.h"

#include "CPlayer.h"
#include "CInteraction_JackyBall.h"
#include "CMJackyLiftState.h"
#include "CMJackyThrowState.h"
#include "CInteraction_TriggerBox.h"
#include "CMJackyEscapeState.h"
#include "CLayer.h"

#include "CLevel.h"
#include "CModel.h"

#ifdef _DEBUG
#include "CImGui_Manager.h"
#endif // _DEBUG

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

    if (FAILED(Ready_EventLisnters()))
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
    Check_Escape();
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

        m_pMonsterBody->Register_AnimKey(ENUM_TO_UINT(CMonster::MONSTER_BASE_STATE::DIE), L"dead");


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
    m_pMonsterBody->Set_Animation_Speed(L"damage", 50.f);

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

   m_States.emplace(CMonster::MONSTER_BASE_STATE::DAMAGE, CMonsterDamageState::Create());


   m_States.emplace(CM_Jacky::MONSTER_BASE_STATE::DIE, CMonsterDeadState::Create());




    return S_OK;
}

HRESULT CM_Jacky::Ready_EventLisnters()
{
    m_pGameInstance->RegisterListners("Start_Boss", [this](const GameEvent event)
        {
            m_bStart = true;

            m_pGameInstance->Invoke(2.f, false, false, false, [this]()
                {
                    //대기 후 1초뒤 양옆으로이동
                    CLevel* pLevel = m_pGameInstance->Get_CurrentLevel();
                    if (pLevel)
                        pLevel->Play_LevelBGM();

                    m_ActionControl.m_bMove = true;
                }, this);

        });

    m_pGameInstance->RegisterListners("BossWalk_Sound", [this](const GameEvent event)
        {
            m_pGameInstance->PlaySoundW(L"Monster/BossWalk.wav", CHANNELID::SOUND_MONSTER_HIT, 0.2f);

        });

    GameEvent Event;
    EventPayload payload;
    Event.Payload = payload;

    
    CAnimation* pAnim = m_pBody->Get_Model()->Find_Animation(L"lifting_move");
    if (pAnim)
    {
        Event.Name = "BossWalk_Sound";
        pAnim->AddNotify(25, Event);

    }

    pAnim = m_pBody->Get_Model()->Find_Animation(L"move");
    if (pAnim)
    {
        Event.Name = "BossWalk_Sound";
        pAnim->AddNotify(21, Event);

    }
    return S_OK;
}

void CM_Jacky::Enter_State(int newState)
{
    __super::Enter_State(newState);
    switch (newState)
    {
    case ENUM_TO_UINT(CMonster::MONSTER_BASE_STATE::IDLE):
    {
        CheckFalse(m_bStart);

        m_pGameInstance->Invoke(2.f, false, false, false, [this]()
            {
                //대기 후 1초뒤 양옆으로이동
                m_ActionControl.m_bMove = true;
        
            }, this);
    }
    break;

    case ENUM_TO_UINT(CMonster::MONSTER_BASE_STATE::WALK):
    {
        /*GameEvent Event;
        Event.Name = "Walk_Sound";

        m_pGameInstance->Emit(Event);*/

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
       
        _vector vMyPos = m_pTransformCom->Get_State(STATE::POSITION);
        _vector vPlayerPos = XMVector3Normalize(m_pTransformCom->Get_State(STATE::LOOK) * (-1.f));


        m_vTargetPos = vMyPos + XMVector3Normalize(vMyPos - vPlayerPos);


    }

    break;

    case ENUM_TO_UINT(CMonster::DIE):
    {
        m_pChaseTarget->Get_Body()->Set_EnableDissolve(true);
        m_pChaseTarget->Get_Body()->Set_DissolveTexture(L"dissolve_02");
        m_pChaseTarget->Get_Body()->Set_PassName("Dissolve");
        m_pChaseTarget->Set_Dead();

        m_pGameInstance->PlaySoundW(L"Monster/BossDead.wav", CHANNELID::SOUND_MONSTER_DEATH, g_EffectVolume);

        m_pGameInstance->StopSoundFade(CHANNELID::SOUND_BGM, 1.f);


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
        Damage_Behavior(fTimeDelta);
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

void CM_Jacky::Update_DeadState(_float fTimeDelta)
{
    __super::Update_DeadState(fTimeDelta);

    if (m_bStartDissolve)
    { 
        m_pChaseTarget->Set_Dead();

    }
}

void CM_Jacky::Set_Dead()
{
    CheckFalse(m_bActive);

    Set_Active(false);

    if (m_AfterDeadEvent)
        m_AfterDeadEvent();


    CLayer* pLayer = m_pGameInstance->Find_Layer(m_iLevelID, L"Monster_Layer");
    pLayer->RequestDestroy(this);

#if _DEBUG
    //selectobjec관련들 다 초기화
    CImGui_Manager::GetInstance()->Reset_Window("ObjectDebugWindow");
    CImGui_Manager::GetInstance()->Reset_Window("StateDebugWindow");

#endif

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

    m_pTransformCom->Set_Speed(m_fInitSpeed);


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
    m_pTransformCom->Set_Speed(m_fInitSpeed*1.5f);

    //도망가라.
    //
   

    m_pTransformCom->Move(DIRECTION::FORWARD, fTimeDelta, Space::Local, m_pNavigationCom);


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

void CM_Jacky::Check_Escape()
{
    CGameObject* pOwner = m_pChaseTarget->Get_Owner();

    if (pOwner != nullptr)
    {
        if (pOwner != this)
            m_JackyActionInput.bEscape = true;

    }

    else
        m_JackyActionInput.bEscape = false;

}

void CM_Jacky::Grab(CInteractionObject* pObj)
{
    CheckTrue(m_iState == ENUM_TO_UINT(CMonster::MONSTER_BASE_STATE::DAMAGE) ||
        m_iState == ENUM_TO_UINT(CMonster::MONSTER_BASE_STATE::DIE));


    CheckTrue(m_JackyActionInput.bLift);
    CheckTrue(m_ActionControl.m_bDamage == 1.f);

    if (m_pChaseTarget->Get_Owner() != nullptr)
        return;


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

void CM_Jacky::OnCollisionEnter(_uint iGroup, CCollider_Base* pOther)
{
    CheckFalse(m_bCanCollision);
    CheckTrue(m_ActionControl.m_bDamage == 1.f);
    CheckTrue(Is_Carrying());

    CGameObject* pOwner = pOther->Get_Owner();
    CheckNull(pOwner);

    switch (COLLISION_GROUP(iGroup))
    {
    case COLLISION_GROUP::INTERACTION:
    {
        if (pOwner->Get_Tag() == L"JackyIronBall0")
        {
            CGameObject* pBallOwner = m_pChaseTarget->Get_Owner();
            CheckNull(pBallOwner);//오너가 없으면 그냥 굴러가는 볼

            if (pBallOwner != this)
            {
                m_ActionControl.m_bDamage = 1.f;
                --iHp;

                if (pBallOwner)
                {

                    //pOther을 바라보고,
                    if (iHp > 0)
                    {
                        m_pTransformCom->LookAt(pBallOwner->Get_Transform());

                        _float3 vDir;
                        XMStoreFloat3(&vDir, m_pTransformCom->Get_State(STATE::LOOK));
                        m_pTransformCom->AddImpulse(-0.3f, vDir);

                    }

                    const _float4x4* CombinedMatrix = m_pChaseTarget->Get_Transform()->Get_WorldMatrixPtr();
                    Spawn_HitSparkle(XMLoadFloat4x4(CombinedMatrix));

                }
            }

            

        }
        
    }

    break;
    }


}
