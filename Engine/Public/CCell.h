#pragma once
#include "CBase.h"

//네브메쉬 삼각형 하나를 의미한다.
//정점3개 저장한다.
//내가 몇번인덱스인지 저장한다.
//인접한 삼각형을 저장한다.
NS_BEGIN(Engine)
class ENGINE_DLL CCell :
    public CBase
{

private:
    CCell(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    virtual ~CCell() = default;

public:
    _vector     Get_Point(POINTType ePoint) const { return XMLoadFloat3(&m_vPoints[ENUM_TO_UINT(ePoint)]); }
    bool        Compare(_fvector vTargetPointA, _fvector vTargetPointB);
    void        Set_Neighbor(LINE eLine, CCell* pCell) { m_iNeighbors[ENUM_TO_UINT(eLine)] = pCell->m_iIndex; }

private:
    _float3         m_vPoints[ENUM_TO_UINT(POINTType::END)] = {};   //각 정점의 좌표(월드)
    _float3         m_vNormals[ENUM_TO_UINT(LINE::END)] = {};   //각 선분에 대한 법선벡터
    _int            m_iNeighbors[ENUM_TO_UINT(LINE::END)] = { -1,-1,-1 };   //각 선분에 대해서 인접한 삼각형의 인덱스(존재하지 않으면 -1로 채움)
    _uint           m_iIndex;           //현재 이 삼각형이 네브메쉬 배열의 몇번째 인덱스인가요

public:
    HRESULT Initialize_Prototype(const _float3* pPoints, _int iIndex);

public:
    _bool isIn(_fvector vResultPos);

private:
    ComPtr<ID3D11Device> m_pDevice = { nullptr };
    ComPtr<ID3D11DeviceContext> m_pContext = { nullptr };

public:                             //생성함수, 각 정점의 위치3개, 인덱스
    static CCell* Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, const _float3* pPoints, _int iIndex);
    virtual void Free() override;

};
NS_END

