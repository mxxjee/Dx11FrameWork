#include "CLoader.h"
#include "CGameInstance.h"

#include "CBackGround.h"
#include "CPlayer.h"
#include "CFloor.h"
#include "CMainCamera.h"
#include "CUICamera.h"
#include "CFreeCamera.h"
#include "CScreenQuad.h"
#include "CMinimapCamera.h"
#include "CMinimapQuad.h"
#include "CTerrain.h"

#include "CTexture.h"
#include "CShader.h"

#include "CTransform.h"
#include "CPerspectiveCameraComponent.h"
#include "COrthographicCameraComponent.h"

#include "CVIBuffer_Rect.h" 
#include "CVIBuffer_Terrain.h"

#include "CModel.h"
#include "CModelObject.h"




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
    HRESULT hr=CoInitializeEx(nullptr, 0);

    hr = {};

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

    case LEVEL_ID::UI:
        Loading_UI();
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

   

    //if (FAILED(m_pGameInstance->Add_Prototype(ENUM_TO_UINT(LEVEL_ID::GAMEPLAY), PROTO_OBJ_NAME(L"BackGround"), CBackGround::Create(m_pDevice, m_pDeviceContext))))
    //    return E_FAIL;

    m_isFinished = true;
    return S_OK;
}

HRESULT CLoader::Loading_Logo()
{
    lstrcpy(m_szFPS, TEXT("텍스쳐를 로딩 중 입니다."));
    if (FAILED(Register_Textures()))
        return E_FAIL;

    lstrcpy(m_szFPS, TEXT("ㅅㅖ이더을(를) 로딩 중 입니다."));

    if (FAILED(Register_Shaders()))
        return E_FAIL;


    lstrcpy(m_szFPS, TEXT("모델을(를) 로딩 중 입니다."));
    if (FAILED(Register_Models()))
        return E_FAIL;




  
    lstrcpy(m_szFPS, TEXT("컴포넌트 원형(를) 로딩 중 입니다."));
    if (FAILED(Register_Components()))
        return E_FAIL;


    lstrcpy(m_szFPS, TEXT("객체원형을(를) 로딩 중 입니다."));
    if (FAILED(Register_GameObjects()))
        return E_FAIL;

   

    m_isFinished = true;
    return S_OK;
}

HRESULT CLoader::Loading_UI()
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
    m_isFinished = true;
    return S_OK;
}

HRESULT CLoader::Register_Shaders()
{
	wchar_t buffer[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, buffer);
	OutputDebugStringW(buffer);

	CShader* pInstance = CShader::Create(m_pDevice,
        m_pDeviceContext, VTXPOSTEX::desc, L"../../Resource/Shader/Shader_VtxPosTex.hlsl",
		"DefaultTechnique");
    m_pGameInstance->Register_Shader(L"Default", pInstance);

	pInstance = CShader::Create(m_pDevice,
        m_pDeviceContext, VTXNORTEX::desc, L"../../Resource/Shader/Shader_VtxNorTex.hlsl",
		"DefaultTechnique");
    m_pGameInstance->Register_Shader(L"VtxNorTex", pInstance);


    pInstance = CShader::Create(m_pDevice,
        m_pDeviceContext, VTXMESH::desc, L"../../Resource/Shader/Shader_VtxMesh.hlsl",
        "DefaultTechnique");
    m_pGameInstance->Register_Shader(L"VtxMesh", pInstance);


    return S_OK;
}

