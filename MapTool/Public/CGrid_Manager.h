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
    CMapGrid* Get_MainGrid() { return m_pMainGrid; }

private:
    ComPtr<ID3D11Device>		m_pDevice = { nullptr };
    ComPtr<ID3D11DeviceContext>	m_pDeviceContext = { nullptr };

private:
    CMapGrid* m_pMainGrid = nullptr;
    CGameInstance* m_pGameInstance = nullptr;


};
NS_END


