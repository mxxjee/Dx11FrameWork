#pragma once
#include "CBase.h"

NS_BEGIN(Engine)
class CGameObject;
class CVIBuffer_Rect;


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


public:
    void        Bind_And_Render_Lights();
    void        Bind_Rect_Matricies();
    void        Render_Combined();

    virtual void    Free() override;

#ifdef  _DEBUG
    void        Render_Debug();

#endif //  _DEBUG

private:
    int             m_RenderMaxCount = 0;

    vector<list<CGameObject*>>   m_RenderGroups;
    vector<function<bool(class CGameObject*, class CGameObject*)>>      m_SortFuncTable;

private:
    _float4x4 m_WorldMatrix, m_ViewMatrix, m_ProjMatrix;
    CVIBuffer_Rect* m_pVIBuffer = { nullptr };
    class CShader* m_pShader = { nullptr };

private:
    class CGameInstance* m_pGameInstance = { nullptr };


    //디버그용(콜라이더,혹은네비등
#ifdef _DEBUG
    list<class CComponent*>         m_DebugComponents;
public:
    HRESULT         Add_DebugComponent(CComponent* pComponent);
#endif

private:
    ENGINE_DESC m_EngineDesc;
};

NS_END