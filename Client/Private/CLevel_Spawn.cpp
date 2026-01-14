#include "CLevel_Spawn.h"
#include "CFadeScreen.h"
#include "CRoom.h"
#include "CStaticBody.h"
#include "CNPC.h"
#include "CTrigger_Box.h"
#include "CEventTrigger.h"
#include "CGameInstance.h"
#include "CMainCamera.h"
#include "CInteraction_Manager.h"
#include "CPlayer.h"
#include "CGameManager.h"
#include "CUICreator.h"

#include "CAnimModelObject.h"
#include "CModel_Bed.h"
#include "CQuest_Manager.h"

#include "CLayer.h"
#include "CInput_Manager.h"
#include "CMeshEffect.h"
#include "CEffectPoolManager.h"

#include "CInput_Manager.h"
#include "CParticle.h"




USING(Client)
CLevel_Spawn::CLevel_Spawn(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pDeviceContext)
    :CLevel(_pDevice,_pDeviceContext)
{
}

HRESULT CLevel_Spawn::Initialize(LevelArgs& args)
{
    m_pGameManager = CGameManager::GetInstance();

    if(FAILED(__super::Initialize(args)))
        return E_FAIL;

    //룸세팅
    if (FAILED(Ready_Layer_Enviroment(L"Enviroment_Layer")))
        return E_FAIL;
    
    if (FAILED(Ready_Lights()))
        return E_FAIL;

    if(FAILED(Ready_Player_Static(L"Player_Layer")))
        return E_FAIL;


     if (FAILED(Ready_UI_Static(L"UI_Layer")))
         return E_FAIL;

     if (FAILED(Ready_Layer_NPC(L"NPC_Layer")))
         return E_FAIL;


    if (FAILED(Ready_Layer_InteractionObject(L"Interaction_Layer")))
        return E_FAIL;

    if (FAILED(Ready_Layer_Trigger(L"Trigger_Layer")))
        return E_FAIL;

    if (FAILED(Ready_Layer_Particle(L"Particle_Layer")))
        return E_FAIL;



    UIGroup* pGroup = m_pGameInstance->Get_UIGroup(L"FadeScreenGroup");
    pFadeScreen = dynamic_cast<CFadeScreen*>(pGroup->Find(L"FadeScreen"));


    if (FAILED(CQuest_Manager::GetInstance()->Initialize()))
        return E_FAIL;

    Play_LevelBGM();


    return S_OK;
}

void CLevel_Spawn::Update_Priority(_float fTimeDelta)
{
    __super::Update_Priority(fTimeDelta);
  
}

void CLevel_Spawn::Update(const _float fTimeDelta)
{
    __super::Update(fTimeDelta);
    
}

void CLevel_Spawn::Update_Late(_float fTimeDelta)
{
    __super::Update_Late(fTimeDelta);
}

void CLevel_Spawn::Render()
{
    wchar_t szTitle[256];
    swprintf_s(szTitle, L"Spawn 씬입니다. FPS : %.1f", m_pGameInstance->Get_FPS(L"Timer_60"));

    SetWindowText(g_hWnd, szTitle);
}

HRESULT CLevel_Spawn::Ready_Lights()
{
    m_pGameInstance->Load_LightData(ENUM_TO_UINT(LEVEL_ID::SPAWN),
        "../../Resource/Data/Map/Lights/LightData_Spawn.json");
   /* LIGHT_DESC      LightDesc{};
    
     LightDesc.LightName = L"Spawn_DirectionLight";
    LightDesc.eType = LIGHT::DIRECTIONAL;
    LightDesc.vDirection = _float4(1.f, -1.f, 1.f, 0.f);
    LightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
    LightDesc.vAmbient = _float4(0.7f, 0.7f, 0.7f, 1.f);
    LightDesc.vSpecular = _float4(0.5f, 0.5f, 0.5f, 1.f);

    if (FAILED(m_pGameInstance->Add_Light(m_iLevelID, LightDesc)))
        return E_FAIL;


    LIGHT_DESC      PointLightDesc{};

    PointLightDesc.eType = LIGHT::POINT;
    PointLightDesc.LightName = L"Spawn_PointLight";
    PointLightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
    PointLightDesc.vAmbient = _float4(0.7f, 0.7f, 0.7f, 1.f);
    PointLightDesc.vSpecular = _float4(0.5f, 0.5f, 0.5f, 1.f);
    PointLightDesc.fRange = _float4(3.f, 0.f, 0.f, 1.f);

    if (FAILED(m_pGameInstance->Add_Light(m_iLevelID, PointLightDesc)))
        return E_FAIL;*/

    return S_OK;
}

