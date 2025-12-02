#pragma once
#include "CBase.h"
#include "CMapToolCell.h"

NS_BEGIN(Engine)
class CMapToolCell;
class CNavEditPreview;

class ENGINE_DLL CNavMeshEdit_Manager :
    public CBase
{
public:
    struct CompareKey
    {
        bool operator()(const _float& a, const _float& b) const
        {
            return a < b;
        }
    };

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
    HRESULT     Create_MapToolCell(const deque<PreviewPoint>& New, const deque<PreviewPoint>& Origin);

    void        Ctrl_Z();
public:
    void        Update(_float fTimeDelta);
    void        Update_Late(_float fTimeDelta);
    void        Render();
    void        Clear_Points();

private:
    //Priview의 버퍼 갱신
    void    UpdatePoints();
    deque<PreviewPoint>    Align_CW(const deque<PreviewPoint>& Origin);     //시계방향으로 정렬


    bool    Check_EmptyPoints(const  deque<PreviewPoint>& Points);        //Resize되어있으므로, 전부다 0,0,0이면 의미없는값 판정
    bool    Check_EmptyPoints(PreviewPoint* Points);        //Resize되어있으므로, 전부다 0,0,0이면 의미없는값 판정
    
    bool    Check_FullPoints(const  deque<PreviewPoint>& Points); //다 채워졌는지 확인한다. (모두 -999가아닌지확인)

    void    Init_Points();  //초기화값 -999로채운다.
    
    void    Modify_Triangle();//삼각형수정모드..
    void    Rebuild_Cell(CMapToolCell* pCell,deque<PreviewPoint>& NewPoints);

private:
    CMapToolCell* Find_NeareastCell(const _float3& vMousePos);
    _float3       Find_NeareastPos(const _float3 vPos, const _float fRadius, const _float3& A, const _float3& B, const _float3& C);

public:
    void         Initialize(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pContext);
    vector<_uint>       Get_Edge(const _float3& P, const _float3& A, const _float3& B,const _float3& C);
    
    void                Set_FixCell(bool b) { m_bFixCell = b; }
    void                Set_FixEdge(bool b) { m_bFixEdge = b; }

public:
    void            Free() override;
    HRESULT         Save_NavigationData(const string& filePath,_uint iNum=0);
    HRESULT         Load_NavigationData(const string& filePath);

public:
    void            Show_Solo_Cell(bool bToggle); 
private:
    int             iDrawIdx = 0;   //클릭한 순서
    int             iRestoreIdx = 1;    //ctrl+z 수행시, 삼각형점ㅈ에서 꺼내올인덱스
private:
    deque<PreviewPoint>         m_Points; //현재 삼각형을 이루기위한 3개의 점. 삼각형이 생성되면 clear
    vector<CMapToolCell*>      m_pMapToolCells;
    class CNavEditPreview* m_pPreview=nullptr;                         
    deque<PreviewPoint>    m_PrePoints;

public:                                                  
    HRESULT     SetUp_Neighbors();
    void        SetUp_Planes();

public:
    void        RequestDestory(CMapToolCell* pObj);
    void        ProcessDestroy();
public:
    const vector<CMapToolCell*>& Get_MapToolCells() { return m_pMapToolCells; }
    CMapToolCell* Get_MapToolCell(_uint idx) { if (idx > m_pMapToolCells.size() - 1)return nullptr; return m_pMapToolCells[idx]; }
    HRESULT     Render_Preview_Imgui_Render();

private:
    ComPtr<ID3D11Device> m_pDevice;
    ComPtr<ID3D11DeviceContext> m_pContext;

private:
    queue<CMapToolCell*>    m_DestroyQueue;

private:
    bool    m_bCheckNextEdge = false;

    bool    m_bFixCell = false;
    bool    m_bFixEdge = false;
    bool    m_bClear = false;
    bool    m_bRestore = false;
    bool    m_bRestoreCell = false;


    CMapToolCell* pTargetCell = nullptr;
    PreviewPoint* pTargetPreviewPoints = nullptr;
    vector<_uint> LastIdx;
 
};

NS_END

