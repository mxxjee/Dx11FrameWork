#include "..\..\MapTool\Public\CLoader.h"
#include "CLoader.h"
#include "CGameInstance.h"
#include "CMapLoader.h"

////////////////////UI/////////////////////
#include "CFadeScreen.h"
#include "CButton.h"
#include "CMinimapQuad.h"
#include "CScreenQuad.h"
#include "CButton.h"
#include "CFontUI.h"


///////////////Component//////////////////////
#include "CNavigation.h"
#include "CTransform.h"
#include "CVIBuffer_Rect.h" 
#include "CVIBuffer_Terrain.h"
#include "CModel.h"
#include "CVIBuffer_Triangle.h"
#include "CGravity.h"
#include "CBoxColliderComponent.h"
#include "CSphereColliderComponent.h"
#include "COBBColliderComponent.h"
#include "CVIBuffer_Particle_Rect.h"
#include "CVIBuffer_Particle_Point.h"
#include "CFontComponent.h"



///////////////GameObject//////////////////////
#include "CBackGround.h"
#include "CPlayer.h"
#include "CFloor.h"
#include "CMainCamera.h"
#include "CUICamera.h"
#include "CFreeCamera.h"
#include "CMinimapCamera.h"
#include "CTerrain.h"
#include "CTexture.h"
#include "CShader.h"
#include "CMonster.h"
#include "CModelObject.h"
#include "CBody.h"
#include "CPlayer_Body.h"
#include "CMonster_Body.h"
#include "CNPC_Body.h"
#include "CM_GreenZol.h"
#include "CPlayer_Sword.h"
#include "CPlayer_Shield.h"
#include "CM_MoriblinSword.h"
#include "CMMoriblin_Weapon.h"
#include "CStaticBody.h"
#include "CAnimBody.h"
#include "CAnimModelObject.h"
#include "CStaticModelObject.h"

#include "CSnow.h"
#include "CExplosion.h"
#include "CMagicPowder.h"

#include "CInteraction_TriggerBox.h"
#include "CInteractionObject.h"
#include "CInteraction_Lawn.h"
#include "CInteraction_Rock.h"
#include "CInteraction_Mushroom.h"
#include "CSocket_Model.h"    

#include "CRoom.h"
#include "CRoomTrigger.h"
#include "CEventTrigger.h"

#include "CNPC_Fairy.h"
#include "CInteraction_BigRock.h"




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

    case Client::LEVEL_ID::SPAWN:
        Loading_Spawn();
        break;

    case Client::LEVEL_ID::TOWN:
        Loading_Town();
        break;


    case LEVEL_ID::UI:
        Loading_UI();
        break;

    case LEVEL_ID::ROOM:
        Loading_Room();
        break;

    case LEVEL_ID::DUNGEON:
        Loading_Dungeon();
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

