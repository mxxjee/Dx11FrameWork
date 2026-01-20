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
    HRESULT         Ready_DepthStencilView();
    HRESULT         SetUp_ViewportDesc(_uint iWidth, _uint iHeight);

    _uint           Get_ViewportWidth() {return m_iViewportWidth;}
    _uint           Get_ViewportHeight() { return m_iViewportHeight; }

public:
    void        Bind_And_Render_Lights();
    void        Bind_Rect_Matricies();
    void        Render_Combined();
    ComPtr<ID3D11DepthStencilView>      Get_DSV() { return m_pDSV; }
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
    ComPtr<ID3D11DepthStencilView>  m_pDSV = { nullptr };
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

    _uint					m_iViewportWidth = { }, m_iViewportHeight = {};

};

NS_END