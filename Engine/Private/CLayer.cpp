#include "CLayer.h"
#include "CGameObject.h"
#include "MathUtils.h"


CLayer::CLayer()
{
}

HRESULT CLayer::Add_GameObject(CGameObject* pGameObject)
{
    m_ObjList.push_back(pGameObject);
    return S_OK;
}

void CLayer::Update_Priority(_float fTimeDelta)
{
    for (auto& i : m_ObjList)
    {
        if (i != nullptr)
        {
            if(i->Is_Active())
                i->Update_Priority(fTimeDelta);
        }
            
    }
}

void CLayer::Update(_float fTimeDelta)
{
    for (auto& i : m_ObjList)
    {
        if (i != nullptr)
        {
            if (i->Is_Active())
                i->Update(fTimeDelta);
        }
            
    }
}

void CLayer::Update_Late(_float fTimeDelta)
{
    for (auto& i : m_ObjList)
    {
        if (i != nullptr)
        {
            if (i->Is_Active())
                i->Update_Late(fTimeDelta);
        }
            
    }

    ProcessDestory();
}

void CLayer::Update_Render(_float fTimeDelta)
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

CGameObject* CLayer::Find_GameObject(const wstring& Tag)
{
    for (auto& i : m_ObjList)
    {
        if (i->Get_Tag() == Tag)
            return i;

    }
    return nullptr;
}
void CLayer::RequestDestroy(CGameObject* pObj)
{
    pObj->Set_Active(false);
    m_DestroyQueue.push(pObj);
}
void CLayer::ProcessDestory()
{
    while (!m_DestroyQueue.empty())
    {
        CGameObject* pObj = m_DestroyQueue.front();
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
//
//bool CLayer::Check_Picking(HWND hWnd, ComPtr<ID3D11DeviceContext> m_pContext, _float4x4& Proj, _float4x4& View)
//{
//    for (auto& i : m_ObjList)
//    {
//        _float4x4 Proj = CGameInstance::GetInstance()->Get_ProjMatrix(ENUM_TO_UINT(CAMERA_TYPE::FREE));
//        _float4x4 View = CGameInstance::GetInstance()->Get_ViewMatrix(ENUM_TO_UINT(CAMERA_TYPE::FREE));
//
//
//        Ray ray = MathUtils::CreateRay(hWnd, m_pContext, i, Proj, View);
//
//    }
//    return false;
//}

CLayer* CLayer::Create()
{
    return new CLayer();
}

void CLayer::Free()
{
    __super::Free();

    for (auto& i : m_ObjList)
        Safe_Release(i);

    m_ObjList.clear();
}
