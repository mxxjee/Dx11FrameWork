#pragma once
#include "CBase.h"

template <typename T>
class CConstantBuffer :
    public CBase
{
public:
    CConstantBuffer(ComPtr<ID3D11Device>& device, ComPtr<ID3D11DeviceContext>& context)
        :m_pDevice{ device }, m_pContext{context}
    {

    }

    ~CConstantBuffer() = default;

    ComPtr<ID3D11Buffer>    GetComPtr() { return m_pConstantBuffer; }
    void Create()       //상수버퍼 만들기
    {
        
        D3D11_BUFFER_DESC desc;
        ZeroDeviceMemory(&desc, sizeof(desc));

        desc.Usage = D3D11_USAGE_DYNAMIC;
        desc.BindFlags - D3D11_BIND_CONSTANT_BUFFER;
        desc.ByteWidth = sizeof(T);
        desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

        
        HRESULT hr =m_pDevice->CreateBuffer(&desc, nullptr, m_pConstantBuffer.GetAddressOf());
        if (FAILEd(hr))
            return E_FAIL;


    }

    void CopyData(const T& data)
    {
        D3D11_MAPPED_SUBRESOURCE SubResource;
        ZeroMemory(&SubResource, sizeof(D3D11_MAPPED_SUBRESOURCE));


        //MAP : GPU에게 데이터를 넘겨줄 준비
        m_pContext->Map(m_pConstantBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &SubResource);
        memcpy(SubResource.pData, &data, sizeof(T));
        m_pContext->Unmap(m_pConstantBuffer.Get(), 0);


    }

private:
    ComPtr<ID3D11Device> m_pDevice;
    ComPtr<ID3D11DeviceContext> m_pContext;
    ComPtr<ID3D11Buffer> m_pConstantBuffer;

};

