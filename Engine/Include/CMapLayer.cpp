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

CMapObject* CMapLayer::Find_GameObject(const _wstring& Tag)
{
    for (auto& i : m_ObjList)
    {
        if (i->Get_Tag() == Tag)
            return i;

    }
    return nullptr;
}

bool CMapLayer::Check_Picking(HWND hwnd, _float4x4& Proj, _float4x4& View, float& Dist)
{
    CheckTrueResult(m_ObjList.empty(),false);
    CheckFalseResult(m_bAblePicking,false);

	for (auto& obj : m_ObjList)
	{
        if (obj->Is_Picked(hwnd, Proj, View, Dist))
        {
            OutputDebugString((L"Dist: " + std::to_wstring(Dist) + L"/ name : " + obj->Get_Tag() + L"\n").c_str());
            return true;
        }

        else
            continue;

	}
   
    return false;
}



CMapLayer* CMapLayer::Create(bool bAblePicking)
{
   CMapLayer* pLayer =new CMapLayer();
   pLayer->m_bAblePicking = bAblePicking;

   return pLayer;
}

void CMapLayer::Free()
{
    __super::Free();

    for (auto& i : m_ObjList)
        Safe_Release(i);

    m_ObjList.clear();
}
