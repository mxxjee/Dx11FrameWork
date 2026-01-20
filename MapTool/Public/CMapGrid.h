#pragma once
#include "CTerrain_Base.h"

namespace Engine
{
    class CVIBuffer_CustomTerrain;
    class CShader;
    class CTexture;
}

NS_BEGIN(MapTool)


class CMapGrid :
    public CTerrain_Base
{
private:
    CMapGrid(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    CMapGrid(const CMapGrid& Prototype);
    virtual ~CMapGrid() = default;

public:
    virtual HRESULT Initialize_Prototype(); /* 원형객체가 만들어질 때 호출되는 함수. 무거운 초기화작업.  */
    virtual HRESULT Initialize_Copytype(void* pArg); /* 원형을 복제하여 만든 사본객체가 생성시에 호출되는 함수. 추가적인 초기화를 위해 */
    virtual void Update_Priority(_float fTimeDelta);
    virtual void Update(_float fTimeDelta);
    virtual void Update_Late(_float fTimeDelta);
    virtual void        Update_Render(_float fTimeDelta) override;
    virtual HRESULT Render();

public:
    HRESULT         CreateRasterizerState();
public:
    void        Update_Terrain(_uint NumX, _uint NumZ);
    CVIBuffer_CustomTerrain* Get_CustomVIBuffer() { return m_pCustomBuffer; }

private:
    HRESULT Ready_Components(void* pArg);

    HRESULT Bind_ShaderResources();

public:
    static CMapGrid* Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    virtual CGameObject* Clone(void* pArg);
    virtual void Free() override;

private:
    CVIBuffer_CustomTerrain*        m_pCustomBuffer = nullptr;
    ComPtr<ID3D11RasterizerState> m_pWireframeRS = nullptr;

};
NS_END
