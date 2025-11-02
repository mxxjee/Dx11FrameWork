#pragma once
#include "CBase.h"

NS_BEGIN(Engine)

class CMapObject;

class ENGINE_DLL CMapLayer :
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
    CMapObject* Find_GameObject(const _wstring& Tag);

    CMapObject*        Check_Picking(_vector Origin, _vector Dir, float& Dist);

    
public:
    void        Set_AblePicking(bool b) { m_bAblePicking = b; }
private:
    list<CMapObject*>      m_ObjList;

public:
    static CMapLayer* Create();
    virtual void Free() override;

private:
    bool            m_bAblePicking = true;
};

NS_END