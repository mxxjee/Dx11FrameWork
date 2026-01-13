#include "CLevel_NPCRoom.h"
#include "CRoom_Manager.h"
#include "CGameInstance.h"
#include "CLayer.h"
#include "CGameInstance.h"
#include "CInteraction_Manager.h"
#include "CPlayer.h"
#include "CMainCamera.h"
#include "CFadeScreen.h"
#include "CGameManager.h"
#include "CNavMesh_Manager.h"
#include "CIInteractable.h"


USING(Client)
CLevel_NPCRoom::CLevel_NPCRoom(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pDeviceContext)
    :CLevel(_pDevice, _pDeviceContext)
{
}

HRESULT CLevel_NPCRoom::Initialize(LevelArgs& args)
{
    if (__super::Initialize(args))
        return E_FAIL;

    HRESULT hr;
    hr=m_pGameInstance->Make_New_Layer(ENUM_TO_UINT(LEVEL_ID::ROOM), L"Enviroment_Layer");
    hr = m_pGameInstance->Make_New_Layer(ENUM_TO_UINT(LEVEL_ID::ROOM), L"Trigger_Layer");
    hr = m_pGameInstance->Make_New_Layer(ENUM_TO_UINT(LEVEL_ID::ROOM), L"NPC_Layer");


    CLayer* pEnvLayer = m_pGameInstance->Find_Layer(ENUM_TO_UINT(LEVEL_ID::ROOM), L"Enviroment_Layer");
    CLayer* pTriggerLayer = m_pGameInstance->Find_Layer(ENUM_TO_UINT(LEVEL_ID::ROOM), L"Trigger_Layer");
    CLayer* pNPCLayer = m_pGameInstance->Find_Layer(ENUM_TO_UINT(LEVEL_ID::ROOM), L"NPC_Layer");


    if(FAILED(CRoom_Manager::GetInstance()->Initialize(m_pDevice, m_pContext, pEnvLayer, pNPCLayer, pTriggerLayer)))
        return E_FAIL;

    LIGHT_DESC      LightDesc{};
    LightDesc.eType = LIGHT::DIRECTIONAL;
    LightDesc.vDirection = _float4(1.f, -1.f, 1.f, 0.f);
    LightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
    LightDesc.vAmbient = _float4(0.7f, 0.7f, 0.7f, 1.f);
    LightDesc.vSpecular = _float4(0.5f, 0.5f, 0.5f, 1.f);


    if (FAILED(m_pGameInstance->Add_Light(m_iLevelID, LightDesc)))
        return E_FAIL;


    UIGroup* pGroup = m_pGameInstance->Get_UIGroup(L"FadeScreenGroup");
    pFadeScreen = dynamic_cast<CFadeScreen*>(pGroup->Find(L"FadeScreen"));


    return S_OK;
}

void CLevel_NPCRoom::Update_Priority(_float fTimeDelta)
{
    __super::Update_Priority(fTimeDelta);
}

void CLevel_NPCRoom::Update(const _float fTimeDelta)
{
    __super::Update(fTimeDelta);
}

void CLevel_NPCRoom::Update_Late(_float fTimeDelta)
{
    __super::Update_Late(fTimeDelta);
}

void CLevel_NPCRoom::Render()
{
    __super::Render();

    //UI렌더. (로딩바)
    wchar_t szTitle[256];
    swprintf_s(szTitle, L"Room 씬입니다. FPS : %.1f", m_pGameInstance->Get_FPS(L"Timer_60"));

    SetWindowText(g_hWnd, szTitle);
}

void CLevel_NPCRoom::Play_LevelBGM()
{
    if(CRoom_Manager::GetInstance()->Get_RequestRoom()=="MagicPowerHouse")
        m_pGameInstance->PlayBGM(L"BGM/WitchRoom.wav", g_BGMVolume);

    else
        m_pGameInstance->PlayBGM(L"BGM/House.wav", g_BGMVolume);

}

void CLevel_NPCRoom::OnEnter()
{
    CRoom_Manager::GetInstance()->Switch_Room(CRoom_Manager::GetInstance()->Get_RequestRoom());
    CInteraction_Manager::GetInstance()->Change_Scene(ENUM_TO_UINT(LEVEL_ID::ROOM));

    _float4 vSpawnPos = CRoom_Manager::GetInstance()->Get_SpawnPosition();
   
    m_pPlayer = CGameManager::GetInstance()->Get_MainPlayer();
    CheckNull(m_pPlayer);


    m_pPlayer->Get_Transform()->Set_State(STATE::POSITION, XMLoadFloat4(&vSpawnPos));
    m_pPlayer->Get_Transform()->Rotation(_float3(0.f, 0.f, 0.f));
    m_pPlayer->Change_MainNavMesh();
 

 
    ////GameEvent (카메라 조절)
    GameEvent Event;
    EventPayload payload;
    Event.Payload = payload;
    if (CRoom_Manager::GetInstance()->Get_RequestRoom() == "RichardHouse")
    {
        Event.Payload.Ptrs["Player"] = m_pPlayer;
        Event.Payload.Floats["OffSet_X"] = 0.f;
        Event.Payload.Floats["OffSet_Y"] = 10.f;
        Event.Payload.Floats["OffSet_Z"] = -3.f;


        CGameObject* pObj = m_pGameInstance->Find_GameObject(m_iLevelID, L"Enviroment_Layer", L"Workbench0");
        if (pObj)
            CInteraction_Manager::GetInstance()->RegisterInteractable(dynamic_cast<CIInteractable*>(pObj));

    }

    else
    {
        Event.Payload.Ptrs["Player"] = m_pPlayer;
        Event.Payload.Floats["OffSet_X"] = 0.f;
        Event.Payload.Floats["OffSet_Y"] = 9.f;
        Event.Payload.Floats["OffSet_Z"] = -4.f;
    }
    Event.Name = "Enter_NPCRoom";

    m_pGameInstance->Emit(Event);
    CheckNull(pFadeScreen);
    pFadeScreen->PlayFadeOut();

  
  
    //씬이 다시시작행슬때 메인 상호작용오브ㅈ게트들 설정
    //////현재씬의 itneraction 등록
  
    __super::OnEnter();

    m_pGameInstance->Set_IsLoading(false);

    if (CRoom_Manager::GetInstance()->Get_RequestRoom() == "RichardHouse")
    {
        //카메라고정
        CCamera_Base* pMainCam = m_pGameInstance->Find_Camera(CAMERA_TYPE::TARGET);
        CMainCamera* ppMainCam = dynamic_cast<CMainCamera*>(pMainCam);
        if (ppMainCam)
        {
            ppMainCam->Set_Target(m_pPlayer, true);
            ppMainCam->Set_Lock(true);

        }
    }

    else
    {
        m_pGameInstance->Invoke(0.2f, 0.f, false, false, [this]()
            {
                Play_LevelBGM();

            }, CGameManager::GetInstance()->Get_MainPlayer());

    }

}

