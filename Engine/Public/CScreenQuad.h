#pragma once
#include "CUI.h"

NS_BEGIN(Engine)

class ENGINE_DLL CScreenQuad :
    public CUI 
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

public:
    void Set_ScreenTexture(UINT Flag);
    HRESULT CreateBlendState();
    void Set_BlendState();

public:
    static CScreenQuad* Create(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pDeviceContext);
    virtual CGameObject* Clone(void* pArg) override;
    virtual void Free() override;


};

NS_END

