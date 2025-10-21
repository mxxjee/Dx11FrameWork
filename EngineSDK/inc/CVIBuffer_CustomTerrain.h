#pragma once
#include "CVIBuffer.h"
#include "VertexData.h"

NS_BEGIN(Engine)
class ENGINE_DLL CVIBuffer_CustomTerrain :
    public CVIBuffer
{


protected:
    CVIBuffer_CustomTerrain(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    CVIBuffer_CustomTerrain(const CVIBuffer_CustomTerrain& Prototype);
    virtual ~CVIBuffer_CustomTerrain() = default;

public:
    virtual         HRESULT     Initialize_Prototype(_uint iNumVerticesX, _uint iNumVerticesZ, _uint Offset);
    virtual         HRESULT     Initialize_Copytype(void* pArg) override;


public:
    HRESULT         CreateVertexBuffer_Begin(_float VertexCountX, _float VertexCountZ, VTXNORTEX** pVertices,D3D11_BUFFER_DESC* pDesc);
    
    //분기처리
    HRESULT         CreateIndexBuffer_Begin(_float VertexCountX, _float VertexCountZ, VTXNORTEX** pVertices,_ushort** pIndices, D3D11_BUFFER_DESC* pDesc);
    HRESULT         CreateIndexBuffer_Begin(_float VertexCountX, _float VertexCountZ, VTXNORTEX** pVertices, _uint** pIndices, D3D11_BUFFER_DESC* pDesc);


    HRESULT         CreateVertexBuffer_End(D3D11_BUFFER_DESC& VertexDesc, VTXNORTEX* pVertices);
    
    //분기처리
    HRESULT         CreateIndexBuffer_End(D3D11_BUFFER_DESC& IndexDesc, _ushort* pIndices);
    HRESULT         CreateIndexBuffer_End(D3D11_BUFFER_DESC& IndexDesc, _uint* pIndices);

  
public:
    HRESULT         Modify_VertexBuffer_Begin(D3D11_MAPPED_SUBRESOURCE* mapped, _float VertexCountX, _float VertexCountZ);
    HRESULT         Modify_IndexBuffer_Begin(D3D11_MAPPED_SUBRESOURCE* mapped, _float VertexCountX, _float VertexCountZ);

    HRESULT         Modify_VertexBuffer_End(D3D11_BUFFER_DESC& VertexDesc, VTXNORTEX* pVertices);
    HRESULT         Modify_IndexBuffer_End(D3D11_BUFFER_DESC& IndexDesc, _ushort* pIndices);

public:
    HRESULT         ResizeBuffer(_float fNewVertexCountX, _float fNewVertexCountZ);

public:
    _uint           Get_NumVerticesX() { return m_iNumVerticesX; }
    _uint           Get_NumVerticesZ() { return m_iNumVerticesZ; }

private:
    _uint       m_iNumVerticesX;        //가로정점개수
    _uint       m_iNumVerticesZ;        //가로정점개수
    _uint       m_iOffSet = 1;

public:
    static CVIBuffer_CustomTerrain* Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, _uint iNumVerticesX=10, _uint iNumVerticesZ=10, _uint Offset=1);
    virtual CComponent* Clone(void* pArg);
    virtual void Free() override;

};
NS_END

