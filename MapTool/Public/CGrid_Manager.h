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
public:
    CMapGrid* Get_MainGrid() { return m_pMainGrid; }
    _float3             Get_PickingWorldPos() { return PickingWolrdPos; }

private:
    ComPtr<ID3D11Device>		m_pDevice = { nullptr };
    ComPtr<ID3D11DeviceContext>	m_pDeviceContext = { nullptr };

private:
    CMapGrid* m_pMainGrid = nullptr;
    CGameInstance* m_pGameInstance = nullptr;

private:
    ENGINE_DESC             m_EngineDesc;
    Triangle                 PickLocalTriangle;
    _float3                    PickingWolrdPos;     //terrain픽킹한 월드좌표

};
NS_END