HRESULT CLoader::Register_Textures()
{
    CTexture* pTexture = CTexture::Create(m_pDevice, m_pDeviceContext, L"../../Resource/Keroro.png", 1);
    if (FAILED(m_pGameInstance->Register_Texture(L"Keroro", pTexture)))
        return E_FAIL;


    pTexture = CTexture::Create(m_pDevice, m_pDeviceContext, L"../../Resource/Player_Marker.png", 1);
    if (FAILED(m_pGameInstance->Register_Texture(L"Player_Marker", pTexture)))
        return E_FAIL;



    pTexture = CTexture::Create(m_pDevice, m_pDeviceContext, L"../../Resource/Hp.png", 1);
    if (FAILED(m_pGameInstance->Register_Texture(L"Hp", pTexture)))
        return E_FAIL;


    pTexture = CTexture::Create(m_pDevice, m_pDeviceContext, L"../../Resource/Ground%d.dds", 2);
    if (FAILED(m_pGameInstance->Register_Texture(L"Terrain", pTexture)))
        return E_FAIL;

    pTexture = CTexture::Create(m_pDevice, m_pDeviceContext, L"../../Resource/Mask.bmp", 1);
    if (FAILED(m_pGameInstance->Register_Texture(L"Mask", pTexture)))
        return E_FAIL;


    pTexture = CTexture::Create(m_pDevice, m_pDeviceContext, L"../../Resource/Skeleton.png", 1);
    if (FAILED(m_pGameInstance->Register_Texture(L"Skeleton", pTexture)))
        return E_FAIL;

    return S_OK;
}

HRESULT CLoader::Register_Models()
{

    //if (FAILED(m_pGameInstance->Add_Prototype(ENUM_TO_UINT(LEVEL_ID::STATIC), PROTO_COMPONENT_NAME(L"Zelda_Model"), CModel::Create(m_pDevice, m_pDeviceContext, "C:/Users/kmj69/Documents/GitHub/DX11Framework/Resource/Data/Zelda/Zelda.json"))))
    //    return E_FAIL;

    //if (FAILED(m_pGameInstance->Add_Prototype(ENUM_TO_UINT(LEVEL_ID::STATIC), PROTO_COMPONENT_NAME(L"ForkLift_Model"), CModel::Create(m_pDevice, m_pDeviceContext, "C:/Users/kmj69/Documents/GitHub/DX11Framework/Resource/Data/ForkLift/ForkLift.json"))))
    //    return E_FAIL;

    _matrix preMatrix = XMMatrixRotationY(XMConvertToRadians(180.0f));

    if (FAILED(m_pGameInstance->Add_Prototype(ENUM_TO_UINT(LEVEL_ID::STATIC), PROTO_COMPONENT_NAME(L"Fiona_Model"), CModel::Create(m_pDevice, m_pDeviceContext,XMMatrixIdentity(), "C:/Users/kmj69/Documents/GitHub/DX11Framework/Resource/Data/Fiona/Fiona.json"))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Prototype(ENUM_TO_UINT(LEVEL_ID::STATIC), PROTO_COMPONENT_NAME(L"Field_03P2_Model"), CModel::Create(m_pDevice, m_pDeviceContext, XMMatrixIdentity(), "C:/Users/kmj69/Documents/GitHub/DX11Framework/Resource/Data/Field_03P2/Field_03P2.json"))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Prototype(ENUM_TO_UINT(LEVEL_ID::STATIC), PROTO_COMPONENT_NAME(L"Link2_Model"), CModel::Create(m_pDevice, m_pDeviceContext, preMatrix,"C:/Users/kmj69/Documents/GitHub/DX11Framework/Resource/Data/Link2/Link2.json"))))
        return E_FAIL;

    return S_OK;
}

