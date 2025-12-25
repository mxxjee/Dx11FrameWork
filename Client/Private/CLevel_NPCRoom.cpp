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
    hr = m_pGameInstance->Make_New_Layer(ENUM_TO_UINT(LEVEL_ID::ROOM), L"Interaction_Layer");

    CLayer* pEnvLayer = m_pGameInstance->Find_Layer(ENUM_TO_UINT(LEVEL_ID::ROOM), L"Enviroment_Layer");
    CLayer* pTriggerLayer = m_pGameInstance->Find_Layer(ENUM_TO_UINT(LEVEL_ID::ROOM), L"Trigger_Layer");
    CLayer* pNPCLayer = m_pGameInstance->Find_Layer(ENUM_TO_UINT(LEVEL_ID::ROOM), L"NPC_Layer");
    CLayer* pInteractionLayer = m_pGameInstance->Find_Layer(ENUM_TO_UINT(LEVEL_ID::ROOM), L"Interaction_Layer");


    if(FAILED(CRoom_Manager::GetInstance()->Initialize(m_pDevice, m_pContext, pEnvLayer, pNPCLayer, pTriggerLayer)))
        return E_FAIL;

    LIGHT_DESC      LightDesc{};
    LightDesc.eType = LIGHT::DIRECTIONAL;
    LightDesc.vDirection = _float4(1.f, -1.f, 1.f, 0.f);
    LightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
    LightDesc.vAmbient = _float4(0.5f, 0.5f, 0.5f, 1.f);
    LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);

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

void CLevel_NPCRoom::OnEnter()
{
    CRoom_Manager::GetInstance()->Switch_Room(CRoom_Manager::GetInstance()->Get_RequestRoom());
    
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

    Event.Payload.Ptrs["Player"] = m_pPlayer;
    Event.Payload.Floats["OffSet_X"] = 0.f;
    Event.Payload.Floats["OffSet_Y"] = 9.f;
    Event.Payload.Floats["OffSet_Z"] = -4.f;

    Event.Name = "Enter_NPCRoom";

    m_pGameInstance->Emit(Event);
    CheckNull(pFadeScreen);
    pFadeScreen->PlayFadeOut();

  
  
    //씬이 다시시작행슬때 메인 상호작용오브ㅈ게트들 설정
    //////현재씬의 itneraction 등록
    CInteraction_Manager::GetInstance()->Change_Scene(ENUM_TO_UINT(LEVEL_ID::ROOM));

    __super::OnEnter();

    m_pGameInstance->Set_IsLoading(false);


}

void CLevel_NPCRoom::OnResume(_uint iPreLevel)
{
    
    m_pGameInstance->Clear_SceneColliders(m_iLevelID);

    CRoom_Manager::GetInstance()->Switch_Room(CRoom_Manager::GetInstance()->Get_RequestRoom());
 
    CheckNull(m_pPlayer);
 
    _float4 vSpawnPos = CRoom_Manager::GetInstance()->Get_SpawnPosition();
    m_pPlayer->Get_Transform()->Set_State(STATE::POSITION, XMLoadFloat4(&vSpawnPos));
    m_pPlayer->Change_MainNavMesh();



    ////GameEvent (카메라 조절)
    GameEvent Event;
    EventPayload payload;
    Event.Payload = payload;

    Event.Payload.Ptrs["Player"] = m_pPlayer;
    Event.Payload.Floats["OffSet_X"] = 0.f;
    Event.Payload.Floats["OffSet_Y"] = 9.f;
    Event.Payload.Floats["OffSet_Z"] = -4.f;

    Event.Name = "Enter_NPCRoom";

    m_pGameInstance->Emit(Event);
    CheckNull(pFadeScreen);
   pFadeScreen->PlayFadeOut();

  


    //씬이 다시시작행슬때 메인 상호작용오브ㅈ게트들 설정
    //////현재씬의 itneraction 등록
    CInteraction_Manager::GetInstance()->Change_Scene(ENUM_TO_UINT(LEVEL_ID::ROOM));

    __super::OnResume(iPreLevel);
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
