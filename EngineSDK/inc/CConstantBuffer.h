#pragma once
#include "CBase.h"

NS_BEGIN(Engine)

class ENGINE_DLL CConstantBuffer :
    public CBase
{
public:
    CConstantBuffer(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> context)
        :m_pDevice(device), m_pContext(context)
    {

    }

    ~CConstantBuffer() = default;

    ComPtr<ID3D11Buffer>    GetComPtr() { return m_pConstantBuffer; }
    void Create(int Slot,_uint iSize)       //상수버퍼 만들기
    {
        m_iSlot = Slot;

        D3D11_BUFFER_DESC desc;
        memset(&desc, 0,sizeof(desc));

        desc.Usage = D3D11_USAGE_DYNAMIC;
        desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        desc.ByteWidth = iSize;
        desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;


        HRESULT hr = m_pDevice->CreateBuffer(&desc, nullptr, m_pConstantBuffer.GetAddressOf());
        if (FAILED(hr))
            return;


    }

    void CopyData(const void* pData,_uint iSize)
    {
        D3D11_MAPPED_SUBRESOURCE SubResource;

        memset(&SubResource,0, sizeof(D3D11_MAPPED_SUBRESOURCE));

    

        //MAP : GPU에게 데이터를 넘겨줄 준비
        //직접 ConstantBuffer을 채우는것이 아닌, Subresource(Map을 통해 CPU가 임시 접근할 수 있는공간)에 복사
        m_pContext->Map(m_pConstantBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &SubResource);

        memcpy(SubResource.pData, pData, iSize);


        m_pContext->Unmap(m_pConstantBuffer.Get(), 0);


    }


public:
    virtual void Free()
    {
        __super::Free();
    }

private:
    ComPtr<ID3D11Device> m_pDevice;
    ComPtr<ID3D11DeviceContext> m_pContext;
    ComPtr<ID3D11Buffer> m_pConstantBuffer;

private:
    _uint               m_iSlot = 0;    //register(b#)

};


NS_END