HRESULT CLevel_Spawn::Ready_Layer_Enviroment(const _wstring& strLayerTag)
{
    //룸읽기
    CRoom::MODELOBJECT_DESC     RoomDesc;
    RoomDesc.ObjTag = L"MarinHouse_Room";

    CStaticBody::BODY_DESC  BodyDesc;
    BodyDesc.eRenderGroup = ENUM_TO_UINT(RENDERGROUP::NONALPHA);
    BodyDesc.modelName = L"MarinHouse";
    RoomDesc.BodyDesc = &BodyDesc;

    CTransform::TRANSFORM_DESC TransDesc;
    TransDesc.vLocalPosition = _float4(12.6f, 0.f, 6.5f,1.f);
    RoomDesc.TransformDesc = &TransDesc;

    CBase* pBaseRoom = m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_TO_UINT(LEVEL_ID::STATIC), PROTO_OBJ_NAME(L"Room"), &RoomDesc);
    CGameObject* pRoom = dynamic_cast<CGameObject*>(pBaseRoom);
    if (pRoom)
    {
        if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(ENUM_TO_UINT(LEVEL_ID::SPAWN), strLayerTag, pRoom)))
            return E_FAIL;

        string NavFile ="../../Resource/Data/Map/Room/MarinHouse_room_Nav.dat";
        if (FAILED(m_pGameInstance->Load_NavMesh(ENUM_TO_UINT(LEVEL_ID::SPAWN), NavFile)))
            return E_FAIL;


        m_pGameInstance->Set_MainCells(ENUM_TO_UINT(LEVEL_ID::SPAWN));


    }
    return S_OK;
}


HRESULT CLevel_Spawn::Ready_Layer_NPC(const _wstring& strLayerTag)
{
#pragma region 마린 생성
    CNPC::NPC_DESC pMarin_Desc;

    CTransform::TRANSFORM_DESC pTransDesc;
    pTransDesc.vLocalPosition = _float4(13.f, 0.f, 8.164f, 1.f);
    pTransDesc.vLocalRotation = _float4(0.f, 180.f, 0.f, 1.f);

    pMarin_Desc.ModelName = L"Marin";
    pMarin_Desc.ObjTag = L"NPC_" + pMarin_Desc.ModelName;
    pMarin_Desc.pTarget = nullptr;
    pMarin_Desc.TalkRange = 3.f;
    pMarin_Desc.SceneName = "MarinHouse_Room";

    _uint   iCurIdx[ENUM_TO_UINT(CNPC::EXPRESSION::END)] = {0,2,3,4};
    _uint   iOpenIdx[ENUM_TO_UINT(CNPC::EXPRESSION::END)] = { 1,7,4,8 };

    memcpy(&pMarin_Desc.iExpressionIdxEye, iCurIdx,sizeof(_uint)*ENUM_TO_UINT(CNPC::EXPRESSION::END));
    memcpy(&pMarin_Desc.iExpressionIdx_Mouth, iCurIdx, sizeof(_uint) * ENUM_TO_UINT(CNPC::EXPRESSION::END));

    memcpy(&pMarin_Desc.iOpenIdx_Mouth, iOpenIdx, sizeof(_uint) * ENUM_TO_UINT(CNPC::EXPRESSION::END));


    pMarin_Desc.m_iLevelID = ENUM_TO_UINT(LEVEL_ID::SPAWN);
    pMarin_Desc.TransformDesc = &pTransDesc;


    CNPC* pMarin_Npc = CNPC::Create(m_pDevice, m_pContext, &pMarin_Desc);
    if (pMarin_Npc)
    {
        if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(ENUM_TO_UINT(LEVEL_ID::SPAWN), strLayerTag, pMarin_Npc)))
            return E_FAIL;

        pMarin_Npc->Get_Transform()->Rotation(_float3(0.f, -90.f, 0.f));


    }
#pragma endregion

