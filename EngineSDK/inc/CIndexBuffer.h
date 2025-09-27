
#include "CBase.h"

NS_BEGIN(Engine)

class ENGINE_DLL CIndexBuffer :
    public CBase
{
public:
    CIndexBuffer();
    CIndexBuffer(ComPtr<ID3D11Device> pDevice);
    ~CIndexBuffer();


    ComPtr<ID3D11Buffer> GetComPtr() { return m_pIndexBuffer; }
    UINT32               GetStride() { return _stride; }
    UINT32               GetOffSet() { return _offset; }
    UINT32               GetCount() { return _count; }


    void    Create(const vector<UINT32>& _indices);
public:
    virtual void Free() override;

private:
    ComPtr<ID3D11Device>    m_pDevice;
    ComPtr<ID3D11Buffer>    m_pIndexBuffer = nullptr;

    UINT32      _stride = 0;
    UINT32      _offset = 0;
    UINT32      _count = 0;
};

NS_END