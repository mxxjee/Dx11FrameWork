#include "CNPC_Fairy.h"
#include "CPlayer.h"
#include "CAnimBody.h"
#include "CAnimation.h"
#include "CModel.h"


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
            Set_Active(true);
        });

    m_pGameInstance->RegisterListners("Fairy_End", [this](const GameEvent& event)
        {
            m_pAnimBody->Reserve_Animation(L"heel_st", false);
        });


    m_pGameInstance->RegisterListners("Fairy_Go_Loop", [this](const GameEvent& event)
        {
            m_pAnimBody->Reserve_Animation(L"heel_lp", false);
        });


    m_pGameInstance->RegisterListners("Fairy_Go_End", [this](const GameEvent& event)
        {
            m_pAnimBody->Reserve_Animation(L"heel_ed", false);
        });

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

void CNPC_Fairy::Reigster_AnimNotify()
{
    GameEvent Event;
    EventPayload payload;
    Event.Payload = payload;

    //st->lp
    CAnimation* pAnim = m_pAnimBody->Get_Model()->Find_Animation(L"heel_st");

    if (pAnim)
    {
        Event.Name = "Fairy_Go_Loop";
        pAnim->AddNotify(129, Event);
    }

    pAnim = m_pAnimBody->Get_Model()->Find_Animation(L"heel_lp");

    if (pAnim)
    {
        Event.Name = "Fairy_Go_End";
        pAnim->AddNotify(100, Event);
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
