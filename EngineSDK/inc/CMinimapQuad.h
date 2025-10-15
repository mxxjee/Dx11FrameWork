#pragma once
#include "CUI.h"

NS_BEGIN(Engine)
class ENGINE_DLL CMinimapQuad :
    public CUI
{
protected:
    CMinimapQuad(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    CMinimapQuad(const CMinimapQuad& rhs);
    virtual ~CMinimapQuad() = default;

public:
    virtual HRESULT     Initialize_Prototype(); /*원형 객체가 생성될때 부르는 Initialize*/
    virtual HRESULT     Initialize_Copytype(void* pArg); /*사본 객체가 생성될때 부르는 Initialize*/

    virtual void        Update_Priority(_float fTimeDelta);
    virtual void        Update(_float fTimeDelta);
    virtual void        Update_Late(_float fTimeDelta);
    virtual void        Update_Render(_float fTimeDelta) override;
    virtual HRESULT        Render() override;

    virtual HRESULT     CreateTexture(const RENDER_TARGET& m_Target);

    void            CreateBlendState();
public:
    static CMinimapQuad* Create(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pDeviceContext);
    virtual CGameObject* Clone(void* pArg) override;
    virtual void Free() override;


private:
    class CMinimapCamera* m_pMinimapCamera = nullptr;


private:
    ComPtr<ID3D11BlendState>        m_BlendState;
};


NS_END