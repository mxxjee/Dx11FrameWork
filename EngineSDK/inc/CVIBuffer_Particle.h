#pragma once
#include "CVIBuffer.h"
#include "VertexData.h"

/*파티클을 생성하기 위한 인스턴스 버퍼를 사용하는 버퍼의 부모
 Map/UnMap을 통해 행렬을 갱신시켜 움직임을 나타낸다.


 사본 생성할때 진짜 버퍼를 만든다.
*/
NS_BEGIN(Engine)
class ENGINE_DLL CVIBuffer_Particle abstract:
    public CVIBuffer
{
public:
    typedef struct tagVIBuffer_Particle_Desc
    {
        bool        isLoop = false;
        _uint       iNumInstance = {};      //생성할 입자개수]
        _float3 vRange = { 1.f, 1.f, 1.f };     //파티클 생성범위

        _float3 vCenter = { 0.f,0.f,0.f };

        _float2 vSizeRange_Start = { 0.1f,0.3f };//생성될 때 크기 (Min, Max)
        _float2 vSizeRange_End = { 0.5f, 0.8f };   // 죽을 때 크기 (Min, Max)


        _float2 vSpeedRange = { 1.f, 5.f };    // 파티클 이동속도(Min,Max)
        _float2 vLifeTimeRange = { 1.f, 2.f }; // 수명(Min,Max)
     
    }PARTICLE_DESC;


protected:
    CVIBuffer_Particle(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    CVIBuffer_Particle(const CVIBuffer_Particle& Prototype);
    virtual ~CVIBuffer_Particle() = default;

public:
    virtual         HRESULT     Initialize_Prototype(const _tchar* pHeightFileMapPath);
    virtual         HRESULT     Initialize_Copytype(void* pArg) override;

                    //인스턴싱의 렌더방식과 바인드 방식이 다르므로 오버라이드
    virtual         HRESULT     Bind_Resource();
    virtual         HRESULT     Render();

public:
    void        Drop(_float fTimeDelta);        //파티클을 위에서 아래로 내려오게하는 기능함수
    void        Spread(_float fTimeDelta);
protected:
    ComPtr<ID3D11Buffer>        m_pVBInstance = { nullptr };        //추가적인 버퍼. 
    D3D11_BUFFER_DESC           m_InstanceBufferDesc = {};


protected:
    VTXPARTICLE*            m_pInstanceVertices = { nullptr };  //초기값들을 저장한다.
    _uint                   m_iIndexCountPerInstance = {};      //인스턴스 하나 당의 인덱스개수
    _uint                   m_iNumInstance = {};                //인스턴스 개수
    _uint                   m_iInstanceVertexStride = {};       //인스턴스 하나당 버텍스size
    _float*                 m_pSpeeds = { nullptr };
    _bool                  m_isLoop = { false };
    _float3                 m_vPivot = {};


public:
    virtual CComponent* Clone(void* pArg)=0;
    virtual void Free() override;



};

NS_END