#pragma region 타린생성
    CNPC::NPC_DESC pTarin_Desc;

    pTransDesc = {};
    pTransDesc.vLocalPosition = _float4(14.8f, 0.f, 7.013f, 1.f);
    pTransDesc.vLocalRotation = _float4(0.f, 180.f, 0.f, 1.f);

    
    pTarin_Desc.ModelName = L"Tarin";
    pTarin_Desc.ObjTag = L"NPC_" + pTarin_Desc.ModelName;
    pTarin_Desc.pTarget = nullptr;
    pTarin_Desc.TalkRange = 0.5f;

    pTarin_Desc.SceneName = "MarinHouse_Room";

    pTarin_Desc.m_iLevelID = ENUM_TO_UINT(LEVEL_ID::SPAWN);
    pTarin_Desc.TransformDesc = &pTransDesc;


    CNPC* pTarin_Npc = CNPC::Create(m_pDevice, m_pContext, &pTarin_Desc);
    if (pTarin_Npc)
    {
        if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(ENUM_TO_UINT(LEVEL_ID::SPAWN), strLayerTag, pTarin_Npc)))
            return E_FAIL;
        
        pTarin_Npc->Get_Transform()->Rotation(_float3(0.f, -90.f, 0.f));

    }
#pragma endregion

    
    return S_OK;
}

HRESULT CLevel_Spawn::Ready_Layer_InteractionObject(const _wstring& strLayerTag)
{
 
    return S_OK;


}

HRESULT CLevel_Spawn::Ready_Layer_Trigger(const _wstring& strLayerTag)
{
    CEventTrigger::EventTriggerDesc EventTriggerDesc;
    EventTriggerDesc.vCenter = _float3(0.f,0.f,0.f);
    EventTriggerDesc.vExtents = _float3(1.5f,1.0f,1.0f);
    EventTriggerDesc.ObjTag = L"Trigger" + 0;
    EventTriggerDesc.EnterFunc = [this]()
    {
        m_pGameInstance->StopSoundFade(CHANNELID::SOUND_BGM, 1.f);

        pFadeScreen->Set_FadeInEndFunc([]()
            {
                /*씬이동*/
                LevelArgs args;
                args.iNextLevelID = ENUM_TO_UINT(LEVEL_ID::TOWN);
                args.changeType = LEVELCHANGETYPE::REPLACETOP;
                args.loadingChangeType = LEVELCHANGETYPE::PUSH;
                args.m_iLevelID = ENUM_TO_UINT(LEVEL_ID::LOADING);



                if (FAILED(CGameInstance::GetInstance()->Level_Changer(
                    ENUM_TO_UINT(LEVEL_ID::LOADING),
                    args)))
                    return;
            });
        pFadeScreen->PlayFadeIn();

      
    };

    EventTriggerDesc.m_iLevelID = ENUM_TO_UINT(LEVEL_ID::SPAWN);

    CTransform::TRANSFORM_DESC TransDesc;
    TransDesc.vLocalPosition = _float4(12.56f, 0.f, 1.22f, 1.f);
    //TransDesc.vLocalScale = _float4(pTriggerInfo.vScale.x, pTriggerInfo.vScale.y, pTriggerInfo.vScale.z, 1.f);

    EventTriggerDesc.TransformDesc = &TransDesc;
    CBase* pBaseTrigger = m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_TO_UINT(LEVEL_ID::STATIC), PROTO_OBJ_NAME(L"EventTrigger"), &EventTriggerDesc);
    CGameObject* pEventTrigger = dynamic_cast<CGameObject*>(pBaseTrigger);
    if (pEventTrigger)
    {

        if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(ENUM_TO_UINT(LEVEL_ID::SPAWN), strLayerTag, pEventTrigger)))
            return E_FAIL;

    }

    return S_OK;
}

HRESULT CLevel_Spawn::Ready_Layer_Particle(const _wstring& strLayerTag)
{
   
    return S_OK;
}

HRESULT CLevel_Spawn::Ready_Player_Static(const _wstring& strLayerTag)
{
    if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(ENUM_TO_UINT(LEVEL_ID::STATIC),
        PROTO_OBJ_NAME(L"Player"),
        ENUM_TO_UINT(LEVEL_ID::STATIC),
        strLayerTag, nullptr)))
        return E_FAIL;

    CGameObject* pObj = m_pGameInstance->Find_GameObject(ENUM_TO_UINT(LEVEL_ID::STATIC), L"Player_Layer", L"Player");
    if (pObj)
    {
        m_pGameManager->Set_MainPlayer(pObj);
        CInteraction_Manager::GetInstance()->Set_MainPlayer(pObj);

    }

    /// <summary>
    /// 
    /// </summary>
    /// <param name="strLayerTag"></param>
    /// <returns></returns>
    /// 
  

    return S_OK;
}

