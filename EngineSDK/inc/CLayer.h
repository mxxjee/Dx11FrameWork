#pragma once
#include "CBase.h"

NS_BEGIN(Engine)

class CGameObject;

class CLayer final:
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
    CGameObject* Find_GameObject(const _wstring& Tag);

    bool            Check_Picking(HWND hWnd, ComPtr<ID3D11DeviceContext> m_pContext,_float4x4& Proj, _float4x4& View);

private:
    list<CGameObject*>      m_ObjList;

public:
    static CLayer* Create();
    virtual void Free() override;
};
NS_END

