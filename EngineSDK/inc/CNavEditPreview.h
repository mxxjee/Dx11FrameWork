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
    void    UpdatePoints(deque<PreviewPoint> Points);
    HRESULT     Render();
    HRESULT     Ready_Components();
public:
    void        Set_Active(bool b) { m_bActive = b; }


public:
    void        Set_Shader(class CShader* pShader);
   

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

private:
    _float4x4   WorldMatrix;

private:
    ComPtr<ID3D11RasterizerState> m_pWireframeRS = nullptr;
};

NS_END
