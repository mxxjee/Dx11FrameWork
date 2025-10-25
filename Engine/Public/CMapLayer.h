#pragma once
#include "CBase.h"

NS_BEGIN(Engine)

class CMapObject;

class CMapLayer :
    public CBase
{
private:
    CMapLayer();
    virtual ~CMapLayer() = default;

public:
    HRESULT     Add_GameObject(CMapObject* pGameObject);
    void Update_Priority(_float fTimeDelta);
    void Update(_float fTimeDelta);
    void Update_Late(_float fTimeDelta);
    void Update_Render(_float fTimeDelta);

    const list<CMapObject*>& Get_ObjList() { return m_ObjList; }
    CMapObject* Find_GameObject(const _wstring & Tag);

    bool        Check_Picking(HWND hwnd, _float4x4& Proj, _float4x4& View,float& Dist);
private:
    list<CMapObject*>      m_ObjList;

public:
    static CMapLayer* Create();
    virtual void Free() override;
};

NS_END