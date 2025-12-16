#pragma once
#include "CComponent.h"
NS_BEGIN(Engine)
class ENGINE_DLL CFontComponent :
    public CComponent
{

public:
    struct Font_Desc : public CComponent::COMPONENT_DESC
    {
        wstring FontName;
        _float4 vDefaultFontColor = _float4(0.f, 0.f, 0.f, 1.f);//기본색 검은색
        _float fScale = 10.f;
        _float fRotation = 0.f;
    
    };

private:
    CFontComponent(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    CFontComponent(const CFontComponent& Prototype);
    virtual ~CFontComponent() = default;


public:
    virtual         HRESULT     Initialize_Prototype() override;
    virtual         HRESULT     Initialize_Copytype(void* pArg) override;


public:
    void        Set_Text(wstring _Text);

public:
    virtual HRESULT    Render();        

public:
    void        Update(XMMATRIX    WorldMatrix,_float alpha);
public:
    static CFontComponent* Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    virtual CComponent* Clone(void* pArg);
    virtual void Free() override;

private:
    HRESULT Create_BS_AlphaBlend();

private:
    wstring     m_pText;
    _float2     m_vPosition;
    _float4     m_vColor;
    _float      m_vScale;
    _float      m_fRotation = 0;
private:
    SpriteBatch* m_pBatch = { nullptr };
    SpriteFont* m_pFont = { nullptr };

    ComPtr<ID3D11BlendState> BS_AlphaBlend;

};
NS_END

