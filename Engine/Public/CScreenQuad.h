#pragma once
#include "CGameObject.h"

NS_BEGIN(Engine)

class ENGINE_DLL CScreenQuad :
    public CGameObject
{
protected:
    CScreenQuad(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    CScreenQuad(const CScreenQuad& rhs);
    virtual ~CScreenQuad() = default;

public:
    virtual HRESULT     Initialize_Prototype(); /*원형 객체가 생성될때 부르는 Initialize*/
    virtual HRESULT     Initialize_Copytype(void* pArg); /*사본 객체가 생성될때 부르는 Initialize*/

    virtual void        Update_Priority(_float fTimeDelta);
    virtual void        Update(_float fTimeDelta);
    virtual void        Update_Late(_float fTimeDelta);
    virtual void        Update_Render(_float fTimeDelta) override;


    virtual HRESULT Render();  

private:
    HRESULT		CreateRasterizerState();
    HRESULT		CreateSamplerState();
    HRESULT		CreateBlendState();
public:
    void            Set_RenderGroup(RENDERGROUP eGroup) { m_eRenderGroup = eGroup; }
    void            Set_ScreenTexture(UINT iFlag=0);
private:
    void		Set_RasterizerState();
    void		Set_BlendState();

protected:
    RenderStates            m_RenderStates;
    class CTexture* m_pTexture = nullptr;
    class CShader* m_pTexShader = nullptr;
    class CVIBuffer_Rect* m_pVIBufferCom = { nullptr };

public:
    static CScreenQuad* Create(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pDeviceContext);
    virtual CGameObject* Clone(void* pArg) override;
    virtual void Free() override;




protected:
    RENDERGROUP m_eRenderGroup = RENDERGROUP::PRIORITY;

};

NS_END