HRESULT CLoader::Loading_Town()
{
    lstrcpy(m_szFPS, TEXT("텍스쳐를 로딩 중 입니다."));
    for (size_t i = 0; i < 88899999; i++)
    {
        int a = 10;
    }
    lstrcpy(m_szFPS, TEXT("모델을(를) 로딩 중 입니다."));



    lstrcpy(m_szFPS, TEXT("ㅅㅖ이더을(를) 로딩 중 입니다."));
    for (size_t i = 0; i < 88889999; i++)
    {
        int a = 10;
    }


    lstrcpy(m_szFPS, TEXT("맵 로딩중입니다."));
    Load_TownMapData();

    lstrcpy(m_szFPS, TEXT("로딩완료!"));


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

HRESULT CLoader::Loading_Room()
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

HRESULT CLoader::Loading_Spawn()
{
    lstrcpy(m_szFPS, TEXT("텍스쳐를 로딩 중 입니다."));
    for (size_t i = 0; i < 88899999; i++)
    {
        int a = 10;
    }
    lstrcpy(m_szFPS, TEXT("모델을(를) 로딩 중 입니다."));



    lstrcpy(m_szFPS, TEXT("ㅅㅖ이더을(를) 로딩 중 입니다."));
    for (size_t i = 0; i < 88889999; i++)
    {
        int a = 10;
    }


    lstrcpy(m_szFPS, TEXT("맵 로딩중입니다."));


    lstrcpy(m_szFPS, TEXT("로딩완료!"));


    m_isFinished = true;
    return S_OK;
}

HRESULT CLoader::Loading_Dungeon()
{
    lstrcpy(m_szFPS, TEXT("텍스쳐를 로딩 중 입니다."));
    for (size_t i = 0; i < 88899999; i++)
    {
        int a = 10;
    }
    lstrcpy(m_szFPS, TEXT("모델을(를) 로딩 중 입니다."));



    lstrcpy(m_szFPS, TEXT("ㅅㅖ이더을(를) 로딩 중 입니다."));
    for (size_t i = 0; i < 88889999; i++)
    {
        int a = 10;
    }


    lstrcpy(m_szFPS, TEXT("맵 로딩중입니다."));


    lstrcpy(m_szFPS, TEXT("로딩완료!"));


    m_isFinished = true;

    return S_OK;
}

HRESULT CLoader::Register_Shaders()
{
	wchar_t buffer[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, buffer);
	OutputDebugStringW(buffer);

    CShader* pInstance = CShader::Create(m_pDevice,
        m_pDeviceContext, VTXPOS::desc, L"../../Resource/Shader/Shader_VtxPos.hlsl",
        "DefaultTechnique");
    m_pGameInstance->Register_Shader(L"VtxPos", pInstance);




	pInstance = CShader::Create(m_pDevice,
        m_pDeviceContext, VTXNORTEX::desc, L"../../Resource/Shader/Shader_VtxNorTex.hlsl",
		"DefaultTechnique");
    m_pGameInstance->Register_Shader(L"VtxNorTex", pInstance);


    pInstance = CShader::Create(m_pDevice,
        m_pDeviceContext, VTXMESH::desc, L"../../Resource/Shader/Shader_VtxMesh.hlsl",
        "DefaultTechnique");
    m_pGameInstance->Register_Shader(L"VtxMesh", pInstance);

    pInstance = CShader::Create(m_pDevice,
        m_pDeviceContext, VTXANIMMESH::desc, L"../../Resource/Shader/Shader_VtxAnimMesh.hlsl",
        "DefaultTechnique");
    m_pGameInstance->Register_Shader(L"VtxAnimMesh", pInstance);

    
    pInstance = CShader::Create(m_pDevice,
        m_pDeviceContext, tagVertexPosTexParticle::desc, L"../../Resource/Shader/Shader_VtxPosTex_Particle.hlsl",
        "DefaultTechnique");
    m_pGameInstance->Register_Shader(L"VtxPosTexParticle", pInstance);
    

    pInstance = CShader::Create(m_pDevice,
        m_pDeviceContext, tagVertexPosParticle::desc, L"../../Resource/Shader/Shader_VtxPos_Particle.hlsl",
        "DefaultTechnique");
    m_pGameInstance->Register_Shader(L"VtxPosParticle", pInstance);

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



    pTexture = CTexture::Create(m_pDevice, m_pDeviceContext, L"../../Resource/UI/PlayerHUD/Hp.dds", 1);
    if (FAILED(m_pGameInstance->Register_Texture(L"Hp", pTexture)))
        return E_FAIL;


    pTexture = CTexture::Create(m_pDevice, m_pDeviceContext, L"../../Resource/Ground%d.dds", 2);
    if (FAILED(m_pGameInstance->Register_Texture(L"Terrain", pTexture)))
        return E_FAIL;

    pTexture = CTexture::Create(m_pDevice, m_pDeviceContext, L"../../Resource/Mask.bmp", 1);
    if (FAILED(m_pGameInstance->Register_Texture(L"Mask", pTexture)))
        return E_FAIL;

    pTexture = CTexture::Create(m_pDevice, m_pDeviceContext, L"../../Resource/Particle/Snow/Snow.png", 1);
    if (FAILED(m_pGameInstance->Register_Texture(L"Snow", pTexture)))
        return E_FAIL;

    m_pGameInstance->Load_Textures(L"../../Resource/UI/Logo/", L".dds");
    m_pGameInstance->Load_Textures(L"../../Resource/UI/Interaction/", L".dds");
    m_pGameInstance->Load_Textures(L"../../Resource/UI/NPC/", L".dds");
    m_pGameInstance->Load_Textures(L"../../Resource/UI/Items/", L".dds");


    return S_OK;
}

HRESULT CLoader::Register_Models()
{
   
    _matrix matrix = XMMatrixRotationY(XMConvertToRadians(180.f));
     matrix = XMMatrixMultiply(matrix, XMMatrixScaling(1.2f, 1.2f, 1.2f));


    m_pGameInstance->Load_All_Models("C:/Users/kmj69/Documents/GitHub/DX11Framework/Resource/Model/Actor/LinkAnim", matrix);
  
    _matrix NPCmatrix = XMMatrixRotationY(XMConvertToRadians(180.f));
    
    _matrix GreenZolMatrix = XMMatrixScaling(0.7f, 0.7f, 0.7f);
    GreenZolMatrix = XMMatrixMultiply(GreenZolMatrix,XMMatrixRotationY(XMConvertToRadians(180.f)));


    _matrix MoriblinSwordMatrix= XMMatrixRotationY(XMConvertToRadians(180.f));

    _matrix PreMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);
    PreMatrix = XMMatrixMultiply(PreMatrix, XMMatrixRotationX(XMConvertToRadians(-180.f)));

    _matrix RoomMatrix = XMMatrixScaling(1.1f, 1.1f, 1.1f);


    m_pGameInstance->Load_All_Models("C:/Users/kmj69/Documents/GitHub/DX11Framework/Resource/Model/Actor/Enemy/ZolGreenAnim", GreenZolMatrix);
    
    m_pGameInstance->Load_All_Models("C:/Users/kmj69/Documents/GitHub/DX11Framework/Resource/Model/Actor/Enemy/MoriblinSword", MoriblinSwordMatrix);


    _matrix DadMatrix = XMMatrixScaling(1.2f, 1.2f, 1.2f);
    DadMatrix = XMMatrixMultiply(DadMatrix, NPCmatrix);

    m_pGameInstance->Load_Model("C:/Users/kmj69/Documents/GitHub/DX11Framework/Resource/Model/Actor/NPC/Dad/Dad.json", DadMatrix);
    m_pGameInstance->Load_Model("C:/Users/kmj69/Documents/GitHub/DX11Framework/Resource/Model/Actor/NPC/Mom/Mom.json", DadMatrix);
    m_pGameInstance->Load_Model("C:/Users/kmj69/Documents/GitHub/DX11Framework/Resource/Model/Actor/NPC/RichardAnim/RichardAnim.json", NPCmatrix);
   
    m_pGameInstance->Load_Model("C:/Users/kmj69/Documents/GitHub/DX11Framework/Resource/Model/Actor/NPC/Fairy/Fairy.json", NPCmatrix);
    m_pGameInstance->Load_Model("C:/Users/kmj69/Documents/GitHub/DX11Framework/Resource/Model/Actor/NPC/Kid_Green/Kid_Green.json", NPCmatrix);
    m_pGameInstance->Load_Model("C:/Users/kmj69/Documents/GitHub/DX11Framework/Resource/Model/Actor/NPC/Tarin/Tarin.json", NPCmatrix);
    m_pGameInstance->Load_Model("C:/Users/kmj69/Documents/GitHub/DX11Framework/Resource/Model/Actor/NPC/Witch/Witch.json", NPCmatrix);
    m_pGameInstance->Load_Model("C:/Users/kmj69/Documents/GitHub/DX11Framework/Resource/Model/Actor/NPC/Marin/Marin.json", NPCmatrix);
    m_pGameInstance->Load_Model("C:/Users/kmj69/Documents/GitHub/DX11Framework/Resource/Model/Actor/NPC/Kid_Blue/Kid_Blue.json", NPCmatrix);


    m_pGameInstance->Load_All_Models("C:/Users/kmj69/Documents/GitHub/DX11Framework/Resource/Model/Obstacle/", PreMatrix);

    m_pGameInstance->Load_All_Models("C:/Users/kmj69/Documents/GitHub/DX11Framework/Resource/Model/Object/", XMMatrixIdentity());

    m_pGameInstance->Load_All_Models("C:/Users/kmj69/Documents/GitHub/DX11Framework/Resource/Model/Rooms/", RoomMatrix);

    m_pGameInstance->Load_Model("C:/Users/kmj69/Documents/GitHub/DX11Framework/Resource/Model/Actor/Weathercock/Weathercock.json", XMMatrixIdentity());

    return S_OK;
}

