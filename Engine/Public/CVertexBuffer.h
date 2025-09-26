#pragma once
#include "CBase.h"
class CVertexBuffer :
    public CBase
{
public:
    CVertexBuffer();
    CVertexBuffer(ID3D11Device* pDevice);
    ~CVertexBuffer();



    ComPtr<ID3D11Buffer> GetComPtr() { return m_pVertexBuffer; }
    UINT32               GetStride() { return _stride; }
    UINT32               GetOffSet() { return _offset; }
    UINT32               GetCount() { return _count; }

    template <typename T>
    void    Create(const vector<T>& _vertices)
    {
        _stride = sizeof(T);
        _count = static_cast<UINT32>(_vertices.size());

        D3D11_BUFFER_DESC desc;
        ZeroMemory(&desc, sizeof(desc));
        desc.Usage = D3D11_USAGE_IMMUTABLE;
        desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        desc.ByteWidth = (UINT32)sizeof(T) * _vertices.size();

        D3D11_SUBRESOURCE_DATA data;
        ZeroMemory(&data, sizeof(data));
        data.pSysMem = _indices.data();
        HRESULT hr = m_pDevice->CreateBuffer(&desc, &data, m_pVertexBuffer.GetAddressOf());

    }

private:
    ComPtr<ID3D11Device>    m_pDevice;
    ComPtr<ID3D11Buffer>    m_pVertexBuffer = nullptr;


    UINT32      _stride = 0;
    UINT32      _offset = 0;
    UINT32      _count = 0;
};

