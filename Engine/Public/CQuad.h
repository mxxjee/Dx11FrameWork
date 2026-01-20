#pragma once
#include "CGameObject.h"
#include "VertexData.h"

NS_BEGIN(Engine)

class ENGINE_DLL CQuad :
    public CGameObject
{
public:
    typedef struct tagQuad_DESC : CGameObject::GAMEOBJECT_DESC
    {
        _wstring    TextureKey;


        _wstring    ShaderName = L"Default";
        string      passName = "Default";

        _uint eRenderGroup = 0;
        
        
    }QUAD_DESC;


protected:
    CQuad(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    CQuad(const CQuad& rhs);
    virtual ~CQuad() = default;

public:
    virtual HRESULT     Initialize_Prototype(); /*원형 객체가 생성될때 부르는 Initialize*/
    virtual HRESULT     Initialize_Copytype(void* pArg); /*사본 객체가 생성될때 부르는 Initialize*/

    virtual void        Update_Priority(_float fTimeDelta);
    virtual void        Update(_float fTimeDelta);
    virtual void        Update_Late(_float fTimeDelta);
    virtual void        Update_Render(_float fTimeDelta) override;


    virtual HRESULT Render();


public:
    HRESULT                     Ready_Components(void* pArg);
    HRESULT                     Ready_Resources(void* pArg);
    HRESULT                     Bind_ShaderResources();
public:
    void                        Set_Texture(const _wstring& NewTexKey);
    void                        Set_RenderGroup(_uint eGroup) { m_eRenderGroup = eGroup; }
public:
    static CQuad* Create(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pDeviceContext);
    virtual CGameObject* Clone(void* pArg) override;
    virtual void Free() override;

    
protected:
    RenderStates                m_RenderStates;
    class CTexture*             m_pTexture = nullptr;
    class CShader*              m_pShader = nullptr;
    class CVIBuffer_Rect*       m_pVIBufferCom = { nullptr };

public:
    void            Set_ARGB(_float4 rgba) { m_vARGB = rgba; }

public:
    _float4         Get_ARGB() { return m_vARGB; }

protected:
    _uint                   m_eRenderGroup = 0;
    _wstring                m_ShaderName = L"";
    string                  m_passName = "";

protected:
    //셰이더에게 던져줄값들.
    _float4                     m_vARGB = { 1.f,1.f,1.f,1.f };   //기본적으로 1,1,1,1(실제 색상이 아닌 비율값)
    _float                      g_Brightness = 1.f;
};

NS_END

