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

CMapObject* CMapLayer::Find_GameObject(const wstring& Tag)
{
    for (auto& i : m_ObjList)
    {
        if (i->Get_Tag() == Tag)
            return i;

    }
    return nullptr;
}

CMapObject* CMapLayer::Check_Picking(_vector Origin, _vector Dir, float& Dist)
{
    CheckTrueResult(m_ObjList.empty(),nullptr);
    CheckFalseResult(m_bAblePicking, nullptr);

    float fMinDist = FLT_MAX;
    CMapObject* pPickedObj = nullptr;

	for (auto& obj : m_ObjList)
	{
        float fDist = 0.f;
        if (obj->Is_Picked(Origin, Dir, fDist))
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
