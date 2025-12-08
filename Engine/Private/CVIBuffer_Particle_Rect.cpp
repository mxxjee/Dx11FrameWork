#include "CVIBuffer_Particle_Rect.h"
#include "CGameInstance.h"

CVIBuffer_Particle_Rect::CVIBuffer_Particle_Rect(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    :CVIBuffer_Particle(pDevice,pContext)
{
}

CVIBuffer_Particle_Rect::CVIBuffer_Particle_Rect(const CVIBuffer_Particle_Rect& Prototype)
    :CVIBuffer_Particle(Prototype)
{
}

HRESULT CVIBuffer_Particle_Rect::Initialize_Prototype(const PARTICLE_DESC* pInitialDesc)
{
    //멤버변수 값채우고, 인덱스,버텍스 버퍼먼저 생성한다.
    //입자 1개 기준의 버텍스,인덱스버퍼
    m_iVertexStride = sizeof(VTXPOSTEX);
    m_iNumVertices = 4;
    m_iIndexStride = 2;

    m_iNumIndices = 6;
    m_iNumVertexBuffers = 2;        //(VertexBuffer+InstanceBuffer)
    m_ePrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

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

    VTXPOSTEX* pVertices = new VTXPOSTEX[m_iNumVertices];
    ZeroMemory(pVertices, sizeof(VTXPOSTEX) * m_iNumVertices);

    m_pVertexPositions;
    m_pVertexPositions.resize(m_iNumVertices);

    m_pVertexPositions[0] = pVertices[0].vPosition = _float3(-0.5f, 0.5f, 0.f);
    pVertices[0].vTexcoord = _float2(0.0f, 0.f);

    m_pVertexPositions[1] = pVertices[1].vPosition = _float3(0.5f, 0.5f, 0.f);
    pVertices[1].vTexcoord = _float2(1.0f, 0.f);

    m_pVertexPositions[2] = pVertices[2].vPosition = _float3(0.5f, -0.5f, 0.f);
    pVertices[2].vTexcoord = _float2(1.0f, 1.f);

    m_pVertexPositions[3] = pVertices[3].vPosition = _float3(-0.5f, -0.5f, 0.f);
    pVertices[3].vTexcoord = _float2(0.0f, 1.f);

    D3D11_SUBRESOURCE_DATA      VertexInitialData{};
    VertexInitialData.pSysMem = pVertices;

    if (FAILED(m_pDevice->CreateBuffer(&VertexBufferDesc, &VertexInitialData, &m_pVB)))
        return E_FAIL;

    Safe_Delete_Array(pVertices);

#pragma endregion

#pragma region INDEX_BUFFER
    D3D11_BUFFER_DESC           IndexBufferDesc{};
    IndexBufferDesc.ByteWidth = m_iIndexStride * m_iNumIndices;
    IndexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    IndexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    IndexBufferDesc.CPUAccessFlags = 0;
    IndexBufferDesc.MiscFlags = 0;
    IndexBufferDesc.StructureByteStride = m_iIndexStride;

    _ushort* pIndices = new _ushort[m_iNumIndices];

    pIndices[0] = 0;
    pIndices[1] = 1;
    pIndices[2] = 2;

    pIndices[3] = 0;
    pIndices[4] = 2;
    pIndices[5] = 3;

    D3D11_SUBRESOURCE_DATA      IndexInitialData{};
    IndexInitialData.pSysMem = pIndices;

    if (FAILED(m_pDevice->CreateBuffer(&IndexBufferDesc, &IndexInitialData, &m_pIB)))
        return E_FAIL;

    Safe_Delete_Array(pIndices);

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

HRESULT CVIBuffer_Particle_Rect::Initialize_Copytype(void* pArg)
{
    //사본시 실제로 생성
    D3D11_SUBRESOURCE_DATA InstanceInitialData{};
    InstanceInitialData.pSysMem = m_pInstanceVertices;
    if (FAILED(m_pDevice->CreateBuffer(&m_InstanceBufferDesc, &InstanceInitialData, &m_pVBInstance)))
        return E_FAIL;

    return S_OK;
}

CVIBuffer_Particle_Rect* CVIBuffer_Particle_Rect::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, const PARTICLE_DESC* pInitialDesc)
{
    CVIBuffer_Particle_Rect* pInstance = new CVIBuffer_Particle_Rect(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype(pInitialDesc)))
    {
        MSG_BOX("Failed to Created : CVIBuffer_Particle_Rect");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CComponent* CVIBuffer_Particle_Rect::Clone(void* pArg)
{
    CVIBuffer_Particle_Rect* pInstance = new CVIBuffer_Particle_Rect(*this);

    if (FAILED(pInstance->Initialize_Copytype(pArg)))
    {
        MSG_BOX("Failed to Cloned : CVIBuffer_Particle_Rect");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CVIBuffer_Particle_Rect::Free()
{
    __super::Free();

}
