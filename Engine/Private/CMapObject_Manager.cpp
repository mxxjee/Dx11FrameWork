#include "CMapObject_Manager.h"
#include "CLayer.h"
#include "CMapObject.h"
#include "CGameInstance.h"

CMapObject_Manager::CMapObject_Manager(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pContext)
    :m_pDevice{ _pDevice },
    m_pContext{ _pContext },
    m_pGameInstance{ CGameInstance::GetInstance() }
{
    Safe_AddRef(m_pGameInstance);
}

void CMapObject_Manager::Update_Priority(_float fTimeDelta)
{
    for (auto& pair : m_Layers)
    {
        pair.second->Update_Priority(fTimeDelta);
    }
}

void CMapObject_Manager::Update(_float fTimeDelta)
{
    for (auto& pair : m_Layers)
    {
        pair.second->Update(fTimeDelta);
    }
}

void CMapObject_Manager::Update_Late(_float fTimeDelta)
{
    for (auto& pair : m_Layers)
    {
        pair.second->Update_Late(fTimeDelta);
    }
}

void CMapObject_Manager::Update_Render(_float fTimeDelta)
{

    for (auto& pair : m_Layers)
    {
        pair.second->Update_Render(fTimeDelta);
    }
}

HRESULT CMapObject_Manager::Add_MapObject_To_Layer(_uint iProtoLevelIndex, const _wstring& strPrototypeTag, const _wstring& strLayerTag, void* pArg)
{
    CGameObject* pCloneObj = dynamic_cast<CGameObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, iProtoLevelIndex, strPrototypeTag, pArg));
    CheckNullResult(pCloneObj, E_FAIL);

    CLayer* pLayer = Find_Layer(strLayerTag);
    if (!pLayer)
    {
        pLayer = CLayer::Create();
        pLayer->Add_GameObject(pCloneObj);
        m_Layers.emplace(strLayerTag, pLayer);

    }

    else
        pLayer->Add_GameObject(pCloneObj);

    return S_OK;
}

HRESULT CMapObject_Manager::Add_MapObject_To_Layer(const _wstring& LayerTag, CMapObject* pObj)
{
    CLayer* pLayer = Find_Layer(LayerTag);
    if (!pLayer)
    {
        pLayer = CLayer::Create();
        pLayer->Add_GameObject(pObj);
        m_Layers.emplace(LayerTag, pLayer);

    }

    else
        pLayer->Add_GameObject(pObj);

    return S_OK;
}

CMapObject* CMapObject_Manager::Find_MapObject(const _wstring& LayerTag, const _wstring& ObjTag)
{
    CheckTrueResult(m_Layers.empty(), nullptr);
    CLayer* pLayer = Find_Layer(LayerTag);
    if (pLayer)
    {
        return dynamic_cast<CMapObject*>(pLayer->Find_GameObject(ObjTag));
    }

    return nullptr;
}

void CMapObject_Manager::Clear(const _wstring& LayerTag)
{
    CheckTrue(m_Layers.empty());
    CLayer* pLayer = Find_Layer(LayerTag);

    Safe_Release(pLayer);
    m_Layers.erase(LayerTag);
}

CLayer* CMapObject_Manager::Find_Layer(const _wstring& LayerTag)
{
    auto iter = m_Layers.find(LayerTag);
    if (iter != m_Layers.end())
        return iter->second;

    return nullptr;
}

void CMapObject_Manager::Set_SelectObject(CMapObject* pObj)
{
    if (m_pSelectObject)
        m_pSelectObject->Set_Select(false); 
    
    m_pSelectObject = pObj;
    pObj->Set_Select(true);

}

CMapObject_Manager* CMapObject_Manager::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
    CMapObject_Manager* pInstance = new CMapObject_Manager(pDevice, pContext);
    return pInstance;
}

void CMapObject_Manager::Free()
{
    __super::Free();
	for (auto& pair : m_Layers)
	{
		Safe_Release(pair.second);
	}

	

    Safe_Release(m_pGameInstance);
}
