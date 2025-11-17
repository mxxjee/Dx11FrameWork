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
    virtual HRESULT Initialize_Prototype(const _tchar* pNavigationData);
    virtual HRESULT Initialize_Copytype(void* pArg) override;

public:
    _bool isMove(_fvector vResultPos);

private:
    HRESULT             SetUp_Neighbors();


private:
    vector<class CCell*>        m_Cells;//삼각형 배열
    _uint               m_iNumCells = {};//
    _int                m_iCurrentCellIndex;    //현재 오너가 위치한 네브메쉬 셀 중 몇번ㅉ떄?

private:
    static const                    _float4x4* m_pParentMatrix;   //m_Cells의 worldMatrix
																//cell의 각 정점은 이미 월드상의 점이다.
										//만약 지형이 움직이거나 할경우 cell이 이 위치를 따라가야하므로 부모행렬을 정해준다.
										//이는 static으로 사용하므로 이 클래스를 사용하는 모든 객체가 같은 부모행렬을 적용하게된다.

#ifdef _DEBUG
private:
    class CShader* m_pShader = { nullptr };

#endif
public:
    static CNavigation* Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, const _tchar* pNavigationData);
    virtual CComponent* Clone(void* pArg) override;
    virtual void Free() override;
};

NS_END