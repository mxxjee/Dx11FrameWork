#include "CVIBuffer.h"

CVIBuffer::CVIBuffer(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    :CComponent(pDevice,pContext)
{
}

CVIBuffer::CVIBuffer(const CVIBuffer& Prototype)
    : CComponent(Prototype),
    m_pVB(Prototype.m_pVB),
    m_pIB(Prototype.m_pIB),
    m_iNumVertices(Prototype.m_iNumVertices),
    m_iVertexStride(Prototype.m_iVertexStride),
    m_iNumIndices(Prototype.m_iNumIndices),
    m_iIndexStride(Prototype.m_iIndexStride),
    m_iNumVertexBuffers(Prototype.m_iNumVertexBuffers),
    m_iNumIndexBuffers(Prototype.m_iNumIndexBuffers),
    m_pVertexPositions(Prototype.m_pVertexPositions)
{
    m_pIndices = new _uint[m_iNumIndices];
    memcpy((_uint*)m_pIndices, (_uint*)Prototype.m_pIndices, sizeof(_uint) * m_iNumIndices);
}


HRESULT CVIBuffer::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CVIBuffer::Initialize_Copytype(void* pArg)
{
    return S_OK;
}

HRESULT CVIBuffer::Bind_Resource()
{
    //[IA단계 ] 
    //1.생성한 IB와 VB를 바인딩.
    ComPtr<ID3D11Buffer>   pVertexBuffers[] =
    {
        m_pVB,
    };

    ComPtr<ID3D11Buffer>   pIndexBuffers[] =
    {
        m_pIB,
    };

    _uint		 iOffsets[] = {
    0
    }; //D3D11_INPUT_CLASSIFICATION

    m_iNumVertexBuffers = 1;

                      
    m_pContext->IASetVertexBuffers(0, m_iNumVertexBuffers, pVertexBuffers->GetAddressOf(), &m_iVertexStride, iOffsets);
                      //몇번의 버텍스버퍼 슬롯부터 바인딩할건지, 버텍스버퍼개수 
    
    m_pContext->IASetIndexBuffer(m_pIB.Get(), m_iIndexStride == 2 ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT, 0);
    m_pContext->IASetPrimitiveTopology(m_ePrimitiveType);


    //InputLayout은 Shader클래스에서 생성

    return S_OK;
}

HRESULT CVIBuffer::Render()
{
    m_pContext->DrawIndexed(m_iNumIndices, 0, 0);
    return S_OK;
}


void CVIBuffer::Free()
{
    __super::Free();
    if (!m_isClone)
        Safe_Delete_Array(m_pVertexPositions);
}
