#include "CGameObject.h"

CGameObject::CGameObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :m_pDevice(pDevice),m_pContext(pContext)
{
}

CGameObject::CGameObject(const CGameObject* rhs)
    :m_pDevice(rhs->m_pDevice),
    m_pContext(rhs->m_pContext)
{
}

HRESULT CGameObject::Initialize_Prototype()
{

    return S_OK;
}

HRESULT CGameObject::Initialize_Copytype(void* pArg)
{
    GAMEOBJECT_DESC* pDesc = static_cast<GAMEOBJECT_DESC*>(pArg);
    tag = pDesc->ObjTag;


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

HRESULT CGameObject::Render()
{
    return S_OK;
}


void CGameObject::Free()
{
    __super::Free();
}
