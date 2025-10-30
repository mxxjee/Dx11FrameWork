#pragma once
#include "CVIBuffer.h"
#include "VertexData.h"

NS_BEGIN(Engine)
class ENGINE_DLL CVIBuffer_Model:
    public CVIBuffer
{

protected:
    CVIBuffer_Model(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    CVIBuffer_Model(const CVIBuffer_Model& Prototype);
    virtual ~CVIBuffer_Model() = default;

public:
    virtual         HRESULT     Initialize_Prototype(_matrix& Matrix, vector<VTXMESH>& Vertices, vector<_uint>& Indices);
    virtual         HRESULT     Initialize_Copytype(void* pArg) override;



public:
    static CVIBuffer_Model* Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, _matrix& Matrix,vector<VTXMESH>& Vertices, vector<_uint>& Indices);
    virtual CComponent* Clone(void* pArg);
    virtual void Free() override;
};

NS_END

