#include "CGameObject.h"
#include "CGameInstance.h"
#include "CComponent.h"

CGameObject::CGameObject(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    :m_pDevice(pDevice),
    m_pContext(pContext),
    m_pGameInstance(CGameInstance::GetInstance())
{
    Safe_AddRef(m_pGameInstance);
}

CGameObject::CGameObject(const CGameObject& rhs)
    :m_pDevice(rhs.m_pDevice),
    m_pContext(rhs.m_pContext),
    m_pGameInstance(CGameInstance::GetInstance())
{

    Safe_AddRef(m_pGameInstance);
}

HRESULT CGameObject::Initialize_Prototype()
{

    return S_OK;
}

HRESULT CGameObject::Initialize_Copytype(void* pArg)
{
    GAMEOBJECT_DESC* pDesc = static_cast<GAMEOBJECT_DESC*>(pArg);
    tag = pDesc->ObjTag;
    pDesc->pOwner = this;


    m_pTransformCom = dynamic_cast<CTransform*>(m_pGameInstance->Clone_Prototype
    (PROTOTYPE::COMPONENT, 0, PROTO_COMPONENT_NAME(L"Transform"), pDesc));

    CheckNullResult(m_pTransformCom, E_FAIL);

    Safe_AddRef(m_pTransformCom);
    m_Components.emplace(L"Transform", m_pTransformCom);

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
    if (m_pTransformCom)
        m_pTransformCom->Update_Matrix();
}

void CGameObject::Update_Render(_float fTimeDelta)
{
   
}

HRESULT CGameObject::Render()
{
    return S_OK;
}

CComponent* CGameObject::Get_Component(const _wstring Tag)
{
    auto iter = m_Components.find(Tag);
    if (iter == m_Components.end())
        return nullptr;

    return iter->second;
}


void CGameObject::Free()
{
    __super::Free();
    Safe_Release(m_pTransformCom);

    for (auto& pair : m_Components)
        Safe_Release(pair.second);

    Safe_Release(m_pGameInstance);
}
