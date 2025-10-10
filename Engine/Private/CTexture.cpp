#include "CTexture.h"
#include "Engine_Define.h"

CTexture::CTexture(ComPtr<ID3D11Device> device)
	:_device(device)
{
}

CTexture::~CTexture()
{
}

HRESULT CTexture::LoadImg(const wstring& path)
{

	//1.이미지 파일을 가져와라.
	
	DirectX::TexMetadata md;
	DirectX::ScratchImage img;

	HRESULT hr = LoadFromWICFile(path.c_str(), WIC_FLAGS_NONE, &md, img);
	CHECK(hr);

	hr = CreateShaderResourceView(_device.Get(), img.GetImages(), img.GetImageCount(), md, _shaderResourceView.GetAddressOf());
	CHECK(hr);

	return hr;
}

HRESULT CTexture::CreateResourceViewByTex(ComPtr<ID3D11Texture2D>& Source)
{
	_device->CreateShaderResourceView(Source.Get(), nullptr, _shaderResourceView.GetAddressOf());

	return S_OK;
}
