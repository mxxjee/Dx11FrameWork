#include "CGameObject.h"

CGameObject::CGameObject(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    :m_pDevice{pDevice},m_pDeviceContext{pContext}
{

}

CGameObject::CGameObject(const CGameObject& rhs)
{
}


HRESULT CGameObject::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CGameObject::Initialize_Copytype(void* pArg)
{
    return S_OK;
}

void CGameObject::Update_Priority(_float fTimeDelta)
{
}

void CGameObject::Update(_float fTimeDelta)
{
}

void CGameObject::Update_Late(_float fTimeDelta)
{
}

void CGameObject::Render()
{
}



void CGameObject::Free()
{
    __super::Free();
}
