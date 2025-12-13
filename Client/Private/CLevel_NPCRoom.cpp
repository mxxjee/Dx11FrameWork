#include "CLevel_NPCRoom.h"
#include "CRoom_Manager.h"
#include "CGameInstance.h"
#include "CLayer.h"
#include "CGameInstance.h"
#include "CInteraction_Manager.h"
#include "CPlayer.h"
#include "CMainCamera.h"

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
    LightDesc.vDirection = _float4(1.f, 1.f, 1.f, 0.f);
    LightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
    LightDesc.vAmbient = _float4(1.f, 1.f, 1.f, 1.f);
    LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);

    if (FAILED(m_pGameInstance->Add_Light(m_iLevelID, LightDesc)))
        return E_FAIL;


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
    CPlayer* pPlayer = CInteraction_Manager::GetInstance()->Get_MainPlayer();
    CheckNull(pPlayer);

    pPlayer->Get_Transform()->Set_State(STATE::POSITION, XMLoadFloat4(&vSpawnPos));

 
    ////GameEvent (카메라 조절)
    GameEvent Event;
    EventPayload payload;
    Event.Payload = payload;

    Event.Payload.Ptrs["Player"] = pPlayer;
    Event.Name = "Enter_NPCRoom";

    m_pGameInstance->Emit(Event);

}

void CLevel_NPCRoom::OnResume(_uint iPreLevel)
{
}

void CLevel_NPCRoom::OnPause(_uint iNextLevel)
{
}

void CLevel_NPCRoom::OnExit()
{
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
