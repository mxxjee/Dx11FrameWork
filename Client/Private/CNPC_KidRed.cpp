#include "CNPC_KidRed.h"
#include "CAnimBody.h"
#include "CPlayer.h"
#include "CQuest_Manager.h"
#include "CInteraction_Manager.h"
#include "CGameManager.h"


USING(Client)
CNPC_KidRed::CNPC_KidRed(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    :CNPC(pDevice, pContext)
{
}

CNPC_KidRed::CNPC_KidRed(const CNPC_KidRed& rhs)
    : CNPC(rhs)
{
}

HRESULT CNPC_KidRed::Initialize_Prototype(void* pArg)
{
    if (FAILED(__super::Initialize_Prototype(pArg)))
        return E_FAIL;

    m_pAnimBody->Set_Animation_Speed(L"run", 50.f);
    m_pAnimBody->Set_Animation_Speed(L"show", 50.f);
    m_pAnimBody->Set_Animation_Speed(L"wait", 50.f);
    m_pAnimBody->Set_Animation_Speed(L"walk", 80.f);


    m_pAnimBody->Reserve_Animation(L"wait", true, false);

    XMStoreFloat4(&m_vInitPosition, m_pTransformCom->Get_State(STATE::POSITION));
    return S_OK;
}

void CNPC_KidRed::Update_Priority(_float fTimeDelta)
{
    __super::Update_Priority(fTimeDelta);
}

void CNPC_KidRed::Update(_float fTimeDelta)
{
    __super::Update(fTimeDelta);
    if (m_bStartEvent)
        Update_State(fTimeDelta);
}

void CNPC_KidRed::Update_Late(_float fTimeDelta)
{
    __super::Update_Late(fTimeDelta);
}

void CNPC_KidRed::Update_Render(_float fTimeDelta)
{
    __super::Update_Render(fTimeDelta);
}

HRESULT CNPC_KidRed::Render()
{
    __super::Render();

    return S_OK;
}

void CNPC_KidRed::Walk_StartFirstChapter(_float fTimeDelta)
{
    CheckFalse(m_pGameManager->Get_EndingStep() == EndingStep::START_DIALOGUE);

    m_pGameManager->Set_UseCutScene(true);
    m_pGameManager->Set_CutSceneType(CGameManager::CUTSCENE_TYPE::ENDING);

    m_pPlayer->Get_Transform()->LookAtSmooth(Get_Transform()->Get_State(STATE::POSITION),2.f,fTimeDelta);

    _vector PlayerPos = m_pPlayer->Get_Transform()->Get_State(STATE::POSITION);

    m_pTransformCom->LookAtSmooth(PlayerPos, 3.f, fTimeDelta);
    m_pTransformCom->Move(DIRECTION::FORWARD, fTimeDelta, Space::Local);

    m_pAnimBody->Reserve_Animation(L"run", true, false);
    //멈추는조건 (플레이ㅓㅇ와 가까워지면)
    bool bResult = m_pTransformCom->Chase(PlayerPos, fTimeDelta, nullptr, 2.f);
    if (bResult)
    {
        m_iState = State::WAIT;
        m_pAnimBody->Reserve_Animation(L"wait", true, false);

        m_pGameInstance->Invoke(1.f, 0.f, false, false, [this]()
            {

                m_pDialogue_Manager->StartDialogue("NPC_Kid_Red");
                CInteraction_Manager::GetInstance()->Set_CurrentTarget(this);

                m_iState = State::TALK;

            }, this);

    }
}

void CNPC_KidRed::Walk_EndFirstChapter(_float fTimeDelta)
{

    EndingStep endingStep = m_pGameManager->Get_EndingStep();
    if (endingStep == EndingStep::ESCAPE_BOSS || endingStep == EndingStep::FOLLOW_KID)
    {
        m_pAnimBody->Reserve_Animation(L"run", true, false);
        m_pTransformCom->LookAt(XMLoadFloat4(&m_vInitPosition));


        bool bResult = m_pTransformCom->Chase(XMLoadFloat4(&m_vInitPosition), fTimeDelta, nullptr, 1.f);

        if (bResult)
        {
            //다음씬 페이드아웃
            GameEvent Event;
            Event.Name = "Go_to_Ending";

            m_pGameInstance->Emit(Event);
            m_pGameManager->Set_EndingStep(EndingStep::GO_TOWN);
        }

    }
  

}

void CNPC_KidRed::Wait_EndFirstChapter(_float fTimeDelta)
{
    CheckFalse(m_pGameManager->Get_EndingStep() == EndingStep::ESCAPE_BOSS);
    
    m_pGameInstance->Invoke(1.f, false, false,false, [this]()
        {
            m_iState = State::WALK;
        }, this);

  

}

void CNPC_KidRed::Enter_InteractRange()
{
}

void CNPC_KidRed::Enter_Interaction()
{
}

void CNPC_KidRed::Stay_Interaction(_float fTimeDelta)
{
}

void CNPC_KidRed::Exit_Interaction()
{
    m_bTalking = false;
    m_pGameInstance->BroadCastEvent(L"OnDialogueUIHide", nullptr);


}

void CNPC_KidRed::Enter_State(int newState)
{
    m_bTalking = true;
    m_bIsTalking = true;

    if (m_bUseCameraEvent)
        m_pGameInstance->Emit(Enter_Interaction_Event);

}

void CNPC_KidRed::Update_State(_float fTimeDelta)
{
    switch (m_iState)
    {
    case State::WALK:
    {
        Walk_StartFirstChapter(fTimeDelta);
        Walk_EndFirstChapter(fTimeDelta);
    }
    break;


    case State::WAIT:
    {

        Wait_EndFirstChapter(fTimeDelta);

    }
    break;

    case State::TALK:
    {

        

    }
    break;
    }
}

CNPC_KidRed* CNPC_KidRed::Create(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pDeviceContex, void* pArg)
{
    CNPC_KidRed* pInstance = new CNPC_KidRed(_pDevice, _pDeviceContex);
    if (FAILED(pInstance->Initialize_Prototype(pArg)))
    {
        MSG_BOX("Failed to Create :CNPC_KidRed ");
        Safe_Release(pInstance);

    }

    return pInstance;
}

void CNPC_KidRed::Free()
{
    __super::Free();
}

void CNPC_KidRed::End_RedKid()
{
    m_iState = State::WALK;
    CGameManager::GetInstance()->Set_UseCutScene(true);
    CGameManager::GetInstance()->Set_EndingStep(EndingStep::ESCAPE_BOSS);

    m_pGameInstance->Invoke(3.f, false, false, false, [this]()
        {
            CGameManager::GetInstance()->Set_EndingStep(EndingStep::FOLLOW_KID);
            m_iState = State::WALK;
        }, this);

}
