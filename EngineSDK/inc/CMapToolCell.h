#pragma once
#include "CBase.h"
#include "IMapEditable.h"

NS_BEGIN(Engine)
class CVIBuffer_Triangle;
class CGameInstance;

class ENGINE_DLL CMapToolCell :
    public CBase, public IMapEditable
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
    MapToolCellInfo          m_CellInfo;


public:
    HRESULT                Set_WireFrameMode();
    HRESULT                Set_SolidMode();
public:
    /*정렬되기 전 원본의 점 = PreviewPoints*/
    void                Set_PreviewPoints(const deque<PreviewPoint>& New);
    PreviewPoint*        Get_PreviewPoints() { return m_PreviewPoints; }
private:
    PreviewPoint        m_PreviewPoints[ENUM_TO_UINT(POINTType::END)];

public:
    HRESULT Initialize_Prototype(void* pArg);
    void    UpdatePoints(deque<PreviewPoint> Points);
    HRESULT     Ready_Components(void* pArg);
    HRESULT     Ready_Resource(void* pArg);
public:
    HRESULT      Render();

public:
    _float3* Get_vPoints() { return m_CellInfo.m_vPoints;}
    _vector Get_vPoint(POINTType eType) { return XMLoadFloat3(&m_CellInfo.m_vPoints[ENUM_TO_UINT(eType)]); }

    void                Set_Index(int idx) { m_CellInfo.m_iIndex = idx; }
    const MapToolCellInfo& Get_CellInfo() { return m_CellInfo; }

private:
    ComPtr<ID3D11Device> m_pDevice = { nullptr };
    ComPtr<ID3D11DeviceContext> m_pContext = { nullptr };

public:                             //생성함수, 각 정점의 위치3개, 인덱스
    static CMapToolCell* Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, void* pArg);
    virtual void Free() override;

private:
    HRESULT Create_WireFrameRS();
    HRESULT Create_SolidRS();
private:
    CVIBuffer_Triangle* m_pVIBufferCom = { nullptr };
    _float4x4   WorldMatrix;
    CGameInstance* m_pGameInstance = nullptr;
    class CShader* m_pShader = nullptr;

public:
    // IMapEditable을(를) 통해 상속됨
    virtual void Imgui_Render_Properties(_float3* vScale, _float3* vPosition, _float3* vRotation) override;
    virtual void Edit_Move(DIRECTION eDir, float fSpeed, float _fTimeDelta) {};
    virtual void Fix_Y(_float Y){};

public:
    void        Set_Active(bool b) { m_bActive = b; }
    bool         Is_Active() { return m_bActive; }
public:
    bool        Compare(_vector PointA, _vector PointB);
    void        Set_Neighbor(LINE eLine, CMapToolCell* pCell);
    virtual     void Update_SelectMode(float _fTimeDelta);

private:
    ComPtr<ID3D11RasterizerState> m_pCurrentRS = nullptr;
    ComPtr<ID3D11RasterizerState> m_pWireframeRS = nullptr;
    ComPtr<ID3D11RasterizerState> m_pSolidRS = nullptr;


    // IMapEditable을(를) 통해 상속됨
    virtual void OnSeletected(bool bSelected) override;

    virtual void Save_To_Json(json& Json) override;

    virtual void Show_Gizmo() override;

public:
    void        Set_Color(_float4 color) { g_Color = color; }
private:
    const float EPS = 0.01f;
    bool            m_bActive = true;

private:
    _float4     g_Color;
};

NS_END
