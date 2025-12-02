#pragma once
#include "CComponent.h"

/*내브메쉬들을 저장(Cell이라고부름)
현재 플레이어가 어느 삼각형에있는지 인덱스를 기록한다.*/
NS_BEGIN(Engine)
class CShader;

class ENGINE_DLL CNavigation :
    public CComponent
{

private:
    CNavigation(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    CNavigation(const CNavigation& Prototype);
    virtual ~CNavigation() = default;



public:
    void        Set_CurrentIdx(_vector vWorldPos);       //현재 위치기반으로 어느셀에있는지 정해준다.
    virtual HRESULT Initialize_Prototype();
    virtual HRESULT Initialize_Copytype(void* pArg) override;

public:
    class CCell* Get_Cell(_uint i) { return (*m_Cells)[i]; }
public:
    class CCell* Get_CurrentCell() { return (*m_Cells)[m_iCurrentCellIndex]; }
    _uint       Get_PreCellIdx() { return m_iPreCellIdx; }
public:
    _bool isMove(_fvector vResultPos);
    _bool CheckGround(_fvector& vResultPos,_float& vOutGroundPosY);      //마지막으로 측정된 CurIdx기준으로 현재 평면위에있는가.

    _vector SetUp_OnNavigation(_fvector vWorldPos);     //네브메쉬 평면을 타게함.
    _uint    Get_CurrentCellType();
public:
    const list<_vector>* Make_Route(_int iGoalIndex);
    _bool       CanPush(_int iCellIndex);

#ifdef _DEBUG
public:
    HRESULT Render();   //현재 내가 있는 위치의 셀을 표시한다.
private:
    _float4         g_Color = _float4(1.f, 0.f, 0.f, 1.f);
    class CShader*  m_pShader = nullptr;


#endif
private:
    vector<class CCell*>*        m_Cells=nullptr;//삼각형 배열
    _uint                       m_iNumCells = {};//
    _int                        m_iCurrentCellIndex=-1;    //현재 오너가 위치한 네브메쉬 셀 중 몇번ㅉ떄?

private:
    _float4x4* m_pParentMatrix=nullptr;   //m_Cells의 worldMatrix
																//cell의 각 정점은 이미 월드상의 점이다.
										//만약 지형이 움직이거나 할경우 cell이 이 위치를 따라가야하므로 부모행렬을 정해준다.
										
public:
    static CNavigation* Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    virtual CComponent* Clone(void* pArg) override;
    virtual void Free() override;


private:
    _int            m_iPreCellIdx=-1;

private:
    /*A-Star 사용시 필요한 변수들*/
    _int            m_iOldGoalIndex = { -1 };
    list <_int>     m_OpenList;         //탐색하기 위한 노드를담는다.
    list<_int>      m_CloseList;        //이미 탐색한 노드들을 담는다.

    list<_int>      m_Route;        //인덱스를 통해 만들어낸 경로.


};

NS_END