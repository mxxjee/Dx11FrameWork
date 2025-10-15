#pragma once
#include "CBase.h"

NS_BEGIN(Engine)
class CGameObject;


class CRenderer :
    public CBase
{
private:
    CRenderer(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    virtual ~CRenderer() = default;



public:
    HRESULT         Initialize(_uint RenderGroupCount);
    HRESULT         Add_RenderObject(_uint eID, CGameObject* pRenderObject);
    HRESULT         Add_SortFunc(_uint eID, function<bool(class CGameObject*, class CGameObject*)> _Fun);
    void            Draw();

public:
    void        Render_Group(_uint eType);

private:
    void       BindRenderState(_uint eGroup);
    void       SortByDepth(_uint eGroup);
    void       RenderGroupObjects(_uint eGroup);

public:
    int         Get_RenderGroupCount()              { return m_RenderMaxCount; }
public:
    void        Clear_RenderGroups();

    
private:
    ComPtr<ID3D11Device> m_pDevice = nullptr;
    ComPtr<ID3D11DeviceContext> m_pContext = nullptr;
    

public:
    static  CRenderer* Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    virtual void    Free() override;

private:
    _matrix         m_MainCameraView;
    int             m_RenderMaxCount = 0;

    vector<list<CGameObject*>>   m_RenderGroups;
    vector<function<bool(class CGameObject*,class CGameObject*)>>      m_SortFuncMap;
};

NS_END