HRESULT CLevel_Spawn::Ready_UI_Static(const _wstring& strLayerTag)
{
    if (FAILED(UICreator::Create_HeartGroup(strLayerTag)))
        return E_FAIL;

    /////Interaction 관련 PopUp UI
    if (FAILED(UICreator::Create_Interaction_UI(strLayerTag)))
        return E_FAIL;
    /////Interaction 관련 PopUp UI
    if (FAILED(UICreator::Create_Interaction_TalkUI(strLayerTag)))
        return E_FAIL;

    if (FAILED(UICreator::Create_NPC_Dialogue_UI(strLayerTag)))
        return E_FAIL;

    return S_OK;
}

void CLevel_Spawn::EndCutScene()
{
    CLayer* pNPCLayer = m_pGameInstance->Find_Layer(m_iLevelID, L"NPC_Layer");
    CheckNull(pNPCLayer);


    CGameObject* pMarin = pNPCLayer->Find_GameObject(L"NPC_Marin");
    CheckNull(pMarin);

    CNPC* pMarinNPC = dynamic_cast<CNPC*>(pMarin);
    CheckNull(pMarinNPC);

    pMarinNPC->Set_TalkRange(0.2f);

}

void CLevel_Spawn::Play_LevelBGM()
{
    m_pGameInstance->PlayBGM(L"BGM/First_Spawn.wav", g_BGMVolume);

}

void CLevel_Spawn::OnEnter()
{

    __super::OnEnter();


    m_pGameInstance->Set_MainCamera(CAMERA_TYPE::TARGET);
    CPlayer* m_pPlayer = CGameManager::GetInstance()->Get_MainPlayer();

    CGameObject* pMainCamera = m_pGameInstance->Find_Camera(CAMERA_TYPE::TARGET);
    if (pMainCamera)
    {                        
        CMainCamera* ppMainCamera = dynamic_cast<CMainCamera*>(pMainCamera);
        CheckNull(ppMainCamera);
        ppMainCamera->Set_Target(m_pPlayer, true);
    }

    //플레이어 위치
    _float4 vSpawnPos = _float4(10.25f, 0.f, 7.7f,1.f);
    m_pPlayer->Get_Transform()->Set_State(STATE::POSITION, XMLoadFloat4(&vSpawnPos));
    m_pPlayer->Get_Transform()->Rotation(_float3(0.f, 90.f, 0.f));


    m_pPlayer->Change_MainNavMesh();

    /// <summary>
    /// ////
    /// </summary>
    //m_pPlayer->Show_Weapons();

    //////현재씬의 itneraction 등록 
    CInteraction_Manager::GetInstance()->Change_Scene(ENUM_TO_UINT(LEVEL_ID::SPAWN));

    //1초후에 페이드아웃 
    m_pGameInstance->Invoke(2.f, 0.f, false, false, []()
        {
            UIGroup* pGroup = CGameInstance::GetInstance()->Get_UIGroup(L"FadeScreenGroup");
            CFadeScreen* pFadeScreen = dynamic_cast<CFadeScreen*>(pGroup->Find(L"FadeScreen"));

            pFadeScreen->PlayFadeOut();

        }, m_pPlayer);
 

  
    m_pGameInstance->PlayBGM(L"BGM/First_Spawn.wav", g_BGMVolume);

  
    return;



}

void CLevel_Spawn::OnResume(_uint iPreLevel)
{
}

void CLevel_Spawn::OnPause(_uint iNextLeve)
{
}

void CLevel_Spawn::OnExit()
{
    __super::OnExit();


    CInteraction_Manager::GetInstance()->Clear();
    m_pGameInstance->Clear_SceneColliders(m_iLevelID);

}

CLevel_Spawn* CLevel_Spawn::Create(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pDeviceContext, LevelArgs& args)
{
    CLevel_Spawn* pInstance = new CLevel_Spawn(_pDevice, _pDeviceContext);
    if (FAILED(pInstance->Initialize(args)))
    {
        MSG_BOX("Failed to Create : CLevel_Spawn");
        Safe_Release(pInstance);
    }


    return pInstance;
}

void CLevel_Spawn::Free()
{
    __super::Free();

}
