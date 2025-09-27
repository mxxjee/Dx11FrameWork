#pragma once
#include "CBase.h"

NS_BEGIN(Engine)
class ENGINE_DLL CTexture:public CBase
{
public:
	CTexture(ComPtr<ID3D11Device> device);
	~CTexture();

	ComPtr<ID3D11ShaderResourceView> GetComPtr() { return _shaderResourceView; }

	void Create(const wstring& path);
private:
	ComPtr<ID3D11Device> _device;
	ComPtr<ID3D11ShaderResourceView> _shaderResourceView = nullptr;

};

NS_END