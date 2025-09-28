#include "CLoader.h"
#include "CGameInstance.h"

#include "CBackGround.h"
#include "CTransform.h"

USING(Client)
CLoader::CLoader(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pDeviceContext)
    :m_pDevice{_pDevice},
    m_pDeviceContext{_pDeviceContext},
    m_pGameInstance{CGameInstance::GetInstance()}
{
    Safe_AddRef(m_pGameInstance);
}

_uint APIENTRY ThreadMain(void* pArg)
{
    CheckNullResult(pArg,1);
    CLoader* pLoader = static_cast<CLoader*>(pArg);


    //m_iNextLevelID에 맞춰서 로딩수행
    if (FAILED(pLoader->Loading()))
        return 1;


    return 0;

}

HRESULT CLoader::Initialize(LEVEL_ID iNextLevelID)
{
    //스레드생성
    m_iNextLevelID = iNextLevelID;        //불러올 로딩 타입

    /*스레드 초기화*/
    InitializeCriticalSection(&m_CriticalSection);

                                                //stdcall 전역함수, 쓰레드 진입점함수
                                                            //매개변수로 나 자신을 던진다(함수호출을 위해)
    m_hThred = (HANDLE)_beginthreadex(nullptr, 0, ThreadMain, this, 0, nullptr);
    if (m_hThred == 0)
        return E_FAIL;


    return S_OK;
}

HRESULT CLoader::Loading()
{
    /*나 스레드 진입했어. 다른스레드 못오게 막기.*/
    EnterCriticalSection(&m_CriticalSection);

    /*메인스레드에서 사용했던 COM객체를 사용하는 경우에는 반드시 이 함수를 호출해야한다.*/
    CoInitializeEx(nullptr, 0);

    HRESULT		hr = {};

    switch (m_iNextLevelID)
    {
    case Client::LEVEL_ID::STATIC:

        break;

    case Client::LEVEL_ID::LOGO:
        Loading_Logo();
        break;

    case Client::LEVEL_ID::GAMEPLAY:
        Loading_GamePlay();
        break;

    case Client::LEVEL_ID::END:
        break;

    default:
        break;
    }

    if (FAILED(hr))
        return E_FAIL;

    /*스레드 나감.*/
    LeaveCriticalSection(&m_CriticalSection);
    return S_OK;
}

void CLoader::Output()
{
    SetWindowText(g_hWnd, m_szFPS);
}

HRESULT CLoader::Loading_GamePlay()
{
    lstrcpy(m_szFPS, TEXT("텍스쳐를 로딩 중 입니다."));
    for (size_t i = 0; i < 88899999; i++)
    {
        int a = 10;
    }
    lstrcpy(m_szFPS, TEXT("모델을(를) 로딩 중 입니다."));
    for (size_t i = 0; i < 88889999; i++)
    {
        int a = 10;
    }
    lstrcpy(m_szFPS, TEXT("ㅅㅖ이더을(를) 로딩 중 입니다."));
    for (size_t i = 0; i < 88889999; i++)
    {
        int a = 10;
    }
    lstrcpy(m_szFPS, TEXT("객체원형을(를) 로딩 중 입니다."));

    if (FAILED(m_pGameInstance->Add_Prototype(ENUM_TO_UINT(LEVEL_ID::GAMEPLAY), PROTO_OBJ_NAME(L"BackGround"), CBackGround::Create(m_pDevice, m_pDeviceContext))))
        return E_FAIL;



    m_isFinished = true;
    return S_OK;
}

HRESULT CLoader::Loading_Logo()
{
    lstrcpy(m_szFPS, TEXT("텍스쳐를 로딩 중 입니다."));
  
    lstrcpy(m_szFPS, TEXT("모델을(를) 로딩 중 입니다."));
  
    lstrcpy(m_szFPS, TEXT("ㅅㅖ이더을(를) 로딩 중 입니다."));
  
    lstrcpy(m_szFPS, TEXT("객체원형을(를) 로딩 중 입니다."));

    if (FAILED(m_pGameInstance->Add_Prototype(ENUM_TO_UINT(LEVEL_ID::LOGO), PROTO_OBJ_NAME(L"BackGround"), CBackGround::Create(m_pDevice, m_pDeviceContext))))
        return E_FAIL;


    if (FAILED(m_pGameInstance->Add_Prototype(ENUM_TO_UINT(LEVEL_ID::STATIC), PROTO_COMPONENT_NAME(L"Transform"), CTransform::Create(m_pDevice, m_pDeviceContext))))
        return E_FAIL;


    m_isFinished = true;
    return S_OK;
}


CLoader* CLoader::Create(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pDeviceContext, LEVEL_ID iNextLevelID)
{
    CLoader* pInstance = new CLoader(_pDevice, _pDeviceContext);
    if (FAILED(pInstance->Initialize(iNextLevelID)))
    {
        MSG_BOX("Failed to Create : Loader");
        Safe_Release(pInstance);
       
    }


    return pInstance;
}

void CLoader::Free()
{
    __super::Free();

    WaitForSingleObject(m_hThred, INFINITE);
    DeleteCriticalSection(&m_CriticalSection);

    CloseHandle(m_hThred);

    Safe_Release(m_pGameInstance);
}
