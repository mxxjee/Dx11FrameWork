#include "CVIBuffer_Particle.h"

CVIBuffer_Particle::CVIBuffer_Particle(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    :CVIBuffer{pDevice,pContext}
{
}

CVIBuffer_Particle::CVIBuffer_Particle(const CVIBuffer_Particle& Prototype)
    : CVIBuffer{ Prototype}
    , m_pVBInstance{Prototype.m_pVBInstance}
    ,m_InstanceBufferDesc{Prototype.m_InstanceBufferDesc}
    , m_pInstanceVertices{ Prototype.m_pInstanceVertices }  //초기값들을 저장한다.
    , m_iIndexCountPerInstance{ Prototype.m_iIndexCountPerInstance }      //인스턴스 하나 당의 인덱스개수
    , m_iNumInstance{ Prototype.m_iNumInstance }                //인스턴스 개수
    ,m_iInstanceVertexStride{ Prototype.m_iInstanceVertexStride }       //인스턴스 하나당 버텍스size
    , m_pSpeeds{ Prototype.m_pSpeeds }
    , m_isLoop{ Prototype.m_isLoop }
    , m_vPivot{Prototype.m_vPivot}
{
}

HRESULT CVIBuffer_Particle::Initialize_Prototype(const _tchar* pHeightFileMapPath)
{
    return S_OK;
}

HRESULT CVIBuffer_Particle::Initialize_Copytype(void* pArg)
{
    return S_OK;
}

HRESULT CVIBuffer_Particle::Bind_Resource()
{

    ID3D11Buffer* pVertexBuffers[] = {
        m_pVB.Get(),
        m_pVBInstance.Get()
    };

    _uint		 iVertexStrides[] = {
        m_iVertexStride,
        m_iInstanceVertexStride,
    };

    _uint		 iOffsets[] = {
        0,
        0
    };

    m_pContext->IASetVertexBuffers(0, m_iNumVertexBuffers, pVertexBuffers, iVertexStrides, iOffsets);
    m_pContext->IASetIndexBuffer(m_pIB.Get(), m_iIndexStride == 2 ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT, 0);
    m_pContext->IASetPrimitiveTopology(m_ePrimitiveType);


    return S_OK;
}

HRESULT CVIBuffer_Particle::Render()
{
    m_pContext->DrawIndexedInstanced(m_iIndexCountPerInstance, m_iNumInstance, 0, 0, 0);

    return S_OK;
}

void CVIBuffer_Particle::Drop(_float fTimeDelta)
{
    D3D11_MAPPED_SUBRESOURCE        SubResource{};

    m_pContext->Map(m_pVBInstance.Get(), 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

    VTXPARTICLE* pVertices = static_cast<VTXPARTICLE*>(SubResource.pData);
    for (size_t i = 0; i < m_iNumInstance; ++i)
    {
        pVertices[i].vTranslation.y -= m_pSpeeds[i] * fTimeDelta;
        pVertices[i].vLifeTime.x += fTimeDelta;
        if (true == m_isLoop &&
            pVertices[i].vLifeTime.x >= pVertices[i].vLifeTime.y)
        {
            pVertices[i].vLifeTime.x= 0.f;
                                //루프 활성화 시 다시 처음부터
            pVertices[i].vTranslation = m_pInstanceVertices[i].vTranslation;

        }
    }
    m_pContext->Unmap(m_pVBInstance.Get(), 0);
}

void CVIBuffer_Particle::Spread(_float fTimeDelta)
{
    D3D11_MAPPED_SUBRESOURCE        SubResource{};

    m_pContext->Map(m_pVBInstance.Get(), 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

    VTXPARTICLE* pVertices = static_cast<VTXPARTICLE*>(SubResource.pData);
    for (size_t i = 0; i < m_iNumInstance; ++i)
    {
        _vector vLook = XMVector3Normalize(XMVectorSetW(XMLoadFloat4(&pVertices[i].vTranslation) - XMLoadFloat3(&m_vPivot), 0.f)) * m_pSpeeds[i];
        
        
        XMStoreFloat4(&pVertices[i].vTranslation, XMLoadFloat4(&pVertices[i].vTranslation) + vLook * fTimeDelta);

        pVertices[i].vLifeTime.x += fTimeDelta;
        if (true == m_isLoop &&
            pVertices[i].vLifeTime.x >= pVertices[i].vLifeTime.y)
        {
            pVertices[i].vLifeTime.x = 0.f;
            //루프 활성화 시 다시 처음부터
            pVertices[i].vTranslation = m_pInstanceVertices[i].vTranslation;

        }
    }
    m_pContext->Unmap(m_pVBInstance.Get(), 0);
}


void CVIBuffer_Particle::Free()
{
    __super::Free();
    if (false == m_isClone)
    {
        Safe_Delete_Array(m_pSpeeds);
        Safe_Delete_Array(m_pInstanceVertices);

    }

    
}
