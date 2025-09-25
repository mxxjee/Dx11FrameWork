#pragma once
#include "CBase.h"

NS_BEGIN(Engine)
class CGameObject;

class CRenderer :
    public CBase
{
private:
    CRenderer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual ~CRenderer() = default;



public:
    HRESULT         Initialize();
    HRESULT         Add_RenderObject(RENDERGROUP eID, CGameObject* pRenderObject);
    void            Draw();

private:
    void        Render_Priority();
    void        Render_NonBlend();
    void        Render_Blend();
    void        Render_UI();

private:
    ComPtr<ID3D11Device>                m_pDevice;
    ComPtr<ID3D11DeviceContext>        m_pContext;

    

public:
    static  CRenderer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    list<class CGameObject*>        m_RenderObjects[ENUM_TO_UINT(RENDERGROUP::END)];
    virtual void    Free() override;

};

NS_END