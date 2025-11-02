#include "CGrid_Manager.h"
#include "CGameInstance.h"
#include "Client_Defines.h"


USING(MapTool)
IMPLEMENT_SINGLETON(CGrid_Manager)

CGrid_Manager::CGrid_Manager()
    :m_pGameInstance(CGameInstance::GetInstance())
{
    Safe_AddRef(m_pGameInstance);
}
HRESULT CGrid_Manager::Set_MainGrid(CGameObject* pGrid)
{
    CheckNullResult(pGrid, E_FAIL);

    CMapGrid* ppGrid = dynamic_cast<CMapGrid*>(pGrid);
    CheckNullResult(ppGrid, E_FAIL);

    m_pMainGrid = ppGrid;
    return S_OK;
}

void CGrid_Manager::Update_Priority(_float fTimeDelta)
{
    CheckNull(m_pMainGrid);
    m_pMainGrid->Update_Priority(fTimeDelta);

}

void CGrid_Manager::Update(_float fTimeDelta)
{
    CheckNull(m_pMainGrid);
    m_pMainGrid->Update(fTimeDelta);

}

void CGrid_Manager::Update_Late(_float fTimeDelta)
{
    CheckNull(m_pMainGrid);
    m_pMainGrid->Update_Late(fTimeDelta);
}

void CGrid_Manager::Update_Render(_float fTimeDelta)
{
    CheckNull(m_pMainGrid);
    m_pGameInstance->Add_RenderObject(ENUM_TO_UINT(RENDERGROUP::PRIORITY), m_pMainGrid);
}

void CGrid_Manager::Free()
{
    Safe_Release(m_pGameInstance);
    Safe_Release(m_pMainGrid);

}
