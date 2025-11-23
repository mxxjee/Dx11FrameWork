#include "CNPC_Richard.h"
#include    "CPlayer.h"
#include "CBody.h"
#include "CCamera_Base.h"


USING(Client)
CNPC_Richard::CNPC_Richard(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    :CNPC(pDevice, pContext)
{
}

CNPC_Richard::CNPC_Richard(const CNPC_Richard& rhs)
    :CNPC(rhs)
{
}

HRESULT CNPC_Richard::Initialize_Prototype(void* pArg)
{
    if (FAILED(__super::Initialize_Prototype(pArg)))
        return E_FAIL;

    return S_OK;
}

void CNPC_Richard::Update_Priority(_float fTimeDelta)
{
    __super::Update_Priority(fTimeDelta);

}

void CNPC_Richard::Update(_float fTimeDelta)
{
    __super::Update(fTimeDelta);

}

void CNPC_Richard::Update_Late(_float fTimeDelta)
{
    __super::Update_Late(fTimeDelta);
}

void CNPC_Richard::Update_Render(_float fTimeDelta)
{
    __super::Update_Render(fTimeDelta);
}

HRESULT CNPC_Richard::Render()
{
    return S_OK;
}

void CNPC_Richard::EnterInteractRange()
{
   //UI È°¼ºÈ­..

    if (m_pPlayer)
        m_pPlayer->Set_Interaction(InteractionType::NPC, this);


}

void CNPC_Richard::OnInteractRange(_float fTimeDelta)
{
    _vector PlayerPos = m_pPlayer->Get_Transform()->Get_State(STATE::POSITION);
    //m_pTransformCom->LookAt(WORLD_UP, PlayerPos, fTimeDelta, 10.f);

   _vector vUp = XMVector3Normalize(m_pTransformCom->Get_State(STATE::UP));

    m_pTransformCom->LookAtSmooth(PlayerPos,5.f,fTimeDelta);


}

void CNPC_Richard::ExitInteractRange()
{
    if (m_pPlayer)
        m_pPlayer->Reset_Interaction();


}

void CNPC_Richard::Start_Interaction()
{
    m_bTalking = true;
    m_pBody->Reserve_Animation(L"talk", true);

    CCamera_Base* pCameraBase = dynamic_cast<CCamera_Base*>(m_pGameInstance->Get_MainCamera());


    pCameraBase->Set_Target(this);
    pCameraBase->Set_Offset(_float3(0.f, 3.f, -2.f));

}

void CNPC_Richard::On_Interaction(_float fTimeDelta)
{
   
    m_fTime += fTimeDelta;

    m_pPlayer->Get_Transform()->LookAtSmooth(m_pTransformCom->Get_State(STATE::POSITION), 5.f, fTimeDelta);

    if (m_fTime >= 3.f)
    {
        Exit_Interaction();
        m_fTime = 0.f;
    }
}

void CNPC_Richard::Exit_Interaction()
{
    m_bTalking = false;
    m_pPlayer->Get_ActionControl()->m_bTalk = false;
    m_pBody->Reserve_Animation(L"wait", true);
    CCamera_Base* pCameraBase = dynamic_cast<CCamera_Base*>(m_pGameInstance->Get_MainCamera());


    pCameraBase->Set_Target(m_pPlayer);
    pCameraBase->Set_Offset(pCameraBase->Get_InitOffset());
}

CNPC_Richard* CNPC_Richard::Create(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pDeviceContex, void* pArg)
{
    CNPC_Richard* pInstance = new CNPC_Richard(_pDevice, _pDeviceContex);
    if (FAILED(pInstance->Initialize_Prototype(pArg)))
    {
        MSG_BOX("Failed to Create :CNPC_Richard ");
        Safe_Release(pInstance);

    }

    return pInstance;
}

void CNPC_Richard::Free()
{
    __super::Free();
}
