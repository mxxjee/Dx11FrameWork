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
    m_pGameInstance(CGameInstance::GetInstance()),
    m_pTransformCom(nullptr)
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
   
    if (FAILED(CGameObject::Ready_Components(pArg)))
        return E_FAIL;


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

HRESULT CGameObject::Ready_Components(void* pArg)
{
    CComponent* pTransform = dynamic_cast<CTransform*>(m_pGameInstance->Clone_Prototype
    (PROTOTYPE::COMPONENT, 0, PROTO_COMPONENT_NAME(L"Transform"), pArg));

    if (FAILED(Add_Component(COMPONENT_TYPE::TRANSFORM, pTransform, reinterpret_cast<CComponent**>(&m_pTransformCom))))
        return E_FAIL;

    return S_OK;
}

HRESULT CGameObject::Add_Component(COMPONENT_TYPE eType, CComponent* pComp, CComponent** pOut)
{
    auto iter = m_Components.find(eType);
    if (iter != m_Components.end())
        return E_FAIL;

    m_Components.emplace(eType, pComp);
    *pOut = pComp;

    Safe_AddRef(pComp);

    return S_OK;
}

CComponent* CGameObject::Get_Component(COMPONENT_TYPE eType)
{
    auto iter = m_Components.find(eType);
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
