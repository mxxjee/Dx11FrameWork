#include "CPixelShader.h"

CPixelShader::CPixelShader(ComPtr<ID3D11Device> device)
	:CShader(device)
{
}

CPixelShader::~CPixelShader()
{
}

void CPixelShader::Create(const wstring& path, const string& name, const string& version)
{
	LoadShaderFromFile(path, name, version);
	HRESULT hr = _device->CreatePixelShader(_Blob->GetBufferPointer(), _Blob->GetBufferSize(), nullptr, _pixelShader.GetAddressOf());
	CHECK(hr);
}
