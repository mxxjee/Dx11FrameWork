#pragma once
#include "CShader.h"

NS_BEGIN(Engine)
class ENGINE_DLL CVertexShader :
    public CShader
{
public:
    CVertexShader(ComPtr<ID3D11Device> device);
    ~CVertexShader();


    ComPtr<ID3D11VertexShader>  GetComPtr() { return _vertexShader; }

    virtual void Create(const wstring& path, const string& name, const string& version);


protected:
    ComPtr<ID3D11VertexShader>  _vertexShader = nullptr;
};

NS_END