void CLevel_NPCRoom::OnResume(_uint iPreLevel)
{
    
    
    CheckNull(m_pPlayer);
 
    LEVEL_ID PrevID = (LEVEL_ID)iPreLevel;
    switch (PrevID)
    {
    case Client::LEVEL_ID::UI:
        break;

    default:
    {   
        m_pGameInstance->Clear_SceneColliders(m_iLevelID);

        CRoom_Manager::GetInstance()->Switch_Room(CRoom_Manager::GetInstance()->Get_RequestRoom());
        CInteraction_Manager::GetInstance()->Change_Scene(ENUM_TO_UINT(LEVEL_ID::ROOM));


        _float4 vSpawnPos = CRoom_Manager::GetInstance()->Get_SpawnPosition();
        m_pPlayer->Get_Transform()->Set_State(STATE::POSITION, XMLoadFloat4(&vSpawnPos));
        m_pPlayer->Change_MainNavMesh();


        ////GameEvent (카메라 조절)
        GameEvent Event;
        EventPayload payload;
        Event.Payload = payload;

        if (CRoom_Manager::GetInstance()->Get_RequestRoom() == "RichardHouse")
        {
            Event.Payload.Ptrs["Player"] = m_pPlayer;
            Event.Payload.Floats["OffSet_X"] = 0.f;
            Event.Payload.Floats["OffSet_Y"] = 10.f;
            Event.Payload.Floats["OffSet_Z"] = -3.f;


            CGameObject* pObj = m_pGameInstance->Find_GameObject(m_iLevelID, L"Enviroment_Layer", L"Workbench0");
            if (pObj)
                CInteraction_Manager::GetInstance()->RegisterInteractable(dynamic_cast<CIInteractable*>(pObj));

        }

        else
        {
            Event.Payload.Ptrs["Player"] = m_pPlayer;
            Event.Payload.Floats["OffSet_X"] = 0.f;
            Event.Payload.Floats["OffSet_Y"] = 9.f;
            Event.Payload.Floats["OffSet_Z"] = -4.f;
        }



        Event.Name = "Enter_NPCRoom";

        m_pGameInstance->Emit(Event);
        CheckNull(pFadeScreen);
        pFadeScreen->PlayFadeOut();


    }
        break;
    }
    




    __super::OnResume(iPreLevel);


    if (CRoom_Manager::GetInstance()->Get_RequestRoom() == "RichardHouse")
    {
        //카메라고정
        CCamera_Base* pMainCam = m_pGameInstance->Find_Camera(CAMERA_TYPE::TARGET);
        CMainCamera* ppMainCam = dynamic_cast<CMainCamera*>(pMainCam);
        if (ppMainCam)
        {
            ppMainCam->Set_Target(m_pPlayer, true);
            ppMainCam->Set_Lock(true);

        }
    }

    else
    {
        m_pGameInstance->Invoke(0.2f, 0.f, false, false, [this]()
            {
                Play_LevelBGM();

            }, CGameManager::GetInstance()->Get_MainPlayer());

    }
}

void CLevel_NPCRoom::OnPause(_uint iNextLevel)
{
 /*   if (iNextLevel != ENUM_TO_UINT(LEVEL_ID::UI))
        CRoom_Manager::GetInstance()->Reset_CurrentRoomID();*/


}

void CLevel_NPCRoom::OnExit()
{
   /* CRoom_Manager::GetInstance()->Reset_CurrentRoomID();*/
    //m_pGameInstance->Clear_SceneColliders(m_iLevelID);


}

CLevel_NPCRoom* CLevel_NPCRoom::Create(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pDeviceContext, LevelArgs& args)
{
    CLevel_NPCRoom* pInstance = new CLevel_NPCRoom(_pDevice, _pDeviceContext);
    if (FAILED(pInstance->Initialize(args)))
    {
        MSG_BOX("Failed to Create : CLevel_NPCRoom");
        Safe_Release(pInstance);
    }


    return pInstance;
}

void CLevel_NPCRoom::Free()
{
    __super::Free();
}
