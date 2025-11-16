#pragma once
#include "CBase.h"

NS_BEGIN(Engine)
class CMapToolCell;
class CNavEditPreview;

class ENGINE_DLL CNavMeshEdit_Manager :
    public CBase
{

    DECLARE_SINGLETON(CNavMeshEdit_Manager)


private:
    explicit CNavMeshEdit_Manager();
    virtual ~CNavMeshEdit_Manager();

private:
    CNavMeshEdit_Manager(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pContext);
  

public:
    //마우스클릭에 따라서 월드좌표를 보내주고 m_points에 저장
    void        Set_DrawPoint(_float3 v, bool bRegister);
    class CNavEditPreview* Get_Preview() { return m_pPreview; }
    HRESULT     Create_MapToolCell(const deque<PreviewPoint>& New);

public:
    void        Update(_float fTimeDelta);
    void        Render();
    void        Clear_Points();

private:
    //Priview의 버퍼 갱신
    void    UpdatePoints();
    deque<PreviewPoint>    Align_CW();     //시계방향으로 정렬


    bool    Check_EmptyPoints();        //Resize되어있으므로, 전부다 0,0,0이면 의미없는값 판정
    
    bool    Check_FullPoints(); //다 채워졌는지 확인한다. (모두 -999가아닌지확인)

    void    Init_Points();  //초기화값 -999로채운다.
public:
    void         Initialize(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pContext);
    vector<_uint>       Get_Edge(const _float3& P, const _float3& A, const _float3& B,const _float3& C);
public:
    void            Free() override;

private:
    int             iDrawIdx = 0;   //클릭한 순서

private:
    deque<PreviewPoint> m_Points; //현재 삼각형을 이루기위한 3개의 점. 삼각형이 생성되면 clear
    vector<CMapToolCell*>      m_pMapToolCells;
    class CNavEditPreview* m_pPreview=nullptr;                         

private:
    ComPtr<ID3D11Device> m_pDevice;
    ComPtr<ID3D11DeviceContext> m_pContext;

private:
    bool    m_bCheckNextEdge = false;

};

NS_END

