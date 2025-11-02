#pragma once
#include "CGameObject.h"

NS_BEGIN(Engine)

class CVIBuffer;
class CShader;
class CTexture;



class ENGINE_DLL CTerrain_Base :
    public CGameObject
{
public:
    typedef struct tagTerrainDesc : CGameObject::tagGameObjectDesc
    {
        _wstring TextureKey = L"";
        _wstring  ShaderName = L"";
        string  passName = "";

        _uint eRenderGroup = 0;


    }TERRAIN_DESC;

protected:
    CTerrain_Base(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    CTerrain_Base(const CTerrain_Base& Prototype);
    virtual ~CTerrain_Base() = default;

public:
    virtual HRESULT Initialize_Prototype(); /* 원형객체가 만들어질 때 호출되는 함수. 무거운 초기화작업.  */
    virtual HRESULT Initialize_Copytype(void* pArg); /* 원형을 복제하여 만든 사본객체가 생성시에 호출되는 함수. 추가적인 초기화를 위해 */
    virtual void Update_Priority(_float fTimeDelta);
    virtual void Update(_float fTimeDelta);
    virtual void Update_Late(_float fTimeDelta);
    virtual void        Update_Render(_float fTimeDelta) override;
    virtual HRESULT Render();

private:
    HRESULT Ready_Resources(void* pArg);
    HRESULT Bind_ShaderResources();

public:
    CVIBuffer* Get_VIBufferCom() { return m_pVIBufferCom; }

public:
     virtual void Free() override;

protected:
    CVIBuffer* m_pVIBufferCom = { nullptr };
    CTexture* m_pTexture = { nullptr };
    CShader* m_pShader = { nullptr };

protected:
    _uint                   m_eRenderGroup = 0;
    _wstring  m_ShaderName = L"";
    string      m_passName = "";
};

NS_END