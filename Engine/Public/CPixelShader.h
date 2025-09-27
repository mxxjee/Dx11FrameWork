#pragma once
#include "CShader.h"

NS_BEGIN(Engine)
class ENGINE_DLL CPixelShader :
    public CShader
{
public:
    CPixelShader(ComPtr<ID3D11Device> device);
    ~CPixelShader();


    ComPtr<ID3D11PixelShader>  GetComPtr() { return _pixelShader; }

    virtual void Create(const wstring& path, const string& name, const string& version);


protected:
    ComPtr<ID3D11PixelShader>  _pixelShader = nullptr;
};
NS_END
