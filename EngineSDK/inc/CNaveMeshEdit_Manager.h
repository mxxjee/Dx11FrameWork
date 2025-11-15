#pragma once
#include "CBase.h"

NS_BEGIN(Engine)
class CMapToolCell;
class CNavEditPreview;

class ENGINE_DLL CNaveMeshEdit_Manager :
    public CBase
{

    DECLARE_SINGLETON(CNaveMeshEdit_Manager)

private:
    explicit CNaveMeshEdit_Manager();
    virtual ~CNaveMeshEdit_Manager();

private:
    CNaveMeshEdit_Manager(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pContext);
  

public:
    //마우스클릭에 따라서 월드좌표를 보내주고 m_points에 저장
    void        Set_DrawPoint(_float3 v,int idx);


public:
    void        Update(_float fTimeDelta);
    void        Render();


private:
    //Priview의 버퍼 갱신
    void    UpdatePoints();

public:
    void         Initialize(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pContext);
private:
    vector<_float3> m_Points; //현재 삼각형을 이루기위한 3개의 점. 삼각형이 생성되면 clear
    vector<CMapToolCell*>      m_pMapToolCells;

    class CNavEditPreview* m_pPreview=nullptr;

};

NS_END

