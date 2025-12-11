#include "CVIBuffer_Particle_Point.h"
#include "CGameInstance.h"

CVIBuffer_Particle_Point::CVIBuffer_Particle_Point(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    :CVIBuffer_Particle(pDevice,pContext)
{
}

CVIBuffer_Particle_Point::CVIBuffer_Particle_Point(const CVIBuffer_Particle_Point& Prototype)
    :CVIBuffer_Particle(Prototype)
{
}

HRESULT CVIBuffer_Particle_Point::Initialize_Prototype(const PARTICLE_DESC* pInitialDesc)
{
    //멤버변수 값채우고, 인덱스,버텍스 버퍼먼저 생성한다.
    //입자 1개 기준의 버텍스,인덱스버퍼
    m_iVertexStride = sizeof(VTXPOS);
    m_iNumVertices = 1;
    m_iIndexStride = 0;

    m_iNumIndices = 0;
    m_iNumVertexBuffers = 2;        //(VertexBuffer+InstanceBuffer)
    m_ePrimitiveType = D3D_PRIMITIVE_TOPOLOGY_POINTLIST;

    const PARTICLE_RECT_DESC* pParticleDesc = static_cast<const PARTICLE_RECT_DESC*>(pInitialDesc);

    m_isLoop = pParticleDesc->isLoop;
    m_iInstanceVertexStride = sizeof(VTXPARTICLE);
    m_iNumInstance = pParticleDesc->iNumInstance;
    m_iIndexCountPerInstance = m_iNumIndices;

#pragma region VERTEX_BUFFER
    D3D11_BUFFER_DESC           VertexBufferDesc{};
    VertexBufferDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
    VertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    VertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    VertexBufferDesc.CPUAccessFlags = 0;
    VertexBufferDesc.MiscFlags = 0;
    VertexBufferDesc.StructureByteStride = m_iVertexStride;

    VTXPOS* pVertices = new VTXPOS[m_iNumVertices];
    ZeroMemory(pVertices, sizeof(VTXPOS) * m_iNumVertices);

    m_pVertexPositions;
    m_pVertexPositions.resize(m_iNumVertices);

    //기준점
    m_pVertexPositions[0] = pVertices[0].vPosition = _float3(0.f, 0.f, 0.f);
   
    D3D11_SUBRESOURCE_DATA      VertexInitialData{};
    VertexInitialData.pSysMem = pVertices;

    if (FAILED(m_pDevice->CreateBuffer(&VertexBufferDesc, &VertexInitialData, &m_pVB)))
        return E_FAIL;

    Safe_Delete_Array(pVertices);

#pragma endregion



#pragma region INSTANCE_BUFFER
    m_InstanceBufferDesc.ByteWidth = m_iNumInstance * m_iInstanceVertexStride;
    m_InstanceBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    m_InstanceBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    m_InstanceBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    m_InstanceBufferDesc.StructureByteStride = m_iInstanceVertexStride;

    m_pInstanceVertices = new VTXPARTICLE[m_iNumInstance];
    ZeroMemory(m_pInstanceVertices, sizeof(VTXPARTICLE) * m_iNumInstance);

    m_pSpeeds = new _float[m_iNumInstance];
    ZeroMemory(m_pSpeeds, sizeof(_float) * m_iNumInstance);

    m_vPivot = pParticleDesc->vPivot;
    for (size_t i = 0; i < m_iNumInstance; ++i)
    {
        _float fScale = m_pGameInstance->Random(pParticleDesc->vSize.x, pParticleDesc->vSize.y);
        m_pSpeeds[i] = m_pGameInstance->Random(pParticleDesc->vSpeed.x, pParticleDesc->vSpeed.y);

                            //(1.0.0.0)*(fScale)

        XMStoreFloat4(&m_pInstanceVertices[i].vRight, XMVectorSet(1.f, 0.f, 0.f, 0.f) * fScale);
        XMStoreFloat4(&m_pInstanceVertices[i].vUp, XMVectorSet(0.f, 1.f, 0.f, 0.f) * fScale);
        XMStoreFloat4(&m_pInstanceVertices[i].vLook, XMVectorSet(0.f, 0.f, 1.f, 0.f) * fScale);

  
        m_pInstanceVertices[i].vTranslation = _float4(
            m_pGameInstance->Random(pParticleDesc->vCenter.x - pParticleDesc->vRange.x * 0.5f, pParticleDesc->vCenter.x + pParticleDesc->vRange.x * 0.5f),
            m_pGameInstance->Random(pParticleDesc->vCenter.y - pParticleDesc->vRange.y * 0.5f, pParticleDesc->vCenter.y + pParticleDesc->vRange.y * 0.5f),
            m_pGameInstance->Random(pParticleDesc->vCenter.z - pParticleDesc->vRange.z * 0.5f, pParticleDesc->vCenter.z + pParticleDesc->vRange.z * 0.5f),
            1.f
        );


        //LifeTime(현재값, 최대lifetime)
        m_pInstanceVertices[i].vLifeTime = _float2(0.f, m_pGameInstance->Random(pParticleDesc->vLifeTime.x, pParticleDesc->vLifeTime.y));
    
    }
#pragma endregion
    return S_OK;
}

HRESULT CVIBuffer_Particle_Point::Initialize_Copytype(void* pArg)
{
    //사본시 실제로 생성
    D3D11_SUBRESOURCE_DATA InstanceInitialData{};
    InstanceInitialData.pSysMem = m_pInstanceVertices;
    if (FAILED(m_pDevice->CreateBuffer(&m_InstanceBufferDesc, &InstanceInitialData, &m_pVBInstance)))
        return E_FAIL;

    return S_OK;
}

HRESULT CVIBuffer_Particle_Point::Bind_Resource()
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
    m_pContext->IASetPrimitiveTopology(m_ePrimitiveType);



    return S_OK;
}

HRESULT CVIBuffer_Particle_Point::Render()
{
                    //점하나씩 GS에서 그릴거라서 인덱스버퍼 X
    m_pContext->DrawInstanced(1, m_iNumInstance, 0, 0);

    return S_OK;
}

CVIBuffer_Particle_Point* CVIBuffer_Particle_Point::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, const PARTICLE_DESC* pInitialDesc)
{
    CVIBuffer_Particle_Point* pInstance = new CVIBuffer_Particle_Point(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype(pInitialDesc)))
    {
        MSG_BOX("Failed to Created : CVIBuffer_Particle_Point");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CComponent* CVIBuffer_Particle_Point::Clone(void* pArg)
{
    CVIBuffer_Particle_Point* pInstance = new CVIBuffer_Particle_Point(*this);

    if (FAILED(pInstance->Initialize_Copytype(pArg)))
    {
        MSG_BOX("Failed to Cloned : CVIBuffer_Particle_Point");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CVIBuffer_Particle_Point::Free()
{
    __super::Free();

}
