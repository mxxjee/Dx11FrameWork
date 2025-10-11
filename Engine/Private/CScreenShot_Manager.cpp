#include "CScreenShot_Manager.h"
#include "CTexture.h"
#include "CGameInstance.h"



CScreenShot_Manager::CScreenShot_Manager(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pContext)
    :m_pDevice(_pDevice), m_pContext(_pContext)
{
}

void CScreenShot_Manager::ScreenShot(const _wstring& Key)
{
 

    //만약 같은 키값이 존재한다면 덮어쓰기
    CTexture* pFind = Find_ScreenTexture(Key);
    if (pFind)
    {
        Safe_Release(pFind);
        m_Texmap.erase(Key);
    }
    CTexture* pTexture = nullptr;

    ComPtr<ID3D11Texture2D> pBackBuffer;
    ComPtr<ID3D11Texture2D> pCopiedTex;

  
    CGameInstance::GetInstance()->Get_Buffer(&pBackBuffer, 0);
   
    D3D11_TEXTURE2D_DESC pdesc{};
    pBackBuffer->GetDesc(&pdesc);
    OutputDebugString((L"BackBuffer Format: " + std::to_wstring(pdesc.Format) + L"\n").c_str());



    _wstring SavePath = L"../../ScreenShots/" + Key + L".png";

    //텍스처생성
    D3D11_TEXTURE2D_DESC desc{};
    pBackBuffer->GetDesc(&desc);
    desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.CPUAccessFlags = 0;

    m_pDevice->CreateTexture2D(&desc, nullptr, pCopiedTex.GetAddressOf());


    //GPU복사
    m_pContext->CopyResource(pCopiedTex.Get(), pBackBuffer.Get());

    pTexture = new CTexture(m_pDevice);
    pTexture->CreateResourceViewByTex(pCopiedTex);
   

    m_Texmap.emplace(Key, pTexture);
    
}

CTexture* CScreenShot_Manager::Find_ScreenTexture(const _wstring& Key)
{
    auto iter = m_Texmap.find(Key);
    if (iter != m_Texmap.end())
        return iter->second;

    return nullptr;
}

CScreenShot_Manager* CScreenShot_Manager::Create(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pContext)
{
    return new CScreenShot_Manager(_pDevice,_pContext);
}

void CScreenShot_Manager::Free()
{
    for (auto& pair : m_Texmap)
    {
       Safe_Release(pair.second);
    }


}

HRESULT CScreenShot_Manager::SaveTextureToFile(const _wstring& Key, const _wstring& filePath)
{
   
    CTexture* pTexture = Find_ScreenTexture(Key);
    CheckNullResult(pTexture, E_FAIL);

    ComPtr<ID3D11Texture2D> comTex = pTexture->GetTexture();

    D3D11_TEXTURE2D_DESC desc{};
    comTex->GetDesc(&desc);

    desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
    desc.Usage = D3D11_USAGE_STAGING;
    desc.BindFlags = 0;
    desc.MiscFlags = 0;

    ComPtr<ID3D11Texture2D> staging;
    HRESULT hr = m_pDevice->CreateTexture2D(&desc, nullptr, staging.GetAddressOf());

    if (FAILED(hr))
        return E_FAIL;

    //받은 소스를 staging버퍼에 복사
    m_pContext->CopyResource(staging.Get(), comTex.Get());

    //D3D11_MAPPED_SUBRESOURCE mapped{};
    //hr = m_pContext->Map(staging.Get(), 0, D3D11_MAP_READ, 0, &mapped);
    //if (SUCCEEDED(hr))
    //{
    //    for (UINT y = 0; y < desc.Height; ++y)
    //    {
    //        uint8_t* row = reinterpret_cast<uint8_t*>(mapped.pData) + y * mapped.RowPitch;
    //        for (UINT x = 0; x < desc.Width; ++x)
    //        {
    //            row[x * 4 + 3] = 255; // A채널을 강제로 1.0f로 고정
    //        }
    //    }
    //    m_pContext->Unmap(staging.Get(), 0);
    //}

    ScratchImage img;
    hr = CaptureTexture(m_pDevice.Get(), m_pContext.Get(), staging.Get(), img);
    if (FAILED(hr))
        return E_FAIL;

    ScratchImage rgbImage;
    //그냥저장하면 Alpha값이 
    hr = DirectX::Convert(
        *img.GetImage(0, 0, 0),
        DXGI_FORMAT_B8G8R8X8_UNORM,
        DirectX::TEX_FILTER_DEFAULT,
        0.0f,
        rgbImage
    );

    hr = SaveToWICFile(
        *rgbImage.GetImage(0, 0, 0),
        WIC_FLAGS_FORCE_SRGB,
        GUID_ContainerFormatPng,
        filePath.c_str()
    );


    return S_OK;
}


