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
        _wstring ImgPath;
        _uint     ImgCnt = 1;

        _wstring ShaderName = L"Default";
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
    void                        Set_RenderGroup(_uint eGroup) { m_eRenderGroup = eGroup; }
public:
    static CQuad* Create(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pDeviceContext);
    virtual CGameObject* Clone(void* pArg) override;
    virtual void Free() override;

    
protected:
    RenderStates            m_RenderStates;
    class CTexture* m_pTexture = nullptr;
    class CShader* m_pTexShader = nullptr;
    class CVIBuffer_Rect* m_pVIBufferCom = { nullptr };

protected:
    _uint m_eRenderGroup = 0;
};

NS_END

