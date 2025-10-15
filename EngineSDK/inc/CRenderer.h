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
    HRESULT         Initialize();
    HRESULT         Add_RenderObject(RENDERGROUP eID, CGameObject* pRenderObject);
    void            Draw();

public:
    void        Render_Group(RENDERGROUP eType);
public:
    void        Render_Priority();
    void        Render_NonBlend();
    void        Render_Blend();
    void        Render_UI();
    

public:
    void        Clear_RenderGroups();

private:
    void    CreateSamplerStates();
    void    CreateBlendStates();
    void    CreateRasterizerStates();
    void    CreateDepthStencilStates();
    
private:
    ComPtr<ID3D11Device> m_pDevice = nullptr;
    ComPtr<ID3D11DeviceContext> m_pContext = nullptr;
    

public:
    static  CRenderer* Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    list<class CGameObject*>        m_RenderObjects[ENUM_TO_UINT(RENDERGROUP::END)];
    virtual void    Free() override;

private:
    RenderStates m_RenderStates[ENUM_TO_UINT(RENDERGROUP::END)];
    _matrix       m_MainCameraView;
};

NS_END