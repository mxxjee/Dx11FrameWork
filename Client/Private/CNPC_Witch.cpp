#include "CNPC_Witch.h"
#include "CPlayer.h"

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

   
    return S_OK;
}

void CNPC_Witch::Update_Priority(_float fTimeDelta)
{
    __super::Update_Priority(fTimeDelta);
}

void CNPC_Witch::Update(_float fTimeDelta)
{
    __super::Update(fTimeDelta);
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

void CNPC_Witch::Exit_Interaction()
{
    m_bTalking = false;
    m_pGameInstance->BroadCastEvent(L"OnDialogueUIHide", nullptr);
    m_pPlayer->Get_ActionControl()->m_bTalk = false;
    m_bPrevRange = false;
    m_bPrevInteracting = false;
}

void CNPC_Witch::Stay_InteractRange(_float fTimeDelta)
{
    /*쳐다보지않기*/
}

void CNPC_Witch::Reigster_AnimNotify()
{
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
