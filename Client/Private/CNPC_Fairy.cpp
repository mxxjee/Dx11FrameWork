#include "CNPC_Fairy.h"
#include "CPlayer.h"
#include "CAnimBody.h"
#include "CAnimation.h"
#include "CModel.h"
#include "CQuest_Manager.h"
#include "CInteraction_TriggerBox.h"
#include "CParticle.h"
#include "CEffectPoolManager.h"



USING(Client)
CNPC_Fairy::CNPC_Fairy(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    :CNPC(pDevice,pContext)
{
}

CNPC_Fairy::CNPC_Fairy(const CNPC_Fairy& rhs)
    : CNPC(rhs)
{
}

HRESULT CNPC_Fairy::Initialize_Prototype(void* pArg)
{
    if (FAILED(__super::Initialize_Prototype(pArg)))
        return E_FAIL;

    //st-lp-ed
    Reigster_AnimNotify();

    m_pGameInstance->RegisterListners("Enter_Forest", [this](const GameEvent& event)
        {
            if(!m_bEnd)
                Set_Active(true);
        });

    //대화끝나면알아서 애니메이션재생
    m_pGameInstance->RegisterListners("Fairy_End", [this](const GameEvent& event)
        {
            if (m_pNormalEffect)
                m_pNormalEffect->Set_Loop(false);

            m_pAnimBody->Reserve_Animation(L"heel_st", false);
            m_pGameInstance->BroadCastEvent(L"OnTalkUIHide", (void*)nullptr);
            m_pGameInstance->BroadCastEvent(L"OnDialogueUIHide", (void*)nullptr);
            m_pTriggerBox->Set_Active(false);
            m_bEnd = true;

        });

    m_pGameInstance->RegisterListners("Start_SpecialEffect", [this](const GameEvent& event)
        {
            CParticle::PARTICLE_DESC FairySpecialDesc;
            FairySpecialDesc.ProtoName = L"Particle";
            FairySpecialDesc.DataName = L"FairySpecial";
            FairySpecialDesc.eRenderGroup = ENUM_TO_UINT(RENDERGROUP::NONLIGHT);
            FairySpecialDesc.passName = "Default";
            FairySpecialDesc.ShaderName = L"VtxPosParticle";
            FairySpecialDesc.ObjTag = L"FairySpecial";


            m_pSpecialEffect = CEffectPoolManager::GetInstance()->Request_Spawn(FairySpecialDesc.ProtoName, &FairySpecialDesc);
            if (m_pSpecialEffect)
            {

                m_pSpecialEffect->Set_OrigniMatrix(XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr()));
                m_pSpecialEffect->Play();
            }

        });

    m_pGameInstance->RegisterListners("Fairy_Go_Loop", [this](const GameEvent& event)
        {
            m_pAnimBody->Reserve_Animation(L"heel_lp", false);

           


        });



    Enter_Interaction_Event.Payload.Floats["Float_Y"] = 7.5f;
    Enter_Interaction_Event.Payload.Floats["Float_Z"] = -5.f;
    m_ipressionIdx_Eye[EXPRESSION::HAPPY] = 2;
    m_ipressionIdx_Mouth[EXPRESSION::HAPPY] = 2;


    CParticle::PARTICLE_DESC FairyDesc;
    FairyDesc.ProtoName = L"Particle";
    FairyDesc.DataName = L"FairyParticle";
    FairyDesc.eRenderGroup = ENUM_TO_UINT(RENDERGROUP::NONLIGHT);
    FairyDesc.passName = "Default";
    FairyDesc.ShaderName = L"VtxPosParticle";
    FairyDesc.ObjTag = L"FairyParticle";


	m_pNormalEffect = CEffectPoolManager::GetInstance()->Request_Spawn(FairyDesc.ProtoName, &FairyDesc);
	if (m_pNormalEffect)
	{

        m_pNormalEffect->Set_OrigniMatrix(XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr()));
        m_pNormalEffect->Set_Loop(true);
        m_pNormalEffect->Play();
	}



    return S_OK;
}

void CNPC_Fairy::Update_Priority(_float fTimeDelta)
{
    __super::Update_Priority(fTimeDelta);
}

void CNPC_Fairy::Update(_float fTimeDelta)
{
    __super::Update(fTimeDelta);
}

void CNPC_Fairy::Update_Late(_float fTimeDelta)
{
    __super::Update_Late(fTimeDelta);
}

void CNPC_Fairy::Update_Render(_float fTimeDelta)
{
    __super::Update_Render(fTimeDelta);
}

HRESULT CNPC_Fairy::Render()
{
    return S_OK;
}

void CNPC_Fairy::Exit_Interaction()
{
    m_bTalking = false;
    m_bIsTalking = false;
    m_pGameInstance->BroadCastEvent(L"OnDialogueUIHide", nullptr);
    m_pAnimBody->Reserve_Animation(L"wait", true);
    
    m_pPlayer->Get_ActionControl()->m_bTalk = false;

    if(CQuest_Manager::GetInstance()->Get_QuestState(1003)!= QuestState::COMPLETABLE)
        m_pGameInstance->Emit(Exit_Interaction_Event);



    m_bPrevRange = false;
    m_bPrevInteracting = false;
}

void CNPC_Fairy::Reigster_AnimNotify()
{
    GameEvent Event;
    EventPayload payload;
    Event.Payload = payload;

    m_pAnimBody->Set_Animation_Speed(L"heel_st", 50.f);
    m_pAnimBody->Set_Animation_Speed(L"heel_lp", 50.f);
    m_pAnimBody->Set_Animation_Speed(L"heel_ed", 50.f);

    //st->lp
    CAnimation* pAnim = m_pAnimBody->Get_Model()->Find_Animation(L"heel_st");
    
    if (pAnim)
    {

        Event.Name = "Start_SpecialEffect";
        pAnim->AddNotify(93, Event);

        Event.Name = "Fairy_Go_Loop";
        pAnim->AddNotify(127, Event);

    }

    pAnim = m_pAnimBody->Get_Model()->Find_Animation(L"heel_lp");

    if (pAnim)
    {

        Event.Name = "FadeScreen_Before_WitchRoom";
        pAnim->AddNotify(80, Event);

        Event.Name = "Go_WitchRoom";
        pAnim->AddNotify(90, Event);
    }

    pAnim = m_pAnimBody->Get_Model()->Find_Animation(L"heel_ed");
 /*   if (pAnim)
    {
        Event.Name = "Go_";
        pAnim->AddNotify(100, Event);
    }*/
}

CNPC_Fairy* CNPC_Fairy::Create(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pDeviceContex, void* pArg)
{
    CNPC_Fairy* pInstance = new CNPC_Fairy(_pDevice, _pDeviceContex);
    if (FAILED(pInstance->Initialize_Prototype(pArg)))
    {
        MSG_BOX("Failed to Create :CNPC_Fairy ");
        Safe_Release(pInstance);

    }

    return pInstance;
}

void CNPC_Fairy::Free()
{
    __super::Free();
}
