#pragma once
#include "CBase.h"


NS_BEGIN(Engine)
class CVIBuffer_Triangle;
class CGameInstance;

class ENGINE_DLL CNavEditPreview :
    public CBase
{
private:
    CNavEditPreview(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    virtual ~CNavEditPreview() = default;

public:
    HRESULT Initialize();
    void    UpdatePoints(_float3 p0, _float3 p1, _float3 p2);
    HRESULT     Render();
    HRESULT     Ready_Components();
public:
    void        Set_Active(bool b) { m_bActive = b; }




public:                             //생성함수, 각 정점의 위치3개, 인덱스
    static CNavEditPreview* Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    virtual void Free() override;

private:
    ComPtr<ID3D11Device>        m_pDevice;
    ComPtr<ID3D11DeviceContext> m_pContext;
    class CShader*              m_pShader = nullptr;

private:
    bool            m_bActive = true;
    CVIBuffer_Triangle* m_pVIBufferCom = { nullptr };
    CGameInstance* m_pGameInstance = nullptr;

};

NS_END
