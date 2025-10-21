#pragma once
#include "CVIBuffer.h"

/*높이 맵을 이용해서 정점을 생성하고 높이를 결정할거다.
높이맵 텍스처의 크기 = 정점 개수
*/
NS_BEGIN(Engine)
class ENGINE_DLL CVIBuffer_Terrain final:
    public CVIBuffer
{
protected:
    CVIBuffer_Terrain(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    CVIBuffer_Terrain(const CVIBuffer_Terrain& Prototype);
    virtual ~CVIBuffer_Terrain() = default;

public:
    virtual         HRESULT     Initialize_Prototype(const _tchar* pHeightFileMapPath);
    virtual         HRESULT     Initialize_Copytype(void* pArg) override;


private:
    _uint       m_iNumVerticesX = 0;        //가로정점개수
    _uint       m_iNumVerticesZ = 0;        //가로정점개수

public:
    static CVIBuffer_Terrain* Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, const _tchar * pHeightFileMapPath);
    virtual CComponent* Clone(void* pArg);
    virtual void Free() override;
};
NS_END