HRESULT CLoader::Register_Components()
{



    if (FAILED(m_pGameInstance->Add_Prototype(ENUM_TO_UINT(LEVEL_ID::STATIC), PROTO_COMPONENT_NAME(L"VIBuffer_Terrain"), CVIBuffer_Terrain::Create(m_pDevice, m_pDeviceContext, L"../../Resource/Height.bmp"))))
        return E_FAIL;


    CVIBuffer_Particle_Rect::PARTICLE_RECT_DESC	SnowDesc{};
    SnowDesc.iNumInstance = 8000;
    SnowDesc.vCenter = _float3(0.f, 0.f, 0.f);
    SnowDesc.vSize = _float2(0.1f, 0.3f);
    SnowDesc.vRange = _float3(130.f, 1.f, 130.f);
    SnowDesc.vSpeed = _float2(2.f, 5.f);
    SnowDesc.vLifeTime = _float2(3.f, 5.f);
    SnowDesc.isLoop = true;
    if (FAILED(m_pGameInstance->Add_Prototype(ENUM_TO_UINT(LEVEL_ID::STATIC), PROTO_COMPONENT_NAME(L"VIBuffer_Particle_Rect"), CVIBuffer_Particle_Rect::Create(m_pDevice, m_pDeviceContext,&SnowDesc))))
        return E_FAIL;



    CVIBuffer_Particle_Point::PARTICLE_DESC	ExploDesc{};
    ExploDesc.iNumInstance = 800;
    ExploDesc.vCenter = _float3(0.f, 0.f, 0.f);
    ExploDesc.vSize = _float2(0.05f, 0.15f);
    ExploDesc.vRange = _float3(0.5f, 0.5f, 0.5f);
    ExploDesc.vSpeed = _float2(2.f, 5.f);
    ExploDesc.vLifeTime = _float2(1.f, 1.5f);
    ExploDesc.isLoop = true;
    ExploDesc.vPivot = _float3(0.f, -0.5f, 0.f);
    if (FAILED(m_pGameInstance->Add_Prototype(ENUM_TO_UINT(LEVEL_ID::STATIC), PROTO_COMPONENT_NAME(L"VIBuffer_Particle_Point"), CVIBuffer_Particle_Point::Create(m_pDevice, m_pDeviceContext, &ExploDesc))))
        return E_FAIL;



    if (FAILED(m_pGameInstance->Add_Prototype(ENUM_TO_UINT(LEVEL_ID::STATIC), PROTO_COMPONENT_NAME(L"Navigation"), CNavigation::Create(m_pDevice, m_pDeviceContext))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Prototype(ENUM_TO_UINT(LEVEL_ID::STATIC), PROTO_COMPONENT_NAME(L"VIBuffer_Triangle"), CVIBuffer_Triangle::Create(m_pDevice, m_pDeviceContext))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Prototype(ENUM_TO_UINT(LEVEL_ID::STATIC), PROTO_COMPONENT_NAME(L"Gravity"), CGravity::Create(m_pDevice, m_pDeviceContext))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Prototype(ENUM_TO_UINT(LEVEL_ID::STATIC), PROTO_COMPONENT_NAME(L"BoxCollider"), CBoxColliderComponent::Create(m_pDevice, m_pDeviceContext))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Prototype(ENUM_TO_UINT(LEVEL_ID::STATIC), PROTO_COMPONENT_NAME(L"SphereCollider"), CSphereColliderComponent::Create(m_pDevice, m_pDeviceContext))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Prototype(ENUM_TO_UINT(LEVEL_ID::STATIC), PROTO_COMPONENT_NAME(L"OBBCollider"), COBBColliderComponent::Create(m_pDevice, m_pDeviceContext))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Prototype(ENUM_TO_UINT(LEVEL_ID::STATIC), PROTO_COMPONENT_NAME(L"Font"), CFontComponent::Create(m_pDevice, m_pDeviceContext))))
        return E_FAIL;
    return S_OK;
}


