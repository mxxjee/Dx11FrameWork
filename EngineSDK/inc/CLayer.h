#pragma once
#include "CBase.h"

NS_BEGIN(Engine)

class CGameObject;

class ENGINE_DLL CLayer final:
    public CBase
{
private:
    CLayer();
    virtual ~CLayer() = default;

public:
    HRESULT     Add_GameObject(CGameObject* pGameObject);
    void Update_Priority(_float fTimeDelta);
    void Update(_float fTimeDelta);
    void Update_Late(_float fTimeDelta);
    void Update_Render(_float fTimeDelta);

    const list<CGameObject*>& Get_ObjList() {return m_ObjList; }
    CGameObject* Find_GameObject(const wstring& Tag);

public:
    void        Clear();
public:
    void            RequestDestroy(CGameObject* pObj);
    void            ProcessDestory();
private:
    list<CGameObject*>      m_ObjList;
    queue<CGameObject*>      m_DestroyQueue;         //지연삭제 큐 
public:
    static CLayer* Create();
    virtual void Free() override;
};
NS_END

