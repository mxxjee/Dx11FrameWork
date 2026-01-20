#include "CNPC_Tarin.h"
#include "CAnimBody.h"
#include "CNavigation.h"
#include "CCell.h"
#include "CPlayer.h"
#include "CDialogue_Manager.h"
#include "CInteraction_TriggerBox.h"
#include "CGameManager.h"
#include "CQuest_Manager.h"
#include "CInteraction_Manager.h"


USING(Client)
CNPC_Tarin::CNPC_Tarin(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    :CNPC(pDevice,pContext)
{
}

CNPC_Tarin::CNPC_Tarin(const CNPC_Tarin& rhs)
    : CNPC(rhs)
{
}

HRESULT CNPC_Tarin::Initialize_Prototype(void* pArg)
{
    if (FAILED(__super::Initialize_Prototype(pArg)))
        return E_FAIL;


    CameraEvent.Name = "Exit_Interaction_NPC";
    CameraEvent.Payload.Ptrs["Player"] = m_pPlayer;


    m_pAnimBody->Set_Animation_Speed(L"wait", 50.f);
    m_pAnimBody->Set_Animation_Speed(L"walk", 80.f);

    m_pAnimBody->Reserve_Animation(L"wait", true, false);

    return S_OK;
}

void CNPC_Tarin::Update_Priority(_float fTimeDelta)
{
    __super::Update_Priority(fTimeDelta);
}

void CNPC_Tarin::Update(_float fTimeDelta)
{
    __super::Update(fTimeDelta);
    if (m_bStartEvent)
        Update_State(fTimeDelta);

}

void CNPC_Tarin::Update_Late(_float fTimeDelta)
{
    __super::Update_Late(fTimeDelta);
}

void CNPC_Tarin::Update_Render(_float fTimeDelta)
{
    __super::Update_Render(fTimeDelta);

}

HRESULT CNPC_Tarin::Render()
{
    __super::Render();

    return S_OK;
}

void CNPC_Tarin::Enter_InteractRange()
{
    CheckFalse(m_pTriggerBox->Is_Active());
    _vector ShowPos = MathUtils::WorldToScreen(m_pTransformCom->Get_State(STATE::POSITION),
        m_pGameInstance->Get_ViewMatrix(0), m_pGameInstance->Get_ProjMatrix(0), g_iWinSizeX, g_iWinSizeY);

    CheckTrue(m_pGameManager->Get_UseCutScene());

}

void CNPC_Tarin::Exit_Interaction()
{
    CheckFalse(m_pTriggerBox->Is_Active());
    m_bTalking = false;
    m_pGameInstance->BroadCastEvent(L"OnDialogueUIHide", nullptr);
    m_bIsTalking = false;
    m_pPlayer->Get_ActionControl()->m_bTalk = false;
    m_pAnimBody->Reserve_Animation(L"wait", true);

    m_bPrevRange = false;
    m_bPrevInteracting = false;


    m_pGameInstance->Emit(CameraEvent);

    if (CQuest_Manager::GetInstance()->Get_NPC_Chapter("NPC_Tarin") == "Wait")
    {
        m_pPlayer->Exit_RichardChapterEvent();
        m_iState = State::WAIT;
    }
}

void CNPC_Tarin::Start_SecondChapter()
{
   /* m_pGameInstance->Invoke(4.f, false, false, false, [this]()
        {
            
          Enter_Interaction();

      
        }, this);*/
}

void CNPC_Tarin::Update_State(_float fTimeDelta)
{
    switch (m_iState)
    {
    case State::WALK:
    {   
        _vector PlayerPos = m_pPlayer->Get_Transform()->Get_State(STATE::POSITION);

        m_pTransformCom->LookAtSmooth(PlayerPos, 3.f, fTimeDelta);

        m_pTransformCom->Move(DIRECTION::FORWARD, fTimeDelta, Space::Local, m_pNavigationCom);
        m_pAnimBody->Reserve_Animation(L"walk", true, false);
        //멈추는조건 (플레이ㅓㅇ와 가까워지면)
        bool bResult = m_pTransformCom->Chase(PlayerPos, fTimeDelta, m_pNavigationCom, 2.f);
        if (bResult)
        {
            m_iState = State::WAIT;
            m_pGameInstance->Invoke(0.5f, 0.f, false, false, [this]()
                {
                    CQuest_Manager::GetInstance()->Set_NPC_Chapter("NPC_Tarin", "Q2001_Start");

                    CInteraction_Manager::GetInstance()->Set_CurrentTarget(this);

                    m_iState = State::TALK;
                   
                }, this);

        }

    }
        break;


	case State::WAIT:
    {

        m_pAnimBody->Reserve_Animation(L"wait", true, false);
        

    }   
    break;

    case State::TALK:
    {

        m_pAnimBody->Reserve_Animation(L"talk", true, false);
        

    }
    break;
    }
}

CNPC_Tarin* CNPC_Tarin::Create(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pDeviceContex, void* pArg)
{
    CNPC_Tarin* pInstance = new CNPC_Tarin(_pDevice, _pDeviceContex);
    if (FAILED(pInstance->Initialize_Prototype(pArg)))
    {
        MSG_BOX("Failed to Create :CNPC_Tarin ");
        Safe_Release(pInstance);

    }

    return pInstance;
}

void CNPC_Tarin::Free()
{
    __super::Free();
}
