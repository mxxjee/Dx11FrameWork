#pragma once
#include "CCamera_Base.h"

namespace Engine
{
    class COrthographicCameraComponent;
}

NS_BEGIN(Engine)

class ENGINE_DLL CMinimapCamera :
    public CCamera_Base
{

private:
    CMinimapCamera(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    CMinimapCamera(const CMinimapCamera& rhs);
    virtual ~CMinimapCamera() = default;

public:
    virtual HRESULT     Initialize_Prototype(); /*원형 객체가 생성될때 부르는 Initialize*/
    virtual HRESULT     Initialize_Copytype(void* pArg); /*사본 객체가 생성될때 부르는 Initialize*/

    virtual void        Update_Priority(_float fTimeDelta);
    virtual void        Update(_float fTimeDelta);
    virtual void        Update_Late(_float fTimeDelta);
    virtual void        Update_Render(_float fTimeDelta);

    virtual HRESULT Render();

public:
    void        Set_Target(CGameObject* pTarget);
    void        Follow_Target(_float fTimeDelta);

public:
    HRESULT     Create_RenderTagetview();
    virtual HRESULT        Bind_RenderTarget() override;
    virtual HRESULT        UnBind_RenderTarget()  override;
    virtual HRESULT         Clear_RenderTargeView(const _float4* pClearColor) override;
public:
    static CMinimapCamera* Create(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pDeviceContext);
    virtual CGameObject* Clone(void* pArg);
    virtual void    Free() override;

    const RENDER_TARGET& Get_RenderTarget() const { return m_tRenderTarget; }
private:
    RENDER_TARGET   m_tRenderTarget;

};

NS_END