HRESULT CLoader::Register_Components()
{
    if (FAILED(m_pGameInstance->Add_Prototype(ENUM_TO_UINT(LEVEL_ID::STATIC), PROTO_COMPONENT_NAME(L"Transform"), CTransform::Create(m_pDevice, m_pDeviceContext))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Prototype(ENUM_TO_UINT(LEVEL_ID::STATIC), PROTO_COMPONENT_NAME(L"PerspectiveCamera"), CPerspectiveCameraComponent::Create(m_pDevice, m_pDeviceContext))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Prototype(ENUM_TO_UINT(LEVEL_ID::STATIC), PROTO_COMPONENT_NAME(L"OrthographicCamera"), COrthographicCameraComponent::Create(m_pDevice, m_pDeviceContext))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Prototype(ENUM_TO_UINT(LEVEL_ID::STATIC), PROTO_COMPONENT_NAME(L"VIBuffer_Rect"), CVIBuffer_Rect::Create(m_pDevice, m_pDeviceContext))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Prototype(ENUM_TO_UINT(LEVEL_ID::STATIC), PROTO_COMPONENT_NAME(L"VIBuffer_Terrain"), CVIBuffer_Terrain::Create(m_pDevice, m_pDeviceContext, L"../../Resource/Height.bmp"))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Prototype(ENUM_TO_UINT(LEVEL_ID::STATIC), PROTO_COMPONENT_NAME(L"UI"), CUIComponent::Create(m_pDevice, m_pDeviceContext))))
        return E_FAIL;

    return S_OK;
}

HRESULT CLoader::Register_GameObjects()
{
    if (FAILED(m_pGameInstance->Add_Prototype(ENUM_TO_UINT(LEVEL_ID::STATIC), PROTO_OBJ_NAME(L"BackGround"), CBackGround::Create(m_pDevice, m_pDeviceContext))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Prototype(ENUM_TO_UINT(LEVEL_ID::STATIC), PROTO_OBJ_NAME(L"MainCamera"), CMainCamera::Create(m_pDevice, m_pDeviceContext))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Prototype(ENUM_TO_UINT(LEVEL_ID::STATIC), PROTO_OBJ_NAME(L"UICamera"), CUICamera::Create(m_pDevice, m_pDeviceContext))))
        return E_FAIL;

    //Freecam Test용
    if (FAILED(m_pGameInstance->Add_Prototype(ENUM_TO_UINT(LEVEL_ID::STATIC), PROTO_OBJ_NAME(L"FreeCamera"), CFreeCamera::Create(m_pDevice, m_pDeviceContext))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Prototype(ENUM_TO_UINT(LEVEL_ID::STATIC), PROTO_OBJ_NAME(L"Quad"), CQuad::Create(m_pDevice, m_pDeviceContext))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Prototype(ENUM_TO_UINT(LEVEL_ID::STATIC), PROTO_OBJ_NAME(L"Panel"), CPanel::Create(m_pDevice, m_pDeviceContext))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Prototype(ENUM_TO_UINT(LEVEL_ID::STATIC), PROTO_OBJ_NAME(L"Floor"), CFloor::Create(m_pDevice, m_pDeviceContext))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Prototype(ENUM_TO_UINT(LEVEL_ID::STATIC), PROTO_OBJ_NAME(L"Player"), CPlayer::Create(m_pDevice, m_pDeviceContext))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Prototype(ENUM_TO_UINT(LEVEL_ID::STATIC), PROTO_OBJ_NAME(L"ScreenQuad"), CScreenQuad::Create(m_pDevice, m_pDeviceContext))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Prototype(ENUM_TO_UINT(LEVEL_ID::STATIC), PROTO_OBJ_NAME(L"MinimapCamera"), CMinimapCamera::Create(m_pDevice, m_pDeviceContext))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Prototype(ENUM_TO_UINT(LEVEL_ID::STATIC), PROTO_OBJ_NAME(L"MinimapQuad"), CMinimapQuad::Create(m_pDevice, m_pDeviceContext))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Prototype(ENUM_TO_UINT(LEVEL_ID::STATIC), PROTO_OBJ_NAME(L"Terrain"), CTerrain::Create(m_pDevice, m_pDeviceContext))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Prototype(ENUM_TO_UINT(LEVEL_ID::STATIC), PROTO_OBJ_NAME(L"Model"), CModelObject::Create(m_pDevice, m_pDeviceContext))))
        return E_FAIL;

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
