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
    m_pContext->IASetIndexBuffer(m_pIB.Get(), m_iIndexStride == 2 ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT, 0);
    m_pContext->IASetPrimitiveTopology(m_ePrimitiveType);


    //2.InputLayout생성
    ComPtr<ID3D11InputLayout>       pInputLayout;
    D3D11_INPUT_ELEMENT_DESC    layoutDesc[] = {
        //(float3)
        { "POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0},
        { "TEXCOORD",0,DXGI_FORMAT_R32G32_FLOAT,0,12,D3D11_INPUT_PER_VERTEX_DATA,0}
    };

    ComPtr<ID3D11VertexShader>  VertexShader;
    ComPtr<ID3DBlob>  vsBlob;

   // m_pDevice->CreateVertexShader()
    //m_pDevice->CreateInputL ayout(layoutDesc,sizeof(layoutDesc)/sizeof(layoutDesc[0]),
   
    // m_pContext->IASetInputLayout(pInputLayout.Get());

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
