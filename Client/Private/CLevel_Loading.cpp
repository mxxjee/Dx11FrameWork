#include "CLevel_Loading.h"
#include "CLoader.h"
#include "CGameInstance.h"

#include "CLevel_GamePlay.h"
#include "CLevel_Logo.h"



USING(Client)

CLevel_Loading::CLevel_Loading(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pDeviceContext)
    :CLevel(_pDevice,_pDeviceContext)
{
}

HRESULT CLevel_Loading::Initialize(LEVEL_ID iLevelID, LEVELCHANGETYPE eChangeType)
{
    /*로더 : 로딩을 위한 서브스레드를 생성하고 실제 로딩 수행.*/
    //로더 생성
    m_pLoader = CLoader::Create(m_pDevice, m_pDeviceContext, iLevelID);
    CheckNullResult(m_pLoader, E_FAIL);

    m_eNextLevelID = iLevelID;
    m_eChangeType = eChangeType;

    Set_Flag(LEVELFLAG::TRANSIENT);

    //로딩할동안 보여줄 UI생성
    if (FAILED(Ready_UI_Layer()))
        return E_FAIL;

    return S_OK;
}

HRESULT CLevel_Loading::Update(const _float fTimeDelta)
{
    /*로더야 로딩 다 되었니?*/
    CheckNullResult(m_pLoader,E_FAIL);

    if (m_pLoader->IsFinished() &&
        GetKeyState(VK_RETURN) & 0x8000)
    {
        CLevel* pNewLevel = nullptr;
        switch (m_eNextLevelID)
        {
        case LEVEL_ID::LOGO:
            pNewLevel = CLevel_Logo::Create(m_pDevice, m_pDeviceContext);

            break;

        case LEVEL_ID::GAMEPLAY:
            pNewLevel = CLevel_GamePlay::Create(m_pDevice, m_pDeviceContext);

            //pNewLevel=
            break;
        }


        CheckNullResult(pNewLevel,E_FAIL);
        if (SUCCEEDED(m_pGameInstance->Level_Changer(ENUM_TO_UINT(m_eNextLevelID), pNewLevel, m_eChangeType)))
            return E_FAIL;

        MSG_BOX("다음 씬 불러오기 실패");
    }

    return S_OK;
}

void CLevel_Loading::Render()
{
    //UI렌더. (로딩바)
    CheckNull(m_pLoader);
    m_pLoader->Output();

}

void CLevel_Loading::Clear()
{
    /*뭔가 클리어할게있남*/
}

HRESULT CLevel_Loading::Ready_UI_Layer()
{
    //UI 요소 추가.

    return S_OK;
}


CLevel_Loading* CLevel_Loading::Create(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pDeviceContext, LEVEL_ID iLevelID, LEVELCHANGETYPE eChangeType)
{
    CLevel_Loading* pInstance = new CLevel_Loading(_pDevice, _pDeviceContext);
    if (FAILED(pInstance->Initialize(iLevelID, eChangeType)))
    {
        MSG_BOX("Failed to Create : Level_Loading");
        Safe_Release(pInstance);
    }


    return pInstance;
}

void CLevel_Loading::Free()
{
    __super::Free();
    Safe_Release(m_pLoader);


}
