#pragma once
#include "CEffect.h"

namespace Engine
{
    class CVIBuffer_Rect;
    class CTexture;
}
NS_BEGIN(Client)


class CQuadEffect :
    public CEffect
{
public:
    typedef struct tagQuadEffectDesc : public CEffect::tagEffectDesc
    {
        wstring         TextureKey = L"";
        string          PassName = "";


    }QUADEFFECT_DESC;


#ifdef _DEBUG
public:
    virtual void        Render_DebugImgui();
#endif // _DEBUG


protected:
    CQuadEffect(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    CQuadEffect(const CQuadEffect& rhs);
    virtual ~CQuadEffect() = default;

public:
    virtual HRESULT     Initialize_Prototype(); /*원형 객체가 생성될때 부르는 Initialize*/
    virtual HRESULT     Initialize_Copytype(void* pArg); /*원형 객체가 생성될때 부르는 Initialize*/

    virtual void        Update_Priority(_float fTimeDelta);
    virtual void        Update(_float fTimeDelta);
    virtual void        Update_Late(_float fTimeDelta);
    virtual void        Update_Render(_float fTimeDelta) override;


    virtual HRESULT Render();

public:
    HRESULT             Bind_ShaderResources();

public:
    HRESULT         Ready_Component(void* pArg);
    HRESULT          Ready_Resource(void* pArg);


public:
    virtual void    Spawn(const _float4x4* pSocketMatrix = nullptr, const _float4x4* pParentMatrix = nullptr);
    virtual void    Play();
    virtual void     Stop();


public:
    static CQuadEffect* Create(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pDeviceContext);
    virtual CGameObject* Clone(void* pArg) override;
    virtual void Free() override;

public:
    void                        Set_Texture(const _wstring& NewTexKey);

private:
    CTexture* m_pTexture = nullptr;
    CVIBuffer_Rect* m_pVIBufferCom = { nullptr };
    string                  m_passName = "";

};

NS_END