#include "CContainerObject.h"
#include "CPartObject.h"
#include "CGameInstance.h"


CContainerObject::CContainerObject(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    :CGameObject(pDevice,pContext)
{
}

CContainerObject::CContainerObject(const CContainerObject& rhs)
    :CGameObject(rhs)
{
}

HRESULT CContainerObject::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CContainerObject::Initialize_Copytype(void* pArg)
{
    if(FAILED(__super::Initialize_Copytype(pArg)))
        return E_FAIL;

    return S_OK;
}

void CContainerObject::Update_Priority(_float fTimeDelta)
{
    for (auto& pair : m_PartObjects)
    {
        if (pair.second)
        {
            if(pair.second->Is_Active())
                pair.second->Update_Priority(fTimeDelta);
        
        }
           
    }
        
}

void CContainerObject::Update(_float fTimeDelta)
{
    __super::Update(fTimeDelta);

    for (auto& pair : m_PartObjects)
    {
        if (pair.second)
        {
            if (pair.second->Is_Active())
                pair.second->Update(fTimeDelta);
        }
            
    }
}

void CContainerObject::Update_Late(_float fTimeDelta)
{

    __super::Update_Late(fTimeDelta);

    for (auto& pair : m_PartObjects)
    {
        if (pair.second)
        {
            if (pair.second->Is_Active())
                pair.second->Update_Late(fTimeDelta);
        }
            
    }
}

void CContainerObject::Update_Render(_float fTimeDelta)
{
    for (auto& pair : m_PartObjects)
    {
        if (pair.second)
        {
            if (pair.second->Is_Active())
                pair.second->Update_Render(fTimeDelta);
        }
            
    }
}

HRESULT CContainerObject::Render()
{
    for (auto& pair : m_PartObjects)
    {
        if (pair.second)
            pair.second->Render();
    }
    return S_OK;
}

void CContainerObject::Set_Active(bool _b)
{
    __super::Set_Active(_b);

    for (auto& pair : m_PartObjects)
    {
        if (pair.second)
            pair.second->Set_Active(_b);
    }
}

HRESULT CContainerObject::Add_PartObject(_uint iPrototypeLevelIdx, const _wstring& strPrototypeTag, const _wstring& strPartObjTag, void* pArg)
{
    /*이미존재하는지 체크*/
    if (Find_PartObject(strPartObjTag) != nullptr)
        return E_FAIL;


    CPartObject* pPartObject = dynamic_cast<CPartObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, iPrototypeLevelIdx, strPrototypeTag, pArg));
    CheckNullResult(pPartObject, E_FAIL);

    pPartObject->Set_Tag(strPartObjTag);
    pPartObject->Set_Owner(this);
    pPartObject->Set_SceneID(m_iSceneID);

    m_PartObjects.emplace(strPartObjTag, pPartObject);
    return S_OK;
}

CPartObject* CContainerObject::Find_PartObject(const _wstring& strTag)
{
    auto iter = m_PartObjects.find(strTag);
    if (iter == m_PartObjects.end())
        return nullptr;

    return iter->second;
}


void CContainerObject::Free()
{
    __super::Free();

    for (auto& Pair : m_PartObjects)
        Safe_Release(Pair.second);


    m_PartObjects.clear();
}
