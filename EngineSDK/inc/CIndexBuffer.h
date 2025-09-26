
#include "CBase.h"
class CIndexBuffer :
    public CBase
{
public:
    CIndexBuffer();
    CIndexBuffer(ID3D11Device* pDevice);
    ~CIndexBuffer();


    ComPtr<ID3D11Buffer> GetComPtr() { return m_pIndexBuffer; }
    UINT32               GetStride() { return _stride; }
    UINT32               GetOffSet() { return _offset; }
    UINT32               GetCount() { return _count; }


    void    Create(const vector<UINT32>& _indices);

private:
    ComPtr<ID3D11Device>    m_pDevice;
    ComPtr<ID3D11Buffer>    m_pIndexBuffer = nullptr;

    UINT32      _stride = 0;
    UINT32      _offset = 0;
    UINT32      _count = 0;
};
