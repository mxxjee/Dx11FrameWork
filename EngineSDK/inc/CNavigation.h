#pragma once
#include "CComponent.h"

/*내브메쉬들을 저장(Cell이라고부름)
현재 플레이어가 어느 삼각형에있는지 인덱스를 기록한다.*/
NS_BEGIN(Engine)
class ENGINE_DLL CNavigation :
    public CComponent
{

private:
    CNavigation(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    CNavigation(const CNavigation& Prototype);
    virtual ~CNavigation() = default;



public:
    virtual HRESULT Initialize_Prototype(const _tchar* pNavigationData);
    virtual HRESULT Initialize_Copytype(void* pArg) override;


private:
    HRESULT             SetUp_Neighbors();


private:
    vector<class CCell*>        m_Cells;//삼각형 배열
    _uint               m_iNumCells = {};//
    _int                m_iCurrentCellIndex;    //현재 오너가 위치한 네브메쉬 셀 중 몇번ㅉ떄?

public:
    static CNavigation* Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, const _tchar* pNavigationData);
    virtual CComponent* Clone(void* pArg) override;
    virtual void Free() override;
};

NS_END