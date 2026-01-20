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
    virtual         HRESULT     Initialize_Prototype(_matrix& Matrix, const string& VBPath, const string& IBPath, MODEL eModelType);
    virtual         HRESULT     Initialize_Copytype(void* pArg) override;
    
private:
    HRESULT                  Initialize_AnimMeshVertexBuffer(_matrix& Matrix, const string& Path);
    HRESULT                  Initialize_StaticMeshVertexBuffer(_matrix& Matrix, const string& Path);


public:
    static CVIBuffer_Model* Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, _matrix& Matrix, const string& VBPath, const string& IBPath, MODEL eModelType);
    virtual CComponent* Clone(void* pArg);
    virtual void Free() override;
};

NS_END

