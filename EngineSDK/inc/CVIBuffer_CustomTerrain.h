#pragma once
#include "CVIBuffer.h"

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