HRESULT CLoader::Register_GameObjects()
{
    
    if (FAILED(REGISTER_OBJ(ENUM_TO_UINT(LEVEL_ID::STATIC), L"BackGround", CBackGround::Create(m_pDevice, m_pDeviceContext))))
        return E_FAIL;

    if (FAILED(REGISTER_OBJ(ENUM_TO_UINT(LEVEL_ID::STATIC), L"MainCamera", CMainCamera::Create(m_pDevice, m_pDeviceContext))))
        return E_FAIL;



    //Freecam Test용
    if (FAILED(REGISTER_OBJ(ENUM_TO_UINT(LEVEL_ID::STATIC), L"FreeCamera", CFreeCamera::Create(m_pDevice, m_pDeviceContext))))
        return E_FAIL;



    if (FAILED(REGISTER_OBJ(ENUM_TO_UINT(LEVEL_ID::STATIC), L"Floor", CFloor::Create(m_pDevice, m_pDeviceContext))))
        return E_FAIL;

    if (FAILED(REGISTER_OBJ(ENUM_TO_UINT(LEVEL_ID::STATIC), L"Player", CPlayer::Create(m_pDevice, m_pDeviceContext))))
        return E_FAIL;

    if (FAILED(REGISTER_OBJ(ENUM_TO_UINT(LEVEL_ID::STATIC), L"ScreenQuad", CScreenQuad::Create(m_pDevice, m_pDeviceContext))))
        return E_FAIL;

    if (FAILED(REGISTER_OBJ(ENUM_TO_UINT(LEVEL_ID::STATIC), L"MinimapCamera", CMinimapCamera::Create(m_pDevice, m_pDeviceContext))))
        return E_FAIL;

    if (FAILED(REGISTER_OBJ(ENUM_TO_UINT(LEVEL_ID::STATIC), L"MinimapQuad", CMinimapQuad::Create(m_pDevice, m_pDeviceContext))))
        return E_FAIL;

    if (FAILED(REGISTER_OBJ(ENUM_TO_UINT(LEVEL_ID::STATIC), L"Terrain", CTerrain::Create(m_pDevice, m_pDeviceContext))))
        return E_FAIL;

    if (FAILED(REGISTER_OBJ(ENUM_TO_UINT(LEVEL_ID::STATIC), L"Model", CModelObject::Create(m_pDevice, m_pDeviceContext))))
        return E_FAIL;

    if (FAILED(REGISTER_OBJ(ENUM_TO_UINT(LEVEL_ID::STATIC), L"AnimModel", CAnimModelObject::Create(m_pDevice, m_pDeviceContext))))
        return E_FAIL;

    if (FAILED(REGISTER_OBJ(ENUM_TO_UINT(LEVEL_ID::STATIC), L"StaticModel", CStaticModelObject::Create(m_pDevice, m_pDeviceContext))))
        return E_FAIL;

    if (FAILED(REGISTER_OBJ(ENUM_TO_UINT(LEVEL_ID::STATIC), L"Body", CBody::Create(m_pDevice, m_pDeviceContext))))
        return E_FAIL;

    if (FAILED(REGISTER_OBJ(ENUM_TO_UINT(LEVEL_ID::STATIC), L"AnimBody", CAnimBody::Create(m_pDevice, m_pDeviceContext))))
        return E_FAIL;

    if (FAILED(REGISTER_OBJ(ENUM_TO_UINT(LEVEL_ID::STATIC), L"StaticBody", CStaticBody::Create(m_pDevice, m_pDeviceContext))))
        return E_FAIL;

    if (FAILED(REGISTER_OBJ(ENUM_TO_UINT(LEVEL_ID::STATIC), L"Soket_Model", CSocket_Model::Create(m_pDevice, m_pDeviceContext))))
        return E_FAIL;

    if (FAILED(REGISTER_OBJ(ENUM_TO_UINT(LEVEL_ID::STATIC), L"Player_Body", CPlayer_Body::Create(m_pDevice, m_pDeviceContext))))
        return E_FAIL;


    if (FAILED(REGISTER_OBJ(ENUM_TO_UINT(LEVEL_ID::STATIC), L"Player_Sword", CPlayer_Sword::Create(m_pDevice, m_pDeviceContext))))
        return E_FAIL;

    if (FAILED(REGISTER_OBJ(ENUM_TO_UINT(LEVEL_ID::STATIC), L"Player_Shield", CPlayer_Shield::Create(m_pDevice, m_pDeviceContext))))
        return E_FAIL;

    if (FAILED(REGISTER_OBJ(ENUM_TO_UINT(LEVEL_ID::STATIC), L"NPC_Body", CNPC_Body::Create(m_pDevice, m_pDeviceContext))))
        return E_FAIL;


    if (FAILED(REGISTER_OBJ(ENUM_TO_UINT(LEVEL_ID::STATIC), L"Button", CButton::Create(m_pDevice, m_pDeviceContext))))
        return E_FAIL;

    if (FAILED(REGISTER_OBJ(ENUM_TO_UINT(LEVEL_ID::STATIC), L"FontUI", CFontUI::Create(m_pDevice, m_pDeviceContext))))
        return E_FAIL;

    ////////////////Monsters//////////////////
    if (FAILED(REGISTER_OBJ(ENUM_TO_UINT(LEVEL_ID::STATIC), L"Monster", CMonster::Create(m_pDevice, m_pDeviceContext))))
        return E_FAIL;

    if (FAILED(REGISTER_OBJ(ENUM_TO_UINT(LEVEL_ID::STATIC), L"Monster_Body", CMonster_Body::Create(m_pDevice, m_pDeviceContext))))
        return E_FAIL;

    if (FAILED(REGISTER_OBJ(ENUM_TO_UINT(LEVEL_ID::STATIC), L"CM_GreenZol", CM_GreenZol::Create(m_pDevice, m_pDeviceContext))))
        return E_FAIL;

    if (FAILED(REGISTER_OBJ(ENUM_TO_UINT(LEVEL_ID::STATIC), L"CM_MoriblinSword", CM_MoriblinSword::Create(m_pDevice, m_pDeviceContext))))
        return E_FAIL;

    if (FAILED(REGISTER_OBJ(ENUM_TO_UINT(LEVEL_ID::STATIC), L"Moriblin_Weapon", CMMoriblin_Weapon::Create(m_pDevice, m_pDeviceContext))))
        return E_FAIL;

    /// /////////Interaction Objects
    if (FAILED(REGISTER_OBJ(ENUM_TO_UINT(LEVEL_ID::STATIC), L"Interaction_TriggerBox", CInteraction_TriggerBox::Create(m_pDevice, m_pDeviceContext))))
        return E_FAIL;

    if (FAILED(REGISTER_OBJ(ENUM_TO_UINT(LEVEL_ID::STATIC), L"InteractionObject", CInteractionObject::Create(m_pDevice, m_pDeviceContext))))
        return E_FAIL;

    if (FAILED(REGISTER_OBJ(ENUM_TO_UINT(LEVEL_ID::STATIC), L"Interaction_Rock", CInteraction_Rock::Create(m_pDevice, m_pDeviceContext))))
        return E_FAIL;

    if (FAILED(REGISTER_OBJ(ENUM_TO_UINT(LEVEL_ID::STATIC), L"Interaction_Lawn", CInteraction_Lawn::Create(m_pDevice, m_pDeviceContext))))
        return E_FAIL;

    if (FAILED(REGISTER_OBJ(ENUM_TO_UINT(LEVEL_ID::STATIC), L"Interaction_Mushroom", CInteraction_Mushroom::Create(m_pDevice, m_pDeviceContext))))
        return E_FAIL;


    if (FAILED(REGISTER_OBJ(ENUM_TO_UINT(LEVEL_ID::STATIC), L"Interaction_BigRock", CInteraction_BigRock::Create(m_pDevice, m_pDeviceContext))))
        return E_FAIL;


    /////////Rooms
    if (FAILED(REGISTER_OBJ(ENUM_TO_UINT(LEVEL_ID::STATIC), L"Room", CRoom::Create(m_pDevice, m_pDeviceContext))))
        return E_FAIL;

    if (FAILED(REGISTER_OBJ(ENUM_TO_UINT(LEVEL_ID::STATIC), L"RoomTrigger", CRoomTrigger::Create(m_pDevice, m_pDeviceContext))))
        return E_FAIL;



    if (FAILED(REGISTER_OBJ(ENUM_TO_UINT(LEVEL_ID::STATIC), L"EventTrigger", CEventTrigger::Create(m_pDevice, m_pDeviceContext))))
        return E_FAIL;

    ////////Particles
    if (FAILED(REGISTER_OBJ(ENUM_TO_UINT(LEVEL_ID::STATIC), L"Snow", CSnow::Create(m_pDevice, m_pDeviceContext))))
        return E_FAIL;

    if (FAILED(REGISTER_OBJ(ENUM_TO_UINT(LEVEL_ID::STATIC), L"Explosion", CExplosion::Create(m_pDevice, m_pDeviceContext))))
        return E_FAIL;

    if (FAILED(REGISTER_OBJ(ENUM_TO_UINT(LEVEL_ID::STATIC), L"MagicPowder", CMagicPowder::Create(m_pDevice, m_pDeviceContext))))
        return E_FAIL;

    return S_OK;
}

HRESULT CLoader::Load_TownMapData()
{
    CMapLoader::Load_Town();
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
