#include "CMapLayer.h"
#include "CMapObject.h"
#include "MathUtils.h"

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

CMapObject* CMapLayer::Check_Picking(HWND g_hWnd, ComPtr<ID3D11DeviceContext> Context, _float4x4& Proj, _float4x4& View, float& Dist)
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
