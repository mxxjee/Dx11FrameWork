#include "CNPC_Witch.h"
#include "CPlayer.h"
#include "CInteraction_TriggerBox.h"
#include "CAnimBody.h"
#include "CDialogue_Manager.h"
#include "CSocket_Model.h"
#include "CAnimation.h"
#include "CAnimNotify.h"
#include "CModel.h"

#include "CInventory_Manager.h"
#include "CParticle.h"
#include "CEffectPoolManager.h"

#include "CInput_Manager.h"




USING(Client)
CNPC_Witch::CNPC_Witch(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    :CNPC(pDevice, pContext)
{
}

CNPC_Witch::CNPC_Witch(const CNPC_Witch& rhs)
    : CNPC(rhs)
{
}

HRESULT CNPC_Witch::Initialize_Prototype(void* pArg)
{
    NPC_DESC* pNpcDesc = static_cast<NPC_DESC*>(pArg);
    
    pNpcDesc->TalkRange = 3.f;

    pNpcDesc->bUseNavMesh = false;

    if (FAILED(__super::Initialize_Prototype(pArg)))
        return E_FAIL;

    if (FAILED(Create_SocketObject()))
        return E_FAIL;


    Ready_Events();
    Reigster_AnimNotify();

    if (FAILED(Register_Listners()))
        return E_FAIL;


    Enter_Interaction_Event.Payload.Floats.at("Float_Y") = 7.5f;
    Enter_Interaction_Event.Payload.Floats.at("Float_Z") = -6.f;

    return S_OK;
}

void CNPC_Witch::Update_Priority(_float fTimeDelta)
{
    __super::Update_Priority(fTimeDelta);
}

void CNPC_Witch::Update(_float fTimeDelta)
{
    __super::Update(fTimeDelta);
    if (CInput_Manager::GetInstance()->IsKeyPressed(KeyCode::E))
    {

        CParticle::PARTICLE_DESC Desc;
        Desc.ProtoName = L"Particle";
        Desc.DataName = L"Smoke";
        Desc.eRenderGroup = ENUM_TO_UINT(RENDERGROUP::NONLIGHT);
        Desc.passName = "Smoke";
        Desc.ShaderName = L"VtxPosParticle";
        Desc.ObjTag = L"Smoke";

        CEffect* pEffect = CEffectPoolManager::GetInstance()->Request_Spawn(Desc.ProtoName, &Desc);

        if (pEffect)
        {

            const _float4x4* Matrix = m_pTransformCom->Get_WorldMatrixPtr();

            pEffect->Set_OrigniMatrix(XMLoadFloat4x4(Matrix));
            pEffect->Play();
        }

        Desc.ProtoName = L"Particle";
        Desc.DataName = L"Explosion_Particle";
        Desc.eRenderGroup = ENUM_TO_UINT(RENDERGROUP::NONLIGHT);
        Desc.passName = "Default";
        Desc.ShaderName = L"VtxPosParticle";
        Desc.ObjTag = L"Explosion_Particle";

        pEffect = CEffectPoolManager::GetInstance()->Request_Spawn(Desc.ProtoName, &Desc);

        if (pEffect)
        {

            const _float4x4* Matrix = m_pTransformCom->Get_WorldMatrixPtr();

            pEffect->Set_OrigniMatrix(XMLoadFloat4x4(Matrix));
            pEffect->Play();
        }

    }
}

void CNPC_Witch::Update_Late(_float fTimeDelta)
{
    __super::Update_Late(fTimeDelta);
}

void CNPC_Witch::Update_Render(_float fTimeDelta)
{
    __super::Update_Render(fTimeDelta);
}

HRESULT CNPC_Witch::Render()
{
    return S_OK;
}


void CNPC_Witch::Enter_Interaction()
{
    CheckTrue(m_bWait);

    m_bTalking = true;
    CheckFalse(m_pTriggerBox->Is_Active());
    m_pAnimBody->Reserve_Animation(L"talk", true);
    m_pGameInstance->BroadCastEvent(L"OnTalkUIHide", (void*)nullptr);


    m_pGameInstance->Emit(Enter_Interaction_Event);

    
    m_pDialogue_Manager->StartDialogue(DialogueTag);
}

void CNPC_Witch::Stay_InteractRange(_float fTimeDelta)
{
    /*쳐다보지않기*/
}


void CNPC_Witch::Exit_Interaction()
{
  
    m_bTalking = false;
    m_pGameInstance->BroadCastEvent(L"OnDialogueUIHide", nullptr);
    m_pPlayer->Get_ActionControl()->m_bTalk = false;
    m_bPrevRange = false;
    m_bPrevInteracting = false;

    m_bEnd = true;

    CheckTrue(m_bWait);
    m_pGameInstance->Emit(Exit_Interaction_Event);

}

void CNPC_Witch::Reigster_AnimNotify()
{
    GameEvent Event;
    EventPayload payload;
    Event.Payload = payload;

    m_pAnimBody->Set_Animation_Speed(L"mix", 55.f);
    m_pAnimBody->Set_Animation_Speed(L"give", 55.f);

    //st->lp
    CAnimation* pAnim = m_pAnimBody->Get_Model()->Find_Animation(L"mix");
    if (pAnim)
    {
        Event.Name = "Start_Mix";
        pAnim->AddNotify(16, Event);

        Event.Name = "Hide_Mushroom"; 
        pAnim->AddNotify(66, Event);

        Event.Name = "Play_WitchRoomMusic_Fast";
        pAnim->AddNotify(80, Event);


        Event.Name = "Witch_Effect";
        pAnim->AddNotify(309, Event);

        Event.Name = "Witch_Effect2";
        pAnim->AddNotify(400, Event);

        Event.Name = "End_Mix";
        pAnim->AddNotify(426, Event);

    }

    pAnim = m_pAnimBody->Get_Model()->Find_Animation(L"give");
    if (pAnim)
    {
        Event.Name = "Start_give";
        pAnim->AddNotify(20, Event);

    }
    
    
}

void CNPC_Witch::Ready_Events()
{
    
    m_pGameInstance->RegisterListners("Play_Witch_Mix", [this](const GameEvent& evt)
        {
            m_pAnimBody->Reserve_Animation(L"mix", false);
            m_pGameInstance->BroadCastEvent(L"OnTalkUIHide", (void*)nullptr);
            m_pSocketMushroom->Set_Active(true);

         
        });


    m_pGameInstance->RegisterListners("Witch_give", [this](const GameEvent evt)
        {

            m_pAnimBody->Reserve_Animation(L"wait", false);
            CInventory_Manager::GetInstance()->Request_Add_To_Inven(ItemType::MAGIC_POWDER, 50);
            m_pMagicPowder->Set_Active(false);

          
        });


    /*마녀 이벤트 - Particle 생성 이펙트(연기)*/
    m_pGameInstance->RegisterListners("Witch_Effect", [this](const GameEvent evt)
        {

            CParticle::PARTICLE_DESC Desc;
            Desc.ProtoName = L"Particle";
            Desc.DataName = L"Smoke";
            Desc.eRenderGroup = ENUM_TO_UINT(RENDERGROUP::NONLIGHT);
            Desc.passName = "Smoke";
            Desc.ShaderName = L"VtxPosParticle";
            Desc.ObjTag = L"Smoke";

            CEffect* pEffect = CEffectPoolManager::GetInstance()->Request_Spawn(Desc.ProtoName, &Desc);

            if (pEffect)
            {

                const _float4x4* Matrix =m_pTransformCom->Get_WorldMatrixPtr();

                pEffect->Set_OrigniMatrix(XMLoadFloat4x4(Matrix));
                pEffect->Play();
            }

            m_pGameInstance->PlaySoundW(L"Effects/Explosion2.wav",CHANNELID::SOUND_NPC_SFX2,g_EffectVolume);

        });

    m_pGameInstance->RegisterListners("Witch_Effect2", [this](const GameEvent evt)
        {

            CParticle::PARTICLE_DESC Desc;
            Desc.ProtoName = L"Particle";
            Desc.DataName = L"Explosion_Particle";
            Desc.eRenderGroup = ENUM_TO_UINT(RENDERGROUP::NONLIGHT);
            Desc.passName = "Default";
            Desc.ShaderName = L"VtxPosParticle";
            Desc.ObjTag = L"Explosion_Particle";

            CEffect* pEffect = CEffectPoolManager::GetInstance()->Request_Spawn(Desc.ProtoName, &Desc);

            if (pEffect)
            {

                const _float4x4* Matrix = m_pTransformCom->Get_WorldMatrixPtr();

                pEffect->Set_OrigniMatrix(XMLoadFloat4x4(Matrix));
                pEffect->Play();
            }


        });

}

HRESULT CNPC_Witch::Create_SocketObject()
{
    CSocket_Model::SOKET_MODEL_DESC Desc;
    Desc.pSocketMatrix = m_pBody->Get_SocketMatrix("attach_L");
    Desc.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
    Desc.m_iLevelID = ENUM_TO_UINT(LEVEL_ID::ROOM);
    Desc.ModelName = L"Mushroom";


    if (FAILED(__super::Add_PartObject(0, PROTO_OBJ_NAME(L"Soket_Model"), L"Soket_Mushroom", &Desc)))
        return E_FAIL;

    m_pSocketMushroom =dynamic_cast<CSocket_Model*>(Find_PartObject(L"Soket_Mushroom"));
    m_pSocketMushroom->Set_Active(false);


    /// /////////////
    CSocket_Model::SOKET_MODEL_DESC MagirPowderDesc;
    MagirPowderDesc.pSocketMatrix = m_pBody->Get_SocketMatrix("attach_L");
    MagirPowderDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
    MagirPowderDesc.m_iLevelID = ENUM_TO_UINT(LEVEL_ID::ROOM);
    MagirPowderDesc.ModelName = L"MagicPowder";


    if (FAILED(__super::Add_PartObject(0, PROTO_OBJ_NAME(L"Soket_Model"), L"Soket_MagicPowder", &MagirPowderDesc)))
        return E_FAIL;

    m_pMagicPowder = dynamic_cast<CSocket_Model*>(Find_PartObject(L"Soket_MagicPowder"));
    m_pMagicPowder->Set_Active(false);

    return S_OK;
}

HRESULT CNPC_Witch::Register_Listners()
{

    //애님노티파이들
    m_pGameInstance->RegisterListners("Start_Mix", [this](const GameEvent& event)
        {
            m_pSocketMushroom->Set_Active(true);
            m_bWait = true;
           

        });

    m_pGameInstance->RegisterListners("Play_WitchRoomMusic_Fast", [this](const GameEvent& event)
        {
            m_pGameInstance->PlayBGM(L"BGM/WitchRoom_Fast.wav", g_BGMVolume);
        });

    m_pGameInstance->RegisterListners("Hide_Mushroom", [this](const GameEvent& event)
        {
            m_pSocketMushroom->Set_Active(false);

        });
    
    m_pGameInstance->RegisterListners("End_Mix", [this](const GameEvent& event)
        {
            m_pGameInstance->Emit(Rotate_Camera_End_Event);
            m_pAnimBody->Reserve_Animation(L"give", false);
            m_pMagicPowder->Set_Active(true);

            m_pGameInstance->Emit(Enter_Interaction_Event);
            m_pDialogue_Manager->StartDialogue(DialogueTag);
            m_bWait = false;

        });

    return S_OK;
}

CNPC_Witch* CNPC_Witch::Create(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pDeviceContex, void* pArg)
{
    CNPC_Witch* pInstance = new CNPC_Witch(_pDevice, _pDeviceContex);
    if (FAILED(pInstance->Initialize_Prototype(pArg)))
    {
        MSG_BOX("Failed to Create :CNPC_Fairy ");
        Safe_Release(pInstance);

    }

    return pInstance;
}

void CNPC_Witch::Free()
{
    __super::Free();
}
