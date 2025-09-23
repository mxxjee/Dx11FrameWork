#include "CLevel_Logo.h"
#include "CLoader.h"
#include "CGameInstance.h"
#include "CLevel_Loading.h"


USING(Client)

CLevel_Logo::CLevel_Logo(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pDeviceContext)
    :CLevel(_pDevice,_pDeviceContext)
{
}


HRESULT CLevel_Logo::Initialize(LevelArgs& args)
{
    __super::Initialize(args);

    return S_OK;
}

HRESULT CLevel_Logo::Update(const _float fTimeDelta)
{
   
    if (GetKeyState(VK_UP) & 0x8000)
    {
        LevelArgs args;
        args.iNextLevelID = ENUM_TO_UINT(LEVEL_ID::GAMEPLAY);
        args.changeType = LEVELCHANGETYPE::OVERLAY;
        args.loadingChangeType = LEVELCHANGETYPE::PUSH;

        if (FAILED(m_pGameInstance->Level_Changer(
            ENUM_TO_UINT(LEVEL_ID::LOADING),
            args)))
            return E_FAIL;
    }
    return S_OK;
}

void CLevel_Logo::Render()
{
    //UI렌더. (로딩바)
    SetWindowText(g_hWnd, L"로고 씬입니다.");

}


CLevel_Logo* CLevel_Logo::Create(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pDeviceContext, LevelArgs& args)
{
    CLevel_Logo* pInstance = new CLevel_Logo(_pDevice, _pDeviceContext);
    if (FAILED(pInstance->Initialize(args)))
    {
        MSG_BOX("Failed to Create : CLevel_Logo");
        Safe_Release(pInstance);
    }


    return pInstance;
}

void CLevel_Logo::Free()
{
    __super::Free();


}
