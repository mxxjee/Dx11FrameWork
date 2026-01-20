#pragma once
#include "CMapObject.h"

NS_BEGIN(Engine)
class ENGINE_DLL CMapQuad :
    public CMapObject
{
public:
    typedef struct tagMapQuad_DESC : CMapObject::MapObject_DESC
    {
        _wstring    TextureKey=L"";

    }MAPQUAD_DESC;

protected:
    CMapQuad(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    CMapQuad(const CMapQuad& rhs);
    virtual ~CMapQuad() = default;

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
    static CMapQuad* Create(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pDeviceContext);
    virtual CGameObject* Clone(void* pArg) override;
    virtual void Free() override;
    
protected:
    class CTexture* m_pTexture = nullptr;
    class CVIBuffer_Rect* m_pVIBufferCom = { nullptr };
};

NS_END

