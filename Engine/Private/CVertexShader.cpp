#include "CVertexShader.h"

CVertexShader::CVertexShader(ComPtr<ID3D11Device> device)
	:CShader(device)
{
}

CVertexShader::~CVertexShader()
{
}

void CVertexShader::Create(const wstring& path, const string& name, const string& version)
{
	LoadShaderFromFile(path, name, version);
	HRESULT hr = _device->CreateVertexShader(_Blob->GetBufferPointer(), _Blob->GetBufferSize(), nullptr, _vertexShader.GetAddressOf());
	CHECK(hr);
}