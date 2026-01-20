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

HRESULT CVIBuffer_Particle_Point::Initialize_Prototype()
{
    //멤버변수 값채우고, 인덱스,버텍스 버퍼먼저 생성한다.
    //입자 1개 기준의 버텍스,인덱스버퍼
    m_iVertexStride = sizeof(VTXPOS);
    m_iNumVertices = 1;
    m_iIndexStride = 0;

    m_iNumIndices = 0;
    m_iNumVertexBuffers = 2;        //(VertexBuffer+InstanceBuffer)
    m_ePrimitiveType = D3D_PRIMITIVE_TOPOLOGY_POINTLIST;

   

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


//모든 버퍼가 다른 Instance_buffer을 가지게 하기 위해 copy_type떄 instancebuffer생성

    return S_OK;
}

HRESULT CVIBuffer_Particle_Point::Initialize_Copytype(void* pArg)
{
    //사본시 실제로 생성
    tagVIBuffer_Particle_Desc* pDesc = static_cast<tagVIBuffer_Particle_Desc*>(pArg);
    if (FAILED(Reset_Particle(*pDesc)))
        return E_FAIL;


    return S_OK;
}

HRESULT CVIBuffer_Particle_Point::Reset_Particle(const tagVIBuffer_Particle_Desc& pData)
{
    //기존버퍼가있다면 삭제한다.
    if (m_pVBInstance)
    {
        m_pVBInstance.Reset();
        m_pVBInstance = nullptr;
    }

    if (m_pInstanceVertices)
    {
        delete[] m_pInstanceVertices;
        m_pInstanceVertices = nullptr;
    }


    if (m_pSpeeds)
    {
        delete[] m_pSpeeds;
        m_pSpeeds = nullptr;
    }

    //데이터갱신


    m_isLoop = pData.isLoop;
    m_iInstanceVertexStride = sizeof(VTXPARTICLE);
    m_iNumInstance = pData.iNumInstance;
    m_iIndexCountPerInstance = m_iNumIndices;

    //초기값설정
    m_pInstanceVertices = new VTXPARTICLE[m_iNumInstance];
    ZeroMemory(m_pInstanceVertices, sizeof(VTXPARTICLE) * m_iNumInstance);

    m_pSpeeds = new _float[m_iNumInstance];

    for (_uint i = 0; i < m_iNumInstance; ++i)
    {
        // 랜덤 위치
        _float fRandX = m_pGameInstance->Random(pData.vRange.x * -0.5f, pData.vRange.x * 0.5f);
        _float fRandY = m_pGameInstance->Random(pData.vRange.y * -0.5f, pData.vRange.y * 0.5f);
        _float fRandZ = m_pGameInstance->Random(pData.vRange.z * -0.5f, pData.vRange.z * 0.5f);

        m_pInstanceVertices[i].vTranslation = _float4(pData.vCenter.x + fRandX, pData.vCenter.y + fRandY, pData.vCenter.z + fRandZ, 1.f);

        // 초기 크기 (Start Size)
        _float fScale = m_pGameInstance->Random(pData.vSizeRange_Start.x, pData.vSizeRange_Start.y);
        XMStoreFloat4(&m_pInstanceVertices[i].vRight, XMVectorSet(1.f, 0.f, 0.f, 0.f) * fScale);
        XMStoreFloat4(&m_pInstanceVertices[i].vUp, XMVectorSet(0.f, 1.f, 0.f, 0.f) * fScale);
        XMStoreFloat4(&m_pInstanceVertices[i].vLook, XMVectorSet(0.f, 0.f, 1.f, 0.f) * fScale);

        // 기본값들
        m_pInstanceVertices[i].vColor = _float4(1.f, 1.f, 1.f, 1.f);
        m_pInstanceVertices[i].vUV_Info = _float4(0.f, 0.f, 1.f, 1.f);
        m_pInstanceVertices[i].vLifeTime = _float2(0.f, m_pGameInstance->Random(pData.vLifeTimeRange.x, pData.vLifeTimeRange.y));

        //속도 저장용
        m_pSpeeds[i] = m_pGameInstance->Random(pData.vSpeedRange.x, pData.vSpeedRange.y);

    }
    
    D3D11_BUFFER_DESC InstanceBufferDesc{};
    InstanceBufferDesc.ByteWidth = m_iNumInstance * m_iInstanceVertexStride;
    InstanceBufferDesc.Usage = D3D11_USAGE_DYNAMIC;             // [중요] CPU Write
    InstanceBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    InstanceBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE; // [중요] Map 허용
    InstanceBufferDesc.StructureByteStride = m_iInstanceVertexStride;

    D3D11_SUBRESOURCE_DATA InstanceInitialData{};
    InstanceInitialData.pSysMem = m_pInstanceVertices;

    if (FAILED(m_pDevice->CreateBuffer(&InstanceBufferDesc, &InstanceInitialData, &m_pVBInstance)))
        return E_FAIL;

    return S_OK;
}

void CVIBuffer_Particle_Point::Update_Buffer(const vector<tagVertexInstance>& InstanceData)
{
    if (m_pContext == nullptr || m_pVBInstance == nullptr) return;
    if (InstanceData.size() > m_iNumInstance) return;

    D3D11_MAPPED_SUBRESOURCE MappedResource;

    if (InstanceData.size() == 0)
        m_iCurrentNumInstance = 0;

    else
        m_iCurrentNumInstance = InstanceData.size();

    // 1. Map (GPU 메모리 열기, DISCARD로 기존 내용 덮어쓰기)
    if (SUCCEEDED(m_pContext->Map(m_pVBInstance.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource)))
    {
        // 2. Copy (CPU vector -> GPU buffer)
        memcpy(MappedResource.pData, InstanceData.data(), sizeof(VTXPARTICLE) * InstanceData.size());

        // 3. Unmap (닫기)
        m_pContext->Unmap(m_pVBInstance.Get(), 0);
    }
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
    m_pContext->DrawInstanced(1, m_iCurrentNumInstance, 0, 0);

    return S_OK;
}

CVIBuffer_Particle_Point* CVIBuffer_Particle_Point::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
    CVIBuffer_Particle_Point* pInstance = new CVIBuffer_Particle_Point(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
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
