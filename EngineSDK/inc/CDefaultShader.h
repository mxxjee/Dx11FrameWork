#pragma once
#include "CBase.h"

enum ShaderScope
{
    /*리소스 사용시, 어느 단계에서 사용하는지 지칭*/
    SS_NONE = 0,
    SS_VertexShader=(1<<0),     //1
    SS_PixelShader=(1<<1),      //2
};
class ENGINE_DLL CDefaultShader :
    public CBase
{
public:
    CDefaultShader(ComPtr<ID3D11Device> device);
    virtual ~CDefaultShader();


public:
    virtual void Create(const wstring& path, const string& name, const string& version) = 0;
    ComPtr<ID3DBlob>        GetBlob() { return _Blob; }

protected:
    void        LoadShaderFromFile(const wstring& path, const string& name, const string& version);

public:
    virtual void Free() override;

protected:
    wstring _path;
    string _name;
    ComPtr<ID3D11Device>    _device;
    ComPtr<ID3DBlob> _Blob = nullptr;

};

