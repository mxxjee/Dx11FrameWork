#include "CMapObject_Manager.h"
#include "CMapLayer.h"
#include "CMapObject.h"
#include "CGameInstance.h"
#include "MathUtils.h"
#include "CInput_Manager.h"
#include "IMapEditable.h"
#include "CInput_Manager.h"
#include "CTerrain_Base.h"
#include "CImGui_Manager.h"

///////////////
#include "CMapInteractObject.h"
#include "CBounding_Mesh.h"
#include "CModel.h"
#include "CMeshColliderComponent.h"


IMPLEMENT_SINGLETON(CMapObject_Manager)

CMapObject_Manager::CMapObject_Manager()
{

}
CMapObject_Manager::CMapObject_Manager(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pContext)
    :m_pDevice{ _pDevice },
    m_pContext{ _pContext },
    m_pGameInstance{ CGameInstance::GetInstance() },
    m_pInputManager{ CInput_Manager::GetInstance()}
{
    Safe_AddRef(m_pInputManager);
  
    Safe_AddRef(m_pGameInstance);
}

HRESULT CMapObject_Manager::Initialize(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pContext)
{
    m_pDevice = _pDevice;
    m_pContext = _pContext;
    m_pGameInstance = CGameInstance::GetInstance();
    m_pInputManager = CInput_Manager::GetInstance();
    m_pImguiManager = CImGui_Manager::GetInstance();

    Safe_AddRef(m_pInputManager);
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

    if (!m_bAblePicking)
        Set_AblePicking(true);

    if (CInput_Manager::GetInstance()->IsMouseButtonPressed(0))
        Check_Picking();

    if (m_pSelectedObject)
        m_pSelectedObject->Update_SelectMode(fTimeDelta);
        
    
    //Active_SelectionMode(fTimeDelta);



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

void CMapObject_Manager::Check_Picking()
{
    CheckTrue(m_Layers.empty());
    CheckFalse(m_bAblePicking);

    //레이를 생성한다.
        //freecam의 view/proj가져오기
    _float4x4 Proj = CGameInstance::GetInstance()->Get_ProjMatrix(ENUM_TO_UINT(CAMERA_TYPE::FREE));
    _float4x4 View = CGameInstance::GetInstance()->Get_ViewMatrix(ENUM_TO_UINT(CAMERA_TYPE::FREE));

    float Dist = 0.f;
    float MinDist = FLT_MAX;

    CMapObject* pObj = nullptr;
    CMapObject* pPickObj = nullptr;

   
    for (auto& pair : m_Layers)
    {
        if (pair.second)
        {
            float fDist = 0;
            pObj = pair.second->Check_Picking(m_EngineDesc.hWnd,m_pContext,Proj,View, fDist);
            
            if (pObj)
            {
                if (fDist < MinDist)
                {
                    MinDist = fDist;
                    pPickObj = pObj;
                }

            }

       }
    }

    if (pPickObj)
    {
        Set_SelectObject(pPickObj);
        Set_AblePicking(false);

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

CBounding_Mesh::BOUNDING_MESH_DESC  CMapObject_Manager::Generate_Collider_By_InteractionType(_uint i)
{
   
    CBounding_Mesh::BOUNDING_MESH_DESC MeshDesc;
    MeshDesc.vCenter = _float3(0.f, 0.f, 0.f);

    switch ((CMapInteractObject::InteractionType)i)
    {
    case CMapInteractObject::InteractionType::ROCK:
        MeshDesc.vCenter = _float3(0.f, 1.f, 0.f);
        MeshDesc.Extents = _float3(0.7f, 0.7f, 0.7f);
        break;


    case CMapInteractObject::InteractionType::LAWN:
        MeshDesc.Extents = _float3(0.5f, 0.5f, 0.5f);
        break;


    case CMapInteractObject::InteractionType::GRASS:
        MeshDesc.Extents = _float3(0.5f, 0.5f, 0.5f);
        break;


    default:
        break;
    }

    return MeshDesc;

   
}

//
//void CMapObject_Manager::Set_SelectObject(CMapObject* pObj)
//{
//    if (m_pSelectObject)
//        m_pSelectObject->Set_Select(false);
//
//    m_pSelectObject = pObj;
//    pObj->Set_Select(true);
//
//}

HRESULT CMapObject_Manager::Save_Data(const wstring& Path)
{
    //json root;
    ////////////
    //json objectArray = json::array();
    //for (auto& pair : m_Layers)
    //{
    //    for (pair.second)
    //    {

    //    }
    //}
    return S_OK;
}

CMapLayer* CMapObject_Manager::Get_Layer_By_MapObjType(MapObjType eType)
{
    wstring LayerTag = L"";

    switch (eType)
    {
    case MapObjType::OBSTACLE:
        LayerTag = L"Obstacle_Layer";
        break;

    case MapObjType::TILE:
        LayerTag = L"Tile_Layer";
        break;


    case MapObjType::POSITION:
        LayerTag = L"Position_Layer";
        break;


    case MapObjType::TRIGGER:
        LayerTag = L"Trigger_Layer";
        break;


    case MapObjType::INTERACTION:
        LayerTag = L"Interaction_Layer";
        break;
    default:
        break;
    }

    CMapLayer* pLayer = Find_MapLayer(LayerTag);
    if (!pLayer)
    {
        pLayer = CMapLayer::Create();
        m_Layers.emplace(LayerTag, pLayer);

       
    }
    
    return pLayer;

}

HRESULT CMapObject_Manager::Save_InteractionData(const string& filePath, _uint iNum)
{
    CMapLayer* pLayer = Find_MapLayer(L"Interaction_Layer");
    if (pLayer)
        pLayer->Save_Data(filePath, iNum);

    return S_OK;
}

HRESULT CMapObject_Manager::Load_InteractionData(const string& filePath, vector<DefaultInteractionData>& Datas)
{
    m_pSelectedObject = nullptr;

    CMapLayer* pLayer = Find_MapLayer(L"Interaction_Layer");
    if (!pLayer)
    {
        pLayer = CMapLayer::Create();
        m_Layers.emplace(L"Interaction_Layer", pLayer);

    }
    
    pLayer->Load_Data(filePath,Datas);

    return S_OK;
}



wstring CMapObject_Manager::Generate_UniqueTag(MapObjType Type, const wstring& baseName)
{
    int iIdx = 0;
    _wstring Result;

    if (Type != MapObjType::TERRAIN)
    {
        CMapLayer* pMapLayer = Get_Layer_By_MapObjType(Type);
        if (pMapLayer)
        {
            while (true)
            {
                Result = baseName + L"_" + to_wstring(iIdx);
                if (pMapLayer->Find_GameObject(Result) == nullptr)
                    break;


                ++iIdx;
            }
        }

        else
            return baseName + L"_" + to_wstring(iIdx);

    }

    else
    {
        while (true)
        {
            Result = baseName + L"_" + to_wstring(iIdx);
            if (m_pGameInstance->Find_Terrain(Result) == nullptr)
                break;


            ++iIdx;
        }



    }

    return Result;
}

void CMapObject_Manager::Show_SelectObject_Gizmo()
{
    if (m_pSelectedObject)
        m_pSelectedObject->Show_Gizmo();
}

void CMapObject_Manager::Set_SelectObject(IMapEditable* pSelectedObject)
{
    
 

    //기존껏은 해제한다.
    if (m_pSelectedObject)
        m_pSelectedObject->OnSeletected(false);



    /*교체*/
    m_pSelectedObject = pSelectedObject;

    CheckNull(m_pSelectedObject);
    m_pSelectedObject->OnSeletected(true);

        
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
        if(pair.second)
		    Safe_Release(pair.second);
	}

    Safe_Release(m_pInputManager);
    Safe_Release(m_pGameInstance);
}
