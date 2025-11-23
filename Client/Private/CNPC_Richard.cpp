#include "CNPC_Richard.h"
#include    "CPlayer.h"
#include "CBody.h"



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
