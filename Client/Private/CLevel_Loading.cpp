#include "CLevel_Loading.h"
#include "CLoader.h"
#include "CGameInstance.h"

#include "CLevel_Town.h"
#include "CLevel_Logo.h"

#include "CInput_Manager.h"



USING(Client)

CLevel_Loading::CLevel_Loading(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pDeviceContext)
    :CLevel(_pDevice,_pDeviceContext)
{
}

HRESULT CLevel_Loading::Initialize(LEVEL_ID iLevelID, LEVELCHANGETYPE eChangeType, LevelArgs& args)
{
    /*로더 : 로딩을 위한 서브스레드를 생성하고 실제 로딩 수행.*/
    //로더 생성
    
    __super::Initialize(args);

    m_pLoader = CLoader::Create(m_pDevice, m_pContext, iLevelID);
    CheckNullResult(m_pLoader, E_FAIL);

    m_eNextLevelID = iLevelID;
    m_eChangeType = eChangeType;

    Set_Flag(LEVELFLAG::TRANSIENT);

    //로딩할동안 보여줄 UI생성
    if (FAILED(Ready_UI_Layer()))
        return E_FAIL;

    return S_OK;
}

void CLevel_Loading::Update_Priority(_float fTimeDelta)
{
    __super::Update_Priority(fTimeDelta);
    if (m_pLoader->IsFinished() &&
        CInput_Manager::GetInstance()->IsKeyPressed(KeyCode::Enter))
    {
        LevelArgs args;
        args.changeType = m_eChangeType;
        args.m_iLevelID = ENUM_TO_UINT(m_eNextLevelID);


        if (SUCCEEDED(m_pGameInstance->Level_Changer(ENUM_TO_UINT(m_eNextLevelID), args)))
            return;

        MSG_BOX("다음 씬 불러오기 실패");
    }
}

void CLevel_Loading::Update(const _float fTimeDelta)
{
    __super::Update(fTimeDelta);

    /*로더야 로딩 다 되었니?*/
    CheckNull(m_pLoader);

   

}

void CLevel_Loading::Update_Late(_float fTimeDelta)
{
    __super::Update_Late(fTimeDelta);

}

void CLevel_Loading::Render()
{
    //UI렌더. (로딩바)
    CheckNull(m_pLoader);
    m_pLoader->Output();

}



HRESULT CLevel_Loading::Ready_UI_Layer()
{
    //UI 요소 추가.

    return S_OK;
}

void CLevel_Loading::OnEnter()
{
    int A = 0;
}

void CLevel_Loading::OnResume()
{
    int A = 0;
}

void CLevel_Loading::OnPause()
{
    int A = 0;

}

void CLevel_Loading::OnExit()
{
    int A = 0;

}


CLevel_Loading* CLevel_Loading::Create(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pDeviceContext, LevelArgs& args)
{
    CLevel_Loading* pInstance = new CLevel_Loading(_pDevice, _pDeviceContext);

    if (FAILED(pInstance->Initialize(
        static_cast<LEVEL_ID>(args.iNextLevelID), 
        static_cast<LEVELCHANGETYPE>(args.changeType), args
    )))
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
