#pragma once
#include "CBase.h"

NS_BEGIN(Engine)
class CVIBuffer_Triangle;
class CGameInstance;

class ENGINE_DLL CMapToolCell :
    public CBase
{
public:
    typedef struct tagMapToolCellDesc
    {
        void* TriangleCom = nullptr;
        _uint iIdx = 0;

    }MAPTOOLCELL_DESC;
private:
    CMapToolCell(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    virtual ~CMapToolCell() = default;

private:
    _float3         m_vPoints[ENUM_TO_UINT(POINTType::END)] = {};   //각 정점의 좌표(월드)
    _float3         m_vNormals[ENUM_TO_UINT(LINE::END)] = {};   //각 선분에 대한 법선벡터
    _int            m_iNeighbors[ENUM_TO_UINT(LINE::END)] = { -1,-1,-1 };   //각 선분에 대해서 인접한 삼각형의 인덱스(존재하지 않으면 -1로 채움)
    _uint           m_iIndex;           //현재 이 삼각형이 네브메쉬 배열의 몇번째 인덱스인가요


public:
    HRESULT Initialize_Prototype(void* pArg);
    HRESULT     Ready_Components(void* pArg);

public:
    HRESULT      Render();

private:
    ComPtr<ID3D11Device> m_pDevice = { nullptr };
    ComPtr<ID3D11DeviceContext> m_pContext = { nullptr };

public:                             //생성함수, 각 정점의 위치3개, 인덱스
    static CMapToolCell* Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, void* pArg);
    virtual void Free() override;

private:
    CVIBuffer_Triangle* m_pVIBufferCom = { nullptr };
    _float4x4   WorldMatrix;
    CGameInstance* m_pGameInstance = nullptr;
    class CShader* m_pShader = nullptr;


private:
    ComPtr<ID3D11RasterizerState> m_pWireframeRS = nullptr;

};

NS_END
