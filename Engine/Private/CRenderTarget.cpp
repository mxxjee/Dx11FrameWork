#include "CRenderTarget.h"
#include "CShader.h"
#include "CVIBuffer_Rect.h"
CRenderTarget::CRenderTarget(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    :m_pDeivce(pDevice), m_pContext(pContext)
{
}

HRESULT CRenderTarget::Initialize(_uint iWidth, _uint iHeight, DXGI_FORMAT ePixelFormat, const _float4& vClearColor)
{
    m_vClearColor = vClearColor;

    D3D11_TEXTURE2D_DESC TextureDesc{};

    TextureDesc.Width = iWidth;
    TextureDesc.Height = iHeight;
    TextureDesc.MipLevels = 1;
    TextureDesc.ArraySize = 1;
    TextureDesc.Format = ePixelFormat;

    TextureDesc.SampleDesc.Quality = 0;
    TextureDesc.SampleDesc.Count = 1;

    TextureDesc.Usage = D3D11_USAGE_DEFAULT;
    TextureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;

    TextureDesc.CPUAccessFlags = 0;
    TextureDesc.MiscFlags = 0;


    //Texutre2D/ RTV/SRV를만들자
    if (FAILED(m_pDeivce->CreateTexture2D(&TextureDesc, nullptr, m_pTexture2D.GetAddressOf())))
        return E_FAIL;

    if (FAILED(m_pDeivce->CreateRenderTargetView(m_pTexture2D.Get(), nullptr, m_pRTV.GetAddressOf())))
        return E_FAIL;


    if (FAILED(m_pDeivce->CreateShaderResourceView(m_pTexture2D.Get(), nullptr, m_pSRV.GetAddressOf())))
        return E_FAIL;

    return S_OK;
}

#ifdef _DEBUG
HRESULT CRenderTarget::Ready_Debug(_float fX, _float fY, _float fSizeX, _float fSizeY)
{
    _uint      iNumViewPots = {};
    D3D11_VIEWPORT      ViewPortDesc = {};


    m_pContext->RSGetViewports(&iNumViewPots, &ViewPortDesc);


    XMStoreFloat4x4(&m_WorldMatrix, XMMatrixScaling(fSizeX, fSizeY, 1.f) * XMMatrixTranslation(fX - ViewPortDesc.Width * 0.5f,
        -fY + ViewPortDesc.Height * 0.5f , 0));
   return S_OK;
}

HRESULT CRenderTarget::Render(CShader* pShader, CVIBuffer_Rect* pVIBuffer)
{
    if (FAILED(pShader->Bind_Matrix("g_WorldMatrix", m_WorldMatrix)))
        return E_FAIL;

    if (FAILED(pShader->Bind_SRV("g_Texture",m_pSRV)))
        return E_FAIL;

    if (FAILED(pShader->Begin("Default")))
        return E_FAIL;

    if (FAILED(pVIBuffer->Render()))
        return E_FAIL;
    return S_OK;
}
#endif // _DEBUG



CRenderTarget* CRenderTarget::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, _uint iWidth, _uint iHeight, DXGI_FORMAT ePixelFormat, const _float4& vClearColor)
{
    CRenderTarget* pInstance = new CRenderTarget(pDevice, pContext);

    if (FAILED(pInstance->Initialize(iWidth, iHeight, ePixelFormat, vClearColor)))
    {
        MSG_BOX("Failed to Created : CRenderTarget");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CRenderTarget::Free()
{
    __super::Free();


}
