#pragma once
#include "CBase.h"

NS_BEGIN(Engine)
class ENGINE_DLL CRenderTarget :
    public CBase
{
private:
    CRenderTarget(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    virtual ~CRenderTarget() = default;
    
public:
    ComPtr<ID3D11RenderTargetView>      Get_RTV() const { return m_pRTV; }

public:
                            //텍스처2D의 사이즈와 픽셀포맷, 클리어컬러 지정
    HRESULT Initialize(_uint iWidth, _uint iHeight, DXGI_FORMAT ePixelFormat, const _float4& vClearColor);
    void        Clear();
#ifdef _DEBUG
public:
                    //디버그로 띄울 사각형 UI 크기설정
    HRESULT     Ready_Debug(_float fX, _float fY, _float fSizeX, _float fSizeY);
    HRESULT     Render(class CShader* pShader, class CVIBuffer_Rect* pVIBuffer);
#endif // _DEBUG

    HRESULT      Bind_ShaderResource(class CShader* pShader, const _char* pConstantName);
public:
    static CRenderTarget* Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, _uint iWidth, _uint iHeight, DXGI_FORMAT ePixelFormat, const _float4& vClearColor);
    virtual void Free() override;

private:
    ComPtr<ID3D11Device>        m_pDeivce = nullptr;
    ComPtr<ID3D11DeviceContext>        m_pContext = nullptr;

    ////////////texture
    ComPtr<ID3D11Texture2D>     m_pTexture2D = nullptr;
    ComPtr<ID3D11RenderTargetView>      m_pRTV = nullptr;
    ComPtr<ID3D11ShaderResourceView>    m_pSRV = nullptr;



private:
    _float4             m_vClearColor = {};

#ifdef _DEBUG
    _float4x4           m_WorldMatrix;          //디버그 전용으로 화면에 띄울떄.
#endif // _DEBUG

};


NS_END

