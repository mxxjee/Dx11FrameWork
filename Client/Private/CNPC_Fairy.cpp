#include "CNPC_Fairy.h"
#include "CPlayer.h"


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


    m_pGameInstance->RegisterListners("Enter_Forest", [this](const GameEvent& event)
        {
            Set_Active(true);
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
