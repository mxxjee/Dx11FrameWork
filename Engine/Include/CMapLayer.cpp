#include "CMapLayer.h"
#include "CMapObject.h"
#include "MathUtils.h"
#include "CMapInteractObject.h"

CMapLayer::CMapLayer()
{
}

HRESULT CMapLayer::Add_GameObject(CMapObject* pGameObject)
{
    m_ObjList.push_back(pGameObject);
    return S_OK;
}

void CMapLayer::Update_Priority(_float fTimeDelta)
{
    for (auto& i : m_ObjList)
    {
        if (i != nullptr)
        {
            if (i->Is_Active())
                i->Update_Priority(fTimeDelta);
        }

    }
}

void CMapLayer::Update(_float fTimeDelta)
{
    for (auto & i : m_ObjList)
    {
        if (i != nullptr)
        {
            if (i->Is_Active())
                i->Update(fTimeDelta);
        }

    }
}

void CMapLayer::Update_Late(_float fTimeDelta)
{

    for (auto& i : m_ObjList)
    {
        if (i != nullptr)
        {
            if (i->Is_Active())
                i->Update_Late(fTimeDelta);
        }

    }

    ProcessDestroy();
}

void CMapLayer::Update_Render(_float fTimeDelta)
{
    for (auto& i : m_ObjList)
    {
        if (i != nullptr) 
        {
            if (i->Is_Active())
                i->Update_Render(fTimeDelta);
        }

    }
}

CMapObject* CMapLayer::Find_GameObject(const _wstring& Tag)
{
    for (auto& i : m_ObjList)
    {
        if (i->Get_Tag() == Tag)
            return i;

    }
    return nullptr;
}

CMapObject* CMapLayer::Find_GameObject(_uint iIdx)
{
    _uint TargetIdx = 0;

    for (auto& i : m_ObjList)
    {
        if (TargetIdx==iIdx)
            return i;

        ++TargetIdx;
    }
    return nullptr;
}

CMapObject* CMapLayer::Check_Picking(_float3& vWorld,  HWND g_hWnd, ComPtr<ID3D11DeviceContext> Context, _float4x4& Proj, _float4x4& View, float& Dist)
{
    CheckTrueResult(m_ObjList.empty(),nullptr);
    CheckFalseResult(m_bAblePicking, nullptr);

    
    float fMinDist = FLT_MAX;
    CMapObject* pPickedObj = nullptr;

	for (auto& obj : m_ObjList)
	{
        Ray ray = MathUtils::CreateRayWorld(g_hWnd, Context, Proj, View);
        float fDist = 0.f;
        if (obj->Is_Picked(ray.Origin, ray.Dir, fDist))
        {
            if (fDist < fMinDist)
            {
                fMinDist = fDist;
                pPickedObj = obj;
                XMStoreFloat3(&vWorld, ray.Origin + ray.Dir * fDist);

            }
        }

        else
            continue;

	}

    //젤 작은거리의 픽킹오브젝트 반환
    Dist = fMinDist;
    return pPickedObj;
}

void CMapLayer::RequestDestroy(CMapObject* pObj)
{
    // m_DestroyQueue에 추가한다.
    pObj->Set_Active(false);
    pObj->OnDestroy();
    m_DestroyQueue.push(pObj);
}

void CMapLayer::ProcessDestroy()
{
    while (!m_DestroyQueue.empty())
    {
        CMapObject* pObj = m_DestroyQueue.front();
        m_DestroyQueue.pop();

        for (auto it = m_ObjList.begin(); it != m_ObjList.end();)
        {
            if (*it == pObj)
                it = m_ObjList.erase(it);

            else
                ++it;

        }

        Safe_Release(pObj);
    }
}

HRESULT CMapLayer::Save_Data(const string& filePath, _uint iNum)
{
    json jInteraction;

	for (auto& obj : m_ObjList)
	{
		if (obj)
		{
            obj->Save_To_Json(jInteraction);
		}
	}
    string FinalPath = "";
    FinalPath = filePath;

    ofstream	file(FinalPath);
    file << std::setw(4) << jInteraction;

    file.close();


    MSG_BOX("Save Complete");
    return S_OK;
}

HRESULT CMapLayer::Save_RoomData(_uint iRoomNum)
{
    json j;//각자 내부에서 알아서저장할거므로 그냥 던지기위한 값 
    int idx = 0;
    for (auto iter = m_ObjList.begin(); iter != m_ObjList.end(); ++iter)
    {
        
        (*iter)->Save_To_Json(j);
        if (idx == iRoomNum-1)
            break;
    }
    return S_OK;
}



HRESULT CMapLayer::Load_Data(const string& LoadPath, vector<DefaultInteractionData>& Infos)
{
    /*기존 리스트를 비우고 로드한다.*/
    for (auto& i : m_ObjList)
        Safe_Release(i);

    m_ObjList.clear();

    ifstream file(LoadPath);
    json jInteractionData = json::parse(file);
    for (auto& iInteraction : jInteractionData)
    {
        DefaultInteractionData Data;

        Data.ModelName= iInteraction["ModelName"];
        string InteractionType = iInteraction["InteractionType"];
        if (InteractionType == "CaveRock")
            Data.InteractionType = ENUM_TO_UINT(CMapInteractObject::InteractionType::CAVEROCK);

        else if (InteractionType == "Rock")
            Data.InteractionType = ENUM_TO_UINT(CMapInteractObject::InteractionType::ROCK);


        else if (InteractionType == "Lawn")
            Data.InteractionType = ENUM_TO_UINT(CMapInteractObject::InteractionType::LAWN);

        
        else if (InteractionType == "Grass")
            Data.InteractionType = ENUM_TO_UINT(CMapInteractObject::InteractionType::GRASS);

        json TransformData = iInteraction["Transform"];

        Data.vPos.x = TransformData["Position"][0].get<float>();
        Data.vPos.y = TransformData["Position"][1].get<float>();
        Data.vPos.z = TransformData["Position"][2].get<float>();

        Data.vRotation.x = TransformData["Rotation"][0].get<float>();
        Data.vRotation.y = TransformData["Rotation"][1].get<float>();
        Data.vRotation.z = TransformData["Rotation"][2].get<float>();

        Data.vScale.x = TransformData["Scale"][0].get<float>();
        Data.vScale.y = TransformData["Scale"][1].get<float>();
        Data.vScale.z = TransformData["Scale"][2].get<float>();


        json ColliderData = iInteraction["Collider"];
        
        Data.ColliderCenter.x = ColliderData["Center"][0].get<float>();
        Data.ColliderCenter.y = ColliderData["Center"][1].get<float>();
        Data.ColliderCenter.z = ColliderData["Center"][2].get<float>();

        Data.ColliderExtent.x = ColliderData["Extent"][0].get<float>();
        Data.ColliderExtent.y = ColliderData["Extent"][1].get<float>();
        Data.ColliderExtent.z = ColliderData["Extent"][2].get<float>();;

        Infos.push_back(Data);

    }

    
    return S_OK;
}




CMapLayer* CMapLayer::Create()
{
   
   return new CMapLayer;
}

void CMapLayer::Free()
{
    __super::Free();

    for (auto& i : m_ObjList)
        Safe_Release(i);

    m_ObjList.clear();
}
