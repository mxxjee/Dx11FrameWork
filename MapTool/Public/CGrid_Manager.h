#pragma once
#include "CBase.h"
#include "CMapGrid.h"

namespace Engine
{
    class CGameInstance;
}

NS_BEGIN(MapTool)

class CGrid_Manager :
    public CBase
{
    DECLARE_SINGLETON(CGrid_Manager)

private:
    CGrid_Manager();
    virtual ~CGrid_Manager() = default;

public:
    HRESULT         Set_MainGrid(CGameObject* pGrid);

    void            Update_Priority(_float fTimeDelta);
    void            Update(_float fTimeDelta);
    void            Update_Late(_float fTimeDelta);
    void            Update_Render(_float fTimeDelta);



    virtual void                Free() override;

public:
    HRESULT         Initialize(ComPtr<ID3D11Device> _pDevice,ComPtr<ID3D11DeviceContext>	_pDeviceContext);

public:

    //픽킹한 삼각형의 점리턴 ( 로컬좌표)
    Triangle* PickTerrain();

    void       Set_MouseWorldPos();
    _float3     Get_MouseWorldPos() { return MouseWorldPos; }
public:
    CMapGrid*           Get_MainGrid() { return m_pMainGrid; }
    _float3             Get_GridPickingWorldPos() { return PickingWolrdPos; }
    bool                IsCollisionWithGrid() { return m_bPicking; }
private:
    ComPtr<ID3D11Device>		m_pDevice = { nullptr };
    ComPtr<ID3D11DeviceContext>	m_pDeviceContext = { nullptr };

private:
    CMapGrid* m_pMainGrid = nullptr;
    CGameInstance* m_pGameInstance = nullptr;

private:
    ENGINE_DESC             m_EngineDesc;
    Triangle                 PickLocalTriangle;
    
    _float3                    PickingWolrdPos;     //픽킹 터레인 마우스 월드좌표
    _float3                     MouseWorldPos;

    bool                    m_bPicking = false;

};
NS_END


