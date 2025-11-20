#pragma once
#include "CBase.h"

//네브메쉬 삼각형 하나를 의미한다.
//정점3개 저장한다.
//내가 몇번인덱스인지 저장한다.
//인접한 삼각형을 저장한다.

NS_BEGIN(Engine)
class CVIBuffer_Triangle;

class ENGINE_DLL CCell :
    public CBase
{

private:
    CCell(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    virtual ~CCell() = default;

public:
    _vector     Get_Point(POINTType ePoint) const { return XMLoadFloat3(&m_CellInfo.m_vPoints[ENUM_TO_UINT(ePoint)]); }
    int         Get_Idx() { return m_CellInfo.m_iIndex; }
    
    void        Get_Neighbors(_int* Output) { memcpy(Output, m_CellInfo.m_iNeighbors, sizeof(_int) * 3); }
    _float      Compute_Height(_vector vCellTargetPos);

                //셀 스페이상의 위치가 셀안에있는지 판단, 없으면 더 나아갈 수 잇는지 판단하기 위해 이웃을 리턴.
    bool        isIn(_fvector vResultPos, _int* pNeighborIndex);
    void        Set_ParentIndex(_int iParentIndex) { m_iParentIndex = iParentIndex; }

    _bool       CanPush(_int iCellIndex);
public:
    /*Using A-Star*/
                //휴리스틱 비용 구하기
    _float      Compute_Cost(const vector<CCell*>& Cells, _int iGoalIndx);
    _vector     Get_CenterPos();

private:
    DefaultCellInfo         m_CellInfo;

public:
    HRESULT Initialize_Prototype(const DefaultCellInfo& Info);
    HRESULT Ready_Components();

private:
    ComPtr<ID3D11Device> m_pDevice = { nullptr };
    ComPtr<ID3D11DeviceContext> m_pContext = { nullptr };

#ifdef _DEBUG
public:
    void    Update_Render();
    HRESULT Render();
private:
    HRESULT Create_WireFrameRS();
    ComPtr<ID3D11RasterizerState> m_pWireframeRS = nullptr;
    CVIBuffer_Triangle* m_pVIBufferCom = { nullptr };

#endif
public:                             //맵툴에서 파싱해온 구조체만 넘겨주기
    static CCell* Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, const DefaultCellInfo& Info);
    virtual void Free() override;

private:
    class CGameInstance* m_pGameInstance = nullptr;


private:
    /*A-star알고리즘을 사용하기 위한 변수들*/
    _float			m_fTotalCost;
    int             m_iParentIndex = -1;

};

NS_END

