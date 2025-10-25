#include "CMapObject_Manager.h"
#include "CMapLayer.h"
#include "CMapObject.h"
#include "CGameInstance.h"
#include "MathUtils.h"
#include "CInput_Manager.h"


IMPLEMENT_SINGLETON(CMapObject_Manager)

CMapObject_Manager::CMapObject_Manager()
{

}
CMapObject_Manager::CMapObject_Manager(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pContext)
    :m_pDevice{ _pDevice },
    m_pContext{ _pContext },
    m_pGameInstance{ CGameInstance::GetInstance() }
{
    Safe_AddRef(m_pGameInstance);
}

HRESULT CMapObject_Manager::Initialize(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pContext)
{
    m_pDevice = _pDevice;
    m_pContext = _pContext;
    m_pGameInstance = CGameInstance::GetInstance();
    Safe_AddRef(m_pGameInstance);

    m_EngineDesc = CGameInstance::GetInstance()->Get_EngineDesc();

    return S_OK;
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
    if(CInput_Manager::GetInstance()->IsMouseButtonPressed(0))
        Check_Picking();

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

void CMapObject_Manager::Check_Picking()
{
    CheckTrue(m_Layers.empty());

    //레이를 생성한다.
        //freecam의 view/proj가져오기
    _float4x4 Proj = CGameInstance::GetInstance()->Get_ProjMatrix(ENUM_TO_UINT(CAMERA_TYPE::FREE));
    _float4x4 View = CGameInstance::GetInstance()->Get_ViewMatrix(ENUM_TO_UINT(CAMERA_TYPE::FREE));

    float Dist = 0.f;
    CMapObject* pObj = nullptr;

    Ray ray = MathUtils::CreateRayWorld(m_EngineDesc.hWnd, m_pContext, Proj, View);
    for (auto& pair : m_Layers)
    {
        if (pair.second)
        {
            pObj = pair.second->Check_Picking(ray.Origin, ray.Dir, Dist);
            if (pObj)
            {
                Set_SelectObject(pObj);
                return;
            }

        }
    }
}

HRESULT CMapObject_Manager::Add_MapObject_To_MapLayer(_uint iProtoLevelIndex, const _wstring& strPrototypeTag, const _wstring& strLayerTag, void* pArg)
{
    CGameObject* pCloneObj = dynamic_cast<CGameObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, iProtoLevelIndex, strPrototypeTag, pArg));
    CheckNullResult(pCloneObj, E_FAIL);

    CMapLayer* pLayer = Find_MapLayer(strLayerTag);
    if (!pLayer)
    {
        pLayer = CMapLayer::Create();
        pLayer->Add_GameObject(dynamic_cast<CMapObject*>(pCloneObj));
        m_Layers.emplace(strLayerTag, pLayer);

    }

    else
        pLayer->Add_GameObject(dynamic_cast<CMapObject*>(pCloneObj));

    return S_OK;
}

HRESULT CMapObject_Manager::Add_MapObject_To_MapLayer(const _wstring& LayerTag, CMapObject* pObj)
{
    CMapLayer* pLayer = Find_MapLayer(LayerTag);
    if (!pLayer)
    {
        pLayer = CMapLayer::Create();
        pLayer->Add_GameObject(pObj);
        m_Layers.emplace(LayerTag, pLayer);

    }

    else
        pLayer->Add_GameObject(pObj);

    return S_OK;
}

HRESULT CMapObject_Manager::Add_Model_To_MapLayer(void* pArg)
{
    CMapLayer* pLayer = Find_MapLayer(L"Model_Layer");

    CGameObject* pCloneObj = dynamic_cast<CGameObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, 0, PROTO_OBJ_NAME(L"MapQuad"), pArg));
    CheckNullResult(pCloneObj, E_FAIL);

    CMapObject* pObj = dynamic_cast<CMapObject*>(pCloneObj);
    CheckNullResult(pObj, E_FAIL);

    if (!pLayer)
    {
        pLayer = CMapLayer::Create();
        pLayer->Add_GameObject(pObj);
        m_Layers.emplace(L"Model_Layer", pLayer);

    }

    else
        pLayer->Add_GameObject(pObj);

    return S_OK;
}

CMapObject* CMapObject_Manager::Find_MapObject(const _wstring& LayerTag, const _wstring& ObjTag)
{
    CheckTrueResult(m_Layers.empty(), nullptr);
    CMapLayer* pLayer = Find_MapLayer(LayerTag);
    if (pLayer)
    {
        return dynamic_cast<CMapObject*>(pLayer->Find_GameObject(ObjTag));
    }

    return nullptr;
}

void CMapObject_Manager::Clear(const _wstring& LayerTag)
{
    CheckTrue(m_Layers.empty());
    CMapLayer* pLayer = Find_MapLayer(LayerTag);

    Safe_Release(pLayer);
    m_Layers.erase(LayerTag);
}

CMapLayer* CMapObject_Manager::Find_MapLayer(const _wstring& LayerTag)
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
    pInstance->m_EngineDesc = CGameInstance::GetInstance()->Get_EngineDesc();


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
