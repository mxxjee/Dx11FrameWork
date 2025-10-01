#pragma once
#include "CComponent.h"

NS_BEGIN(Engine)
class ENGINE_DLL CVIBuffer :
    public CComponent
{
protected:
    CVIBuffer(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    CVIBuffer(const CVIBuffer& Prototype);
    virtual ~CVIBuffer() = default;

public:
    virtual         HRESULT     Initialize_Prototype() override;
    virtual         HRESULT     Initialize_Copytype(void* pArg) override;
    virtual         HRESULT     Bind_Resource();
    virtual         HRESULT     Render();
public:
    virtual CComponent* Clone(void* pArg) = 0;
    virtual void Free() override;

protected:
    ComPtr<ID3D11Buffer>        m_pVB;
    ComPtr<ID3D11Buffer>        m_pIB;



protected:
    _float3*        m_pVertexPositions = { nullptr };       //정점의 위치들만 기록하기 위한 동적배열, 얕은 참조 수행하므로 원본일 때만 메모리반환

    _uint           m_iNumVertices = {};           //정점 개수
    _uint           m_iVertexStride = {};       //정점 구조체 하나의 크기

    _uint           m_iNumIndices = {};            //인덱스 개수
    _uint           m_iIndexStride = {};        //인덱스 하나 size

    _uint           m_iNumVertexBuffers = {};
    _uint            m_iNumIndexBuffers = {};

    D3D11_PRIMITIVE_TOPOLOGY    m_ePrimitiveType=D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
};
NS_END

