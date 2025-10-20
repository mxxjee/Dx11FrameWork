#pragma once
#include "CGameObject.h"

namespace Engine
{
    class CVIBuffer_CustomTerrain;
    class CShader;
    class CTexture;
}

NS_BEGIN(MapTool)
class CMapTerrain :
    public CGameObject
{
public:
    typedef struct tagTerrainDesc : CGameObject::tagGameObjectDesc
    {
        _wstring TextureKey = L"";
        _wstring  ShaderName = L"";
        string  passName = "";


    }TERRAIN_DESC;

private:
    CMapTerrain(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    CMapTerrain(const CMapTerrain& Prototype);
    virtual ~CMapTerrain() = default;

public:
    virtual HRESULT Initialize_Prototype(); /* 원형객체가 만들어질 때 호출되는 함수. 무거운 초기화작업.  */
    virtual HRESULT Initialize_Copytype(void* pArg); /* 원형을 복제하여 만든 사본객체가 생성시에 호출되는 함수. 추가적인 초기화를 위해 */
    virtual void Update_Priority(_float fTimeDelta);
    virtual void Update(_float fTimeDelta);
    virtual void Update_Late(_float fTimeDelta);
    virtual void        Update_Render(_float fTimeDelta) override;
    virtual HRESULT Render();

private:
    CVIBuffer_CustomTerrain* m_pVIBufferCom = { nullptr };
    CTexture* m_pTexture = { nullptr };
    CShader* m_pShader = { nullptr };

private:
    HRESULT Ready_Components(void* pArg);
    HRESULT Ready_Resources(void* pArg);
    HRESULT Bind_ShaderResources();

public:
    static CMapTerrain* Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    virtual CGameObject* Clone(void* pArg);
    virtual void Free() override;

private:
    _wstring  m_ShaderName = L"";
    string      m_passName = "";
};
NS_END
