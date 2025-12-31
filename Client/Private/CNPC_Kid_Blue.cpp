#include "CNPC_Kid_Blue.h"
#include "CPlayer.h"
#include "CAnimBody.h"
#include "CAnimation.h"
#include "CModel.h"
#include "CDialogue_Manager.h"
#include "CMeshComponent.h"

#include "CShader.h"
#include "CInteraction_TriggerBox.h"




USING(Client)
CNPC_Kid_Blue::CNPC_Kid_Blue(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    :CNPC(pDevice, pContext)
{
}

CNPC_Kid_Blue::CNPC_Kid_Blue(const CNPC_Kid_Blue& rhs)
    :CNPC(rhs)
{
}

HRESULT CNPC_Kid_Blue::Initialize_Prototype(void* pArg)
{
    if (FAILED(__super::Initialize_Prototype(pArg)))
        return E_FAIL;

    m_pGameInstance->RegisterListners("Enter_Forest", [this](const GameEvent& event)
        {
            Set_Active(true);
        });

    m_pAnimBody->Reserve_Animation(L"panic", true, false);

    ///////////Notify///////
    GameEvent Event;
    EventPayload payload;
    Event.Payload = payload;
    CContainerObject* pNpc = this;
    
    Event.Name = "InteractionBegin";
    Event.Payload.Ptrs["NPC"] = pNpc;



    CModel* pModel = m_pAnimBody->Get_Model();
    CAnimation* pAnim = pModel->Find_Animation(L"panic");
    pAnim->AddNotify(46, Event);//이 애니메이션 끝에서 인터렉션중인지 체크

    m_pGameInstance->RegisterListners("InteractionBegin", [](const GameEvent& event)
        {
            CNPC_Kid_Blue* pNpc = static_cast<CNPC_Kid_Blue*>(event.Payload.Ptrs.at("NPC"));
            if (pNpc)
            {
                pNpc->Check_Interaction();
            }
        });


    //알파값조절을 위한 셰이더
    pModelShader = pModel->Get_Shader();



    return S_OK;
}

void CNPC_Kid_Blue::Update_Priority(_float fTimeDelta)
{

    __super::Update_Priority(fTimeDelta);
}

void CNPC_Kid_Blue::Update(_float fTimeDelta)
{
    __super::Update(fTimeDelta);
}

void CNPC_Kid_Blue::Update_Late(_float fTimeDelta)
{
    __super::Update_Late(fTimeDelta);
    if (m_bEnd)
    {
        m_fAlpha -= fTimeDelta;
        m_pGameInstance->BroadCastEvent(L"OnTalkUIHide", (void*)nullptr);

    }

    if (m_fAlpha <= 0)
    {
        Set_Active(false);
        m_pGameInstance->BroadCastEvent(L"OnDialogueUIHide", nullptr);
            }


}

void CNPC_Kid_Blue::Update_Render(_float fTimeDelta)
{
    
    __super::Update_Render(fTimeDelta);
}

HRESULT CNPC_Kid_Blue::Render()
{
    CheckNullResult(pModelShader,E_FAIL);

    if (FAILED(pModelShader->Bind_Float("g_Alpha", m_fAlpha)))
        return E_FAIL;
     __super::Render();

    return S_OK;
}

void CNPC_Kid_Blue::Check_Interaction()
{
    if (m_bTalking)
        m_pAnimBody->Reserve_Animation(L"talk", true);

}


void CNPC_Kid_Blue::Enter_Interaction()
{
    m_bTalking = true;
    m_pGameInstance->BroadCastEvent(L"OnTalkUIHide", (void*)nullptr);
 //   m_pGameInstance->BroadCastEvent(L"OnDialogueUIShow", nullptr);



    m_pGameInstance->Emit(Enter_Interaction_Event);


    m_pDialogue_Manager->StartDialogue(DialogueTag);

}

void CNPC_Kid_Blue::Exit_Interaction()
{
    m_bTalking = false;
    m_pGameInstance->BroadCastEvent(L"OnDialogueUIHide", nullptr);


    m_pPlayer->Get_ActionControl()->m_bTalk = false;
    m_pGameInstance->BroadCastEvent(L"OnTalkUIHide", nullptr);

    m_bPrevRange = false;
    m_bPrevInteracting = false;

    //alpha값으로 점점사라지게,..
    CModel* pModel = m_pBody->Get_Model();
    CheckNull(pModel);

    //끝나면 알파패스로 옮기기..
    for (auto& Mesh : pModel->Get_Meshs())
    {
        Mesh.second->Set_PassName("Alpha");
    }

    m_bEnd = true;
    m_pGameInstance->Emit(Exit_Interaction_Event);
}

CNPC_Kid_Blue* CNPC_Kid_Blue::Create(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pDeviceContex, void* pArg)
{
    CNPC_Kid_Blue* pInstance = new CNPC_Kid_Blue(_pDevice, _pDeviceContex);
    if (FAILED(pInstance->Initialize_Prototype(pArg)))
    {
        MSG_BOX("Failed to Create :CNPC_Kid_Blue ");
        Safe_Release(pInstance);

    }

    return pInstance;
}

void CNPC_Kid_Blue::Free()
{
    __super::Free();
}
