#include "CDefaultShader.h"

CDefaultShader::CDefaultShader(ComPtr<ID3D11Device> device)
	:_device(device)
{
}

CDefaultShader::~CDefaultShader()
{
}

void CDefaultShader::Create(const wstring& path, const string& name, const string& version)
{
}

void CDefaultShader::LoadShaderFromFile(const wstring& path, const string& name, const string& version)
{

	const UINT32 compileFlag = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;

	_path = path;
	_name = name;

	D3DCompileFromFile(path.c_str(),
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		name.c_str(),
		version.c_str(),
		compileFlag,
		0,
		_Blob.GetAddressOf(),
		nullptr);
}


void CDefaultShader::Free()
{
	__super::Free();
}
