#include "CLevel_Town.h"
#include "CLoader.h"
#include "CBackGround.h"
#include "CMainCamera.h"
#include "CTerrain.h"

#include "CMonster.h"
#include "CMonster_Body.h"
#include "CInput_Manager.h"
#include "CLight.h"

#include "CStateDebugWindow.h"
#include "CImGui_Manager.h"
#include "CPlayer.h"

#include "MathUtils.h"
#include "CNPC_Richard.h"
#include "CIInteractable.h"
#include "CNPC_Fairy.h"
#include "CNPC_Kid_Blue.h"


#include "CInteraction_Manager.h"
#include "CGameManager.h"

#include "CMapLoader.h"
#include "CLayer.h"
#include "CUICreator.h"
#include "CExplosion.h"

#include "CRoom_Manager.h"
#include "CFadeScreen.h"

#include "CRoomTrigger.h"
#include "CQuest_Manager.h"
#include "CEventTrigger.h"

#include "CBoxColliderComponent.h"
#include "CBounding_AABB.h"
#include "CFontUI.h"
#include "CCell.h"
#include "ColorUtils.h"

#include "CNPC_Tarin.h"
#include "CNavigation.h"
#include "CWeatherCock.h"

#include "CInventory_Manager.h"
#include "CWall.h"


USING(Client)

CLevel_Town::CLevel_Town(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pDeviceContext)
    :CLevel(_pDevice,_pDeviceContext)
{
}

HRESULT CLevel_Town::Initialize(LevelArgs& args)
{
    __super::Initialize(args);

    m_pGameManager = CGameManager::GetInstance();
    if (FAILED(Ready_Lights()))
        return E_FAIL;

    //Terrain파싱
    if (FAILED(Ready_Layer_Enviroment(L"Enviroment_Layer")))
        return E_FAIL;

    if (FAILED(Ready_Layer_Player(L"Player_Layer")))
        return E_FAIL;

    if (FAILED(Ready_Layer_NPC(L"NPC_Layer")))
        return E_FAIL;

    if (FAILED(Ready_Layer_Monster(L"Monster_Layer")))
        return E_FAIL;  

    if (FAILED(Ready_Layer_UI(L"UI_Layer")))
        return E_FAIL;

    if (FAILED(Ready_Layer_InteractionObject(L"Interaction_Layer")))
        return E_FAIL;

   /* if (FAILED(Ready_Layer_Particle(L"Particle_Layer")))
        return E_FAIL;*/

    if (FAILED(Ready_Layer_Trigger(L"Trigger_Layer")))
        return E_FAIL;

    if (FAILED(Ready_EventListners()))
        return E_FAIL;


    UIGroup* pGroup = m_pGameInstance->Get_UIGroup(L"FadeScreenGroup");
    pFadeScreen = dynamic_cast<CFadeScreen*>(pGroup->Find(L"FadeScreen"));



    /*이후에 바로 Spawnscene으로 가기*/
    return S_OK;
}

void CLevel_Town::Update_Priority(_float fTimeDelta)
{
    __super::Update_Priority(fTimeDelta);

    //엔딩씬 테스트
    if (CInput_Manager::GetInstance()->IsKeyPressed(KeyCode::E))
    {

        pFadeScreen->Set_FadeInEndFunc([this]()
            {
                LevelArgs args;
                args.iNextLevelID = ENUM_TO_UINT(LEVEL_ID::ENDING);
                args.changeType = LEVELCHANGETYPE::PUSH;
                args.loadingChangeType = LEVELCHANGETYPE::PUSH;
                args.m_iLevelID = ENUM_TO_UINT(LEVEL_ID::LOADING);

                if (FAILED(m_pGameInstance->Level_Changer(
                    ENUM_TO_UINT(LEVEL_ID::LOADING),
                    args)))
                    return;

            });

        pFadeScreen->PlayFadeIn();
    }


    if (CInput_Manager::GetInstance()->IsKeyPressed(KeyCode::R))
    {

        pFadeScreen->Set_FadeInEndFunc([this]()
            {
                LevelArgs args;
                args.iNextLevelID = ENUM_TO_UINT(LEVEL_ID::DUNGEON);
                args.changeType = LEVELCHANGETYPE::PUSH;
                args.loadingChangeType = LEVELCHANGETYPE::PUSH;
                args.m_iLevelID = ENUM_TO_UINT(LEVEL_ID::LOADING);

      
                if (FAILED(m_pGameInstance->Level_Changer(
                    ENUM_TO_UINT(LEVEL_ID::LOADING),
                    args)))
                    return;

            });

        pFadeScreen->PlayFadeIn();
    }
   
   
}

void CLevel_Town::Update(const _float fTimeDelta)
{

    __super::Update(fTimeDelta);

    CGameObject* pTestObject = m_pGameInstance->Find_GameObject(ENUM_TO_UINT(LEVEL_ID::TOWN), L"UI_Layer", L"Player_Marker");
    CGameObject* pPlayerObject = m_pGameInstance->Find_GameObject(ENUM_TO_UINT(LEVEL_ID::STATIC), L"Player_Layer", L"Player");

    if (pTestObject)
    {
        pTestObject->Get_Transform()->Rotation(_float3(90.f, 0.f, 0.f));
        _vector vPos = pPlayerObject->Get_Transform()->Get_State(STATE::POSITION, TransformScope::WORLD);
        pTestObject->Get_Transform()->Set_State(STATE::POSITION, vPos);

    }
     
    if(CInput_Manager::GetInstance()->IsKeyPressed(KeyCode::M))
    {
        CGameObject* pObj = m_pGameInstance->Find_GameObject(m_iLevelID,
            L"UI_Layer", L"MinimapQuad");

        if (pObj)
        {
            if (pObj->Is_Active())
            {
                pObj->Set_Active(false);
                m_pGameInstance->Set_EnableUpdateMinimap(false);
            }
            else
            {
                pObj->Set_Active(true);
                m_pGameInstance->Set_EnableUpdateMinimap(true);
            }
        }
    }


    return;

}

void CLevel_Town::Update_Late(_float fTimeDelta)
{
    __super::Update_Late(fTimeDelta);

    Change_Area();
    /*점조명테스트*/
    if (CInput_Manager::GetInstance()->IsKeyHeld(KeyCode::PageUp))
    {
        CLight* pLight = m_pGameInstance->Get_Light(m_iLevelID, 0);
        LIGHT_DESC LightDesc = (*pLight->Get_LightDesc());
        LightDesc.vPosition.y += 0.1f;
        pLight->Set_LightDesc(LightDesc);

    }

    if (CInput_Manager::GetInstance()->IsKeyHeld(KeyCode::PageDown))
    {
        CLight* pLight = m_pGameInstance->Get_Light(m_iLevelID, 0);
        LIGHT_DESC LightDesc = (*pLight->Get_LightDesc());
        LightDesc.vPosition.y -= 0.1f;
        pLight->Set_LightDesc(LightDesc);

    }


}

void CLevel_Town::Render()
{
    //UI렌더. (로딩바)
    wchar_t szTitle[256];
    swprintf_s(szTitle, L"Town 씬입니다. FPS : %.1f", m_pGameInstance->Get_FPS(L"Timer_60"));

    SetWindowText(g_hWnd, szTitle);
}

HRESULT CLevel_Town::Ready_Lights()
{ 
    LIGHT_DESC      LightDesc{};
    LightDesc.eType = LIGHT::DIRECTIONAL;
    LightDesc.vDirection = _float4(1.f, -1.f, 1.f, 0.f);
    LightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
    LightDesc.vAmbient = _float4(0.7f, 0.7f, 0.7f, 1.f);
    LightDesc.vSpecular = _float4(0.5f, 0.5f, 0.5f, 1.f);

    if (FAILED(m_pGameInstance->Add_Light(m_iLevelID, LightDesc)))
        return E_FAIL;

    //////////////물 조명//////////
    LIGHT_DESC       WaterLight;
    WaterLight.eType = LIGHT::POINT;

    WaterLight.vDiffuse = COLOR_BLUE;
    WaterLight.fRange = _float4(3.f, 0.f, 0.f, 1.f);
    WaterLight.vPosition = _float4(45.f,8.f, 90.f, 1.f);
    WaterLight.vSpecular = WaterLight.vDiffuse;
    WaterLight.vAmbient = _float4(0.1f, 0.3f, 0.1f, 1.f);


    if (FAILED(m_pGameInstance->Add_Light(m_iLevelID, WaterLight)))
        return E_FAIL;


    return S_OK;
}


HRESULT CLevel_Town::Ready_Layer_Enviroment(const _wstring& strLayerTag)
{

    m_pGameInstance->Set_MainCells(ENUM_TO_UINT(LEVEL_ID::TOWN));
    CGameInstance::GetInstance()->Set_EnalbeUpdateRender(true);

   //30.10.37
    CWeatherCock::MODELOBJECT_DESC ModelDesc;
    ModelDesc.m_iLevelID = m_iLevelID;
    ModelDesc.ObjTag = L"WeatherCock";

    CTransform::tagTransformDesc TransDesc;
    TransDesc.vLocalPosition = _float4(30.8f,10.f,39.f,1.f);
    TransDesc.vLocalScale = _float4(1.5f, 1.5f,1.5f, 1.f);

    ModelDesc.TransformDesc = &TransDesc;

    CGameObject* pObj = CWeatherCock::Create(m_pDevice, m_pContext,&ModelDesc);
    if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(m_iLevelID, strLayerTag, pObj)))
        return E_FAIL;

    return S_OK;
}


HRESULT CLevel_Town::Ready_Layer_UI(const _wstring& strLayerTag)
{
   
    //if(FAILED(UICreator::Create_HeartGroup(strLayerTag)))
    //    return E_FAIL;

    ///////Interaction 관련 PopUp UI
    //if (FAILED(UICreator::Create_Interaction_UI(strLayerTag)))
    //    return E_FAIL;
    ///////Interaction 관련 PopUp UI
    //if (FAILED(UICreator::Create_Interaction_TalkUI(strLayerTag)))
    //    return E_FAIL;

#pragma region MinimapQuad
    ///////////////////Minimapquad생성
    CUI::tagUIDesc        MinimapDesc = {};
    MinimapDesc.ObjTag = L"MinimapQuad";
    MinimapDesc.passName = "Blur";
    MinimapDesc.eRenderGroup = ENUM_TO_UINT(RENDERGROUP::UI);
    MinimapDesc.fSizeX = 150;
    MinimapDesc.fSizeY = 150;
    MinimapDesc.fX = g_iWinSizeX - 100;
    MinimapDesc.fY = 100;
    MinimapDesc.m_iLevelID = m_iLevelID;

    CTransform::TRANSFORM_DESC MinimapTransDesc = {};
    MinimapDesc.TransformDesc = &MinimapTransDesc;

    if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(ENUM_TO_UINT(LEVEL_ID::STATIC),
        PROTO_OBJ_NAME(L"MinimapQuad"),
        ENUM_TO_UINT(LEVEL_ID::TOWN),
        strLayerTag, &MinimapDesc)))
        return E_FAIL;


    /*플레이어 마커 생성*/
    CQuad::QUAD_DESC        Desc = {};

    Desc.ObjTag = L"Player_Marker";
    Desc.TextureKey = L"Player_Marker";
    Desc.eRenderGroup = ENUM_TO_UINT(RENDERGROUP::WORLD_UI_MINIMAP);
    Desc.m_iLevelID = m_iLevelID;


    CTransform::TRANSFORM_DESC TransDesc = {};
    TransDesc.fRotationPerSec = 10.f;
    TransDesc.fSpeedPerSec = 5.f;
    TransDesc.vLocalPosition = { 0.f,0.f,0.f,1.f };
    TransDesc.vLocalScale = { 5.f,5.f,1.f,1.f };

    Desc.TransformDesc = &TransDesc;

    if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(ENUM_TO_UINT(LEVEL_ID::STATIC),
        PROTO_OBJ_NAME(L"Quad"),
        ENUM_TO_UINT(LEVEL_ID::TOWN),
        strLayerTag, &Desc)))
        return E_FAIL;
#pragma endregion

    return S_OK;

}

HRESULT CLevel_Town::Ready_Layer_Player(const _wstring& strLayerTag)
{

    /*if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(ENUM_TO_UINT(LEVEL_ID::STATIC),
        PROTO_OBJ_NAME(L"Player"),
        ENUM_TO_UINT(LEVEL_ID::STATIC),
        strLayerTag, nullptr)))
        return E_FAIL;

    CGameObject* pObj = m_pGameInstance->Find_GameObject(ENUM_TO_UINT(LEVEL_ID::STATIC), L"Player_Layer", L"Player");
    if (pObj)
    {
        m_pGameManager->Set_MainPlayer(pObj);
        CInteraction_Manager::GetInstance()->Set_MainPlayer(pObj);

    }*/
    return S_OK;

}

HRESULT CLevel_Town::Ready_Layer_Monster(const _wstring& strLayerTag)
{  

    //생성 셀번호 668,925,1033,857
    
    //몬스터 생성루틴
#pragma region 초록색슬라임
   /* CMonster::MonsterDesc desc;

    CMonster_Body::MONSTER_BODY_DESC bodyDesc;
    bodyDesc.eRenderGroup = ENUM_TO_UINT(RENDERGROUP::NONALPHA);
    bodyDesc.modelName = L"ZolGreenAnim";

    desc.BodyDesc = &bodyDesc;

     
    desc.iAttack = 10;
    desc.MaxHp = 1;
    desc.fActionRange = 10.f;
    desc.m_iLevelID = m_iLevelID;

    desc.ObjTag = L"CM_GreenZol" + to_wstring(0);
    CTransform::TRANSFORM_DESC TransDesc = {};
    TransDesc.vLocalPosition = { 20.108f,10.5f,27.893f,1.f };
    TransDesc.vLocalRotation = { 0.f,180.f,0.f,1.f };

    TransDesc.fSpeedPerSec = 3.f;
    TransDesc.fRotationPerSec = 10.f;

    desc.TransformDesc = &TransDesc;



    if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(ENUM_TO_UINT(LEVEL_ID::STATIC),
        PROTO_OBJ_NAME(L"CM_GreenZol"),
        ENUM_TO_UINT(LEVEL_ID::TOWN),
        strLayerTag, &desc)))
        return E_FAIL;*/

    
#pragma endregion
    ///////////////////////////
    _uint SpawnIdx[4] = { 686,866,1026,947 };
    _float RoamRadius[4] = { 10,5.f,10.f,10.f };


    vector<CCell*>* m_Cells = m_pGameInstance->Get_MainCells();
    CheckTrueResult(m_Cells->empty(),E_FAIL);

    for (int i = 0; i < 4; ++i)
    {
        _vector vPos = (*m_Cells)[SpawnIdx[i]]->Get_CenterPos();
        CMonster::MonsterDesc Moriblindesc;

        CMonster_Body::MONSTER_BODY_DESC MoriblinbodyDesc;
        MoriblinbodyDesc.eRenderGroup = ENUM_TO_UINT(RENDERGROUP::NONALPHA);
        MoriblinbodyDesc.modelName = L"MoriblinSword";
        MoriblinbodyDesc.m_iLevelID = m_iLevelID;

        Moriblindesc.BodyDesc = &MoriblinbodyDesc;
        

        Moriblindesc.iAttack = 10;
        Moriblindesc.MaxHp = 3;
        Moriblindesc.fActionRange = 3.f;
        Moriblindesc.m_iLevelID = m_iLevelID;
        Moriblindesc.RoamRadius = RoamRadius[i];

        //첫 스폰위치
        Moriblindesc.iHomeIdx = SpawnIdx[i];

        Moriblindesc.ObjTag = L"MoriblinSword" + to_wstring(i);
        CTransform::TRANSFORM_DESC MoriblinTransDesc = {};
        
        MoriblinTransDesc.vLocalRotation = { 0.f,180.f,0.f,1.f };
        XMStoreFloat4(&MoriblinTransDesc.vLocalPosition, vPos);

        MoriblinTransDesc.fSpeedPerSec = 2.f;
        MoriblinTransDesc.fRotationPerSec = 10.f;

        Moriblindesc.TransformDesc = &MoriblinTransDesc;



        if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(ENUM_TO_UINT(LEVEL_ID::STATIC),
            PROTO_OBJ_NAME(L"CM_MoriblinSword"),
            ENUM_TO_UINT(LEVEL_ID::TOWN),
            strLayerTag, &Moriblindesc)))
            return E_FAIL;

        CGameObject* pFindMonster = m_pGameInstance->Find_GameObject(ENUM_TO_UINT(LEVEL_ID::TOWN),
            L"Monster_Layer", Moriblindesc.ObjTag);

        if (pFindMonster)
            pFindMonster->Set_Active(false);

    }
  

#pragma region gidbos Test
    // CMonster::MonsterDesc desc;

    //CMonster_Body::MONSTER_BODY_DESC bodyDesc;
    //bodyDesc.eRenderGroup = ENUM_TO_UINT(RENDERGROUP::NONALPHA);
    //bodyDesc.modelName = L"Gidbos";

    //desc.BodyDesc = &bodyDesc;


    //desc.iAttack = 10;
    //desc.MaxHp = 2;
    //desc.fActionRange = 10.f;
    //desc.m_iLevelID = m_iLevelID;

    //desc.ObjTag = L"Gidbos" + to_wstring(0);
    //CTransform::TRANSFORM_DESC TransDesc = {};
    //TransDesc.vLocalPosition = { 11.5f,10.5f,22.893f,1.f };
    //TransDesc.vLocalRotation = { 0.f,90.f,0.f,1.f };

    //TransDesc.fSpeedPerSec = 3.f;
    //TransDesc.fRotationPerSec = 10.f;

    //desc.TransformDesc = &TransDesc;



    //if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(ENUM_TO_UINT(LEVEL_ID::STATIC),
    //    PROTO_OBJ_NAME(L"CM_Gidbos"),
    //    ENUM_TO_UINT(LEVEL_ID::TOWN),
    //    strLayerTag, &desc)))
    //    return E_FAIL;

#pragma endregion
    return S_OK;

}

HRESULT CLevel_Town::Ready_Layer_NPC(const _wstring& strLayerTag)
{
   /* CNPC::NPC_DESC pDesc;

    CTransform::TRANSFORM_DESC pTransDesc;
    pTransDesc.vLocalPosition = { 30.642f,10.421f,23.642f,1.f };
    pDesc.ObjTag = L"NPC_Richard";
    pDesc.pTarget = nullptr;
    pDesc.ModelName = L"RichardAnim";
    pDesc.SceneName = "Level_Town";
    pDesc.m_iLevelID = m_iLevelID;
    pDesc.TalkRange = 2.f;


    pDesc.TransformDesc = &pTransDesc;

    CNPC_Richard* pNpc_Richard = CNPC_Richard::Create(m_pDevice, m_pContext, &pDesc);
    if (pNpc_Richard)
    {
        if(FAILED(m_pGameInstance->Add_GameObject_To_Layer(ENUM_TO_UINT(LEVEL_ID::TOWN), strLayerTag, pNpc_Richard)))
        return E_FAIL;

    }*/


    //길거리에 있는 애기
    CNPC::NPC_DESC pDesc;

    CTransform::TRANSFORM_DESC pTransDesc;


    _vector vPos = m_pGameInstance->Get_CellPos_By_MainCells(538);
    XMStoreFloat4(&pTransDesc.vLocalPosition, vPos);
    pTransDesc.vLocalRotation = _float4(0.f, 180.f, 0.f, 0.f);

    pDesc.ObjTag = L"NPC_Kid_Green";
    pDesc.pTarget = nullptr;
    pDesc.ModelName = L"Kid_Green";
    pDesc.SceneName = "Level_Town";
    pDesc.m_iLevelID = m_iLevelID;
    pDesc.TalkRange = 3.f;


    _uint        iExpressionIdxEye[CNPC::EXPRESSION::END] = { 0,2,4,3 };
    _uint        iExpressionIdxMouth[CNPC::EXPRESSION::END] = { 0,2,3,3 };
    _uint        iMouthOpen[CNPC::EXPRESSION::END] = { 1,1,3,3 };

    memcpy(pDesc.iExpressionIdxEye, iExpressionIdxEye, sizeof(_uint) * CNPC::EXPRESSION::END);
    memcpy(pDesc.iExpressionIdx_Mouth, iExpressionIdxMouth, sizeof(_uint) * CNPC::EXPRESSION::END);
    memcpy(pDesc.iOpenIdx_Mouth, iMouthOpen, sizeof(_uint) * CNPC::EXPRESSION::END);


    pDesc.TransformDesc = &pTransDesc;

    CNPC* pNpc_GreenKid = CNPC::Create(m_pDevice, m_pContext, &pDesc);
    if (pNpc_GreenKid)
    {
        if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(ENUM_TO_UINT(LEVEL_ID::TOWN), strLayerTag, pNpc_GreenKid)))
            return E_FAIL;

    }

    ////////요정
    CNPC::NPC_DESC pFairyDesc;

    CTransform::TRANSFORM_DESC pFairyTransDesc;


    pFairyTransDesc.vLocalPosition = _float4(45.f, 12.65f, 88.94f, 1.f);
    pFairyTransDesc.vLocalRotation = _float4(0.f, 180.f, 0.f, 0.f);
    pFairyTransDesc.vLocalScale = _float4(0.8f, 0.8f, 0.8f, 1.f);

    pFairyDesc.ObjTag = L"NPC_Fairy";
    pFairyDesc.pTarget = nullptr;
    pFairyDesc.ModelName = L"Fairy";
    pFairyDesc.SceneName = "Level_Town";
    pFairyDesc.m_iLevelID = m_iLevelID;
    pFairyDesc.TalkRange =   4.f;
    pFairyDesc.bUseNavMesh = false;



    pFairyDesc.TransformDesc = &pFairyTransDesc;

    CNPC_Fairy* pNpc_Fairy = CNPC_Fairy::Create(m_pDevice, m_pContext, &pFairyDesc);
    if (pNpc_Fairy)
    {
        if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(ENUM_TO_UINT(LEVEL_ID::TOWN), strLayerTag, pNpc_Fairy)))
            return E_FAIL; 

        //pNpc_Fairy->Set_Active(false);
    }

    /// <summary>
    /// /숲애기
    CNPC::NPC_DESC pKid_Blue_Desc;

    CTransform::TRANSFORM_DESC pKid_Blue_TransDesc;

    vPos = m_pGameInstance->Get_CellPos_By_MainCells(940);
    XMStoreFloat4(&pKid_Blue_TransDesc.vLocalPosition, vPos);
    pKid_Blue_TransDesc.vLocalRotation = _float4(0.f, 180.f, 0.f, 0.f);
    pKid_Blue_TransDesc.vLocalScale = _float4(0.8f, 0.8f, 0.8f, 1.f);

    pKid_Blue_Desc.ObjTag = L"NPC_Kid_Blue";
    pKid_Blue_Desc.pTarget = nullptr;
    pKid_Blue_Desc.ModelName = L"Kid_Blue";
    pKid_Blue_Desc.SceneName = "Level_Town";
    pKid_Blue_Desc.m_iLevelID = m_iLevelID;
    pKid_Blue_Desc.TalkRange = 1.f;
  
    _uint Kid_Blue_iExpressionIdxEye[CNPC::EXPRESSION::END] = { 0,2,4,3};
    _uint Kid_Blue_iExpressionIdxMouth[CNPC::EXPRESSION::END] = { 0,2,3,3 };
    _uint Kid_Blue_iMouthOpen[CNPC::EXPRESSION::END] = { 1,1,3,3 };

    memcpy(pKid_Blue_Desc.iExpressionIdxEye, Kid_Blue_iExpressionIdxEye, sizeof(_uint) * CNPC::EXPRESSION::END);
    memcpy(pKid_Blue_Desc.iExpressionIdx_Mouth, Kid_Blue_iExpressionIdxMouth, sizeof(_uint) * CNPC::EXPRESSION::END);
    memcpy(pKid_Blue_Desc.iOpenIdx_Mouth, Kid_Blue_iMouthOpen, sizeof(_uint) * CNPC::EXPRESSION::END);


    pKid_Blue_Desc.TransformDesc = &pKid_Blue_TransDesc;
    CNPC_Kid_Blue* pNPC_Kid = CNPC_Kid_Blue::Create(m_pDevice, m_pContext, &pKid_Blue_Desc);
    if (pNPC_Kid)
    {
        if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(ENUM_TO_UINT(LEVEL_ID::TOWN), strLayerTag, pNPC_Kid)))
            return E_FAIL;

        pNPC_Kid->Set_Active(false);
    }

    /// <summary>
    /// 이벤트전용  Tarin따로생성
    CNPC::NPC_DESC pTarin;

    CTransform::TRANSFORM_DESC pTarinTransDesc;

    _vector vTarinPos = m_pGameInstance->Get_CellPos_By_MainCells(23);
    XMStoreFloat4(&pTarinTransDesc.vLocalPosition, vTarinPos);

    pTarinTransDesc.vLocalRotation = _float4(0.f, 90.f, 0.f, 0.f);
    pTarinTransDesc.fSpeedPerSec = 2.f;

    pTarin.ObjTag = L"NPC_Tarin";
    pTarin.pTarget = nullptr;
    pTarin.ModelName = L"Tarin";          
    pTarin.SceneName = "Level_Town";
    pTarin.m_iLevelID = m_iLevelID;
    pTarin.TalkRange =4.f;
  //  pTarin.bUseNavMesh = false;



    pTarin.TransformDesc = &pTarinTransDesc;

    CNPC_Tarin* pNpc_Tarin = CNPC_Tarin::Create(m_pDevice, m_pContext, &pTarin);
    if (pNpc_Tarin)
    {
        if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(ENUM_TO_UINT(LEVEL_ID::TOWN), strLayerTag, pNpc_Tarin)))
            return E_FAIL;

        pNpc_Tarin->Set_Active(false);
    }

    return S_OK;
}

HRESULT CLevel_Town::Ready_Layer_InteractionObject(const _wstring& strLayerTag)
{


    if (FAILED(m_pGameInstance->Make_New_Layer(ENUM_TO_UINT(LEVEL_ID::TOWN), strLayerTag)))
        return E_FAIL;


    CLayer* pInteractionLayer = m_pGameInstance->Find_Layer(ENUM_TO_UINT(LEVEL_ID::TOWN), strLayerTag);
    CMapLoader::Make_Object_By_LoadData("Level_Town", pInteractionLayer);



    ////버섯 생짜생성..
    //CInteractionObject::Interaction_DESC Desc;
    //Desc.eInteractionType = ENUM_TO_UINT(InteractionType::OBJECT);
    //Desc.eInteract_Object_Type = Interact_Object_Type::ITEM;
    //Desc.m_iLevelID = ENUM_TO_UINT(LEVEL_ID::TOWN);

    //Desc.eRenderGroup = ENUM_TO_UINT(RENDERGROUP::NONALPHA);
    //Desc.ModelName = L"Mushroom";

    //Desc.SceneName = "Level_Town";
    //Desc.bAnimated = false;
    //Desc.ObjTag = L"Mushroom";

    //CTransform::TRANSFORM_DESC TransDesc;
    //vector<CCell*>* m_Cells = m_pGameInstance->Get_MainCells();
    //CheckNullResult(m_Cells, E_FAIL);
    //CCell* pSpawnCell = (*m_Cells)[0];
    //_vector vSpawnPoint = pSpawnCell->Get_CenterPos();
    //XMStoreFloat4(&TransDesc.vLocalPosition, vSpawnPoint);
    //Desc.TransformDesc = &TransDesc;


    //CBoxColliderComponent::COLLIDER_DESC ColDesc;
    //ColDesc.m_eColGroup = ENUM_TO_UINT(COLLISION_GROUP::INTERACTION);
    //CBounding_AABB::BOUNDING_AABB_DESC aabbDesc;
    //aabbDesc.Extents = _float3(0.5f, 0.5f, 0.5f);
    //ColDesc.m_BoundingDesc = &aabbDesc;
    //ColDesc.m_iLevelID = m_iLevelID;
    //Desc.pColliderComp = &ColDesc;


    //if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(ENUM_TO_UINT(LEVEL_ID::STATIC),
    //    PROTO_OBJ_NAME(L"Interaction_Rock"),
    //    ENUM_TO_UINT(LEVEL_ID::TOWN),
    //    strLayerTag, &Desc)))
    //    return E_FAIL;

    return S_OK;
}

HRESULT CLevel_Town::Ready_Layer_Particle(const _wstring& strLayerTag)
{
    CGameObject::GAMEOBJECT_DESC Desc;
    Desc.m_iLevelID = m_iLevelID;

    CTransform::TRANSFORM_DESC TransDesc;

    CPlayer* pPlayer = m_pGameManager->Get_MainPlayer();

    XMStoreFloat4(&TransDesc.vLocalPosition, pPlayer->Get_Transform()->Get_State(STATE::POSITION));
    Desc.TransformDesc = &TransDesc;

    if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(ENUM_TO_UINT(LEVEL_ID::STATIC),
        PROTO_OBJ_NAME(L"Snow"),
        ENUM_TO_UINT(LEVEL_ID::TOWN),
        strLayerTag,&Desc)))
        return E_FAIL;



    if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(ENUM_TO_UINT(LEVEL_ID::STATIC),
        PROTO_OBJ_NAME(L"Explosion"),
        ENUM_TO_UINT(LEVEL_ID::TOWN),
        strLayerTag, &Desc)))
        return E_FAIL;

    return S_OK;
}

HRESULT CLevel_Town::Ready_Layer_Trigger(const _wstring& strLayerTag)
{
    /// <summary>
    /// 썡으로배치..
    wstring TriggerTags[] = { L"mamasha_House1",L"mamasha_House2",L"TelephoneBox",L"MarinHouse",L"RichardHouse"};
    string NextKeys[] = { "Mamasha_room","Mamasha_room","telephoneBox","MarinHouse","RichardHouse"};

    _float4 TriggerPos[] = {
        _float4(30.75f,12.86f,51.f,1.f),
        _float4(33.66f,12.86f,51.f,1.f),
        _float4(30.81f,10.5,15.746f,1.f),
        _float4(30.63f,10.577f,27.19f,1.f),
        _float4(80.f,13.8f,100.8f,1.f)


    };

    size_t Size = sizeof(TriggerTags) / sizeof(TriggerTags[0]);
    for (size_t i = 0; i <Size ; ++i)
    {
        CRoomTrigger::RoomTriggerDesc RoomTriggerDesc;
        RoomTriggerDesc.vCenter = _float3(0.f, 0.f, 0.f);
        RoomTriggerDesc.vExtents = _float3(0.8f,0.5f, 0.8f);
        RoomTriggerDesc.ObjTag = L"Trigger" + TriggerTags[i];
        RoomTriggerDesc.m_nextKey = NextKeys[i];
        RoomTriggerDesc.m_iLevelID = m_iLevelID;

        CTransform::TRANSFORM_DESC TransDesc;
        TransDesc.vLocalPosition = _float4(TriggerPos[i].x, TriggerPos[i].y, TriggerPos[i].z, 1.f);
        RoomTriggerDesc.TransformDesc = &TransDesc;

        if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(ENUM_TO_UINT(LEVEL_ID::STATIC), PROTO_OBJ_NAME(L"RoomTrigger"), ENUM_TO_UINT(LEVEL_ID::TOWN), strLayerTag, &RoomTriggerDesc)))
            return E_FAIL;


    }

    //숲속 카메라트리거
#pragma region 필드카메라
    CEventTrigger::EventTriggerDesc DefaultEventDesc;
    DefaultEventDesc.vCenter = _float3(0.f, 0.f, 0.f);
    DefaultEventDesc.vExtents = _float3(0.8f, 0.5f, 3.f);
    DefaultEventDesc.ObjTag = L"Default_Trigger";
    DefaultEventDesc.m_iLevelID = m_iLevelID;

    CTransform::TRANSFORM_DESC DefaultEventTransform;
    DefaultEventTransform.vLocalPosition = _float4(2.198f, 12.79f, 40.188f, 1.f);

    DefaultEventDesc.TransformDesc = &DefaultEventTransform;

    DefaultEventDesc.EnterFunc = [this]()
    {
        m_eArea = Area::TOWN;
    };



    DefaultEventDesc.StayFunc = [this]()
    {
        m_Chapter = L"Default";
        CCamera_Base* pCamBase = m_pGameInstance->Find_Camera(CAMERA_TYPE::TARGET);
        CheckNull(pCamBase);

        CMainCamera* pMaincam = dynamic_cast<CMainCamera*>(pCamBase);
        CheckNull(pMaincam);

        _float3 vRotation = _float3(56.f, 0.f, 0.f);
        _float3 vOffSet = _float3(0.f, 9.f, -6.f);

        _float3 vCurFloatRot = pMaincam->Get_Transform()->Get_Rotation_ByEular();
        _float3 vCurFloatOffset = pMaincam->Get_Offset();

        _vector vCurRot = XMLoadFloat3(&vCurFloatRot);
        _vector vCurOffSet = XMLoadFloat3(&vCurFloatOffset);

        _vector fRotation = XMVectorLerp(vCurRot, XMLoadFloat3(&vRotation), 0.02f);
        _vector vOffset = XMVectorLerp(vCurOffSet, XMLoadFloat3(&vOffSet), 0.02f);

        

        _float4 vResult;
        _float3 fOffSet;
        XMStoreFloat4(&vResult, fRotation);
        XMStoreFloat3(&fOffSet, vOffset);
        
        pMaincam->Set_LocalRoation(vResult);
        pCamBase->Set_Offset(fOffSet);
        
        pCamBase->Set_TargetOffset (vOffSet);
        pCamBase->Set_TargetRotation(vRotation);


        //directionlight조절
        //CLight* pDirectionLight = m_pGameInstance->Get_DirectionLight(m_iLevelID);
        //CheckNull(pDirectionLight);

        //LIGHT_DESC NewLightDesc = *pDirectionLight->Get_LightDesc();
        //_vector vDiffuseColor = XMLoadFloat4(&NewLightDesc.vDiffuse);
        //_vector vAmbient = XMLoadFloat4(&NewLightDesc.vAmbient);
        //_vector vSpecular = XMLoadFloat4(&NewLightDesc.vSpecular);

        ///*  LightDesc.vDirection = _float4(1.f, -1.f, 1.f, 0.f);
        //LightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
        //LightDesc.vAmbient = _float4(0.5f, 0.5f, 0.5f, 1.f);
        //LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);*/

        //XMStoreFloat4(&NewLightDesc.vDiffuse, XMVectorLerp(vDiffuseColor, XMVectorSet(1.f,1.f, 1.f, 1.f), 0.2f));
        //XMStoreFloat4(&NewLightDesc.vAmbient, XMVectorLerp(vAmbient, XMVectorSet(0.5f, 0.5f, 0.5f, 1.f), 0.2f));
        //XMStoreFloat4(&NewLightDesc.vSpecular, XMVectorLerp(vSpecular, XMVectorSet(1.f, 1.f, 1.f, 1.f), 0.2f));
      /*
        pDirectionLight->Set_LightDesc(NewLightDesc);*/



    };


    if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(ENUM_TO_UINT(LEVEL_ID::STATIC), PROTO_OBJ_NAME(L"EventTrigger"), ENUM_TO_UINT(LEVEL_ID::TOWN), strLayerTag, &DefaultEventDesc)))
        return E_FAIL;

#pragma endregion

#pragma region 숲카메라
    //숲속 카메라트리거
    CEventTrigger::EventTriggerDesc EventDesc;
    EventDesc.vCenter = _float3(0.f, 0.f, 0.f);
    EventDesc.vExtents = _float3(0.8f, 0.5f, 8.f);
    EventDesc.ObjTag = L"Forest_Trigger";
    EventDesc.m_iLevelID = m_iLevelID;
    
    CTransform::TRANSFORM_DESC EventTransform;
    EventTransform.vLocalPosition = _float4(2.198f, 12.79f, 52.188f, 1.f);

    EventDesc.TransformDesc = &EventTransform;
    EventDesc.EnterFunc = [this]()
    {
        GameEvent Event;
        Event.Name = "Enter_Forest";

        CPlayer* pPlayer = m_pGameManager->Get_MainPlayer();
        m_eArea = Area::FOREST;

        m_pGameInstance->Emit(Event);
     


    };
    EventDesc.StayFunc = [this]()
    {
        m_Chapter = L"Forest";
        CCamera_Base* pCamBase = m_pGameInstance->Find_Camera(CAMERA_TYPE::TARGET);
        CheckNull(pCamBase);

        CMainCamera* pMaincam = dynamic_cast<CMainCamera*>(pCamBase);
        CheckNull(pMaincam);

        _float3 vTargetOffset = _float3(0.f, 7.5f, -4.f);

        _float3 vCurFloatRot = pMaincam->Get_Transform()->Get_Rotation_ByEular();
        _float3 vCurFloatOffset = pMaincam->Get_Offset();

        _vector vCurRot = XMLoadFloat3(&vCurFloatRot);
        _vector vCurOffSet = XMLoadFloat3(&vCurFloatOffset);

        _vector fRotation = XMVectorLerp( vCurRot, XMVectorSet(65.f, 0.f, 0.f, 1.f), 0.02f);
        _vector vOffset = XMVectorLerp(vCurOffSet,XMLoadFloat3(&vTargetOffset), 0.02f);

        _float4 vResult;
        _float3 fOffSet;
        XMStoreFloat4(&vResult, fRotation);
        XMStoreFloat3(&fOffSet, vOffset);

        pMaincam->Set_LocalRoation(vResult);
        pCamBase->Set_Offset(fOffSet);

        pCamBase->Set_TargetOffset(vTargetOffset);


        pCamBase->Set_TargetRotation(_float3(65.f, 0.f, 0.f));


        //directionlight조절
        CLight* pDirectionLight = m_pGameInstance->Get_DirectionLight(m_iLevelID);
        CheckNull(pDirectionLight);

        //빛 변경값..나중에 설정하기!!어두워지기!
     //   LIGHT_DESC NewLightDesc = *pDirectionLight->Get_LightDesc();
     //   _vector vDiffuseColor = XMLoadFloat4(&NewLightDesc.vDiffuse);
     //   _vector vAmbient = XMLoadFloat4(&NewLightDesc.vAmbient);
     //   _vector vSpecular = XMLoadFloat4(&NewLightDesc.vSpecular);

     //   
     //   XMStoreFloat4(&NewLightDesc.vDiffuse, XMVectorLerp(vDiffuseColor, XMVectorSet(0.f, 0.1f, 0.5f, 1.f), 0.2f));
     ////   XMStoreFloat4(&NewLightDesc.vAmbient, XMVectorLerp(vAmbient, XMVectorSet(0.2f, 0.2f, 0.2f, 1.f), 0.2f));
     //   XMStoreFloat4(&NewLightDesc.vSpecular, XMVectorLerp(vSpecular, XMVectorSet(0.5f, 0.5f, 0.5f, 1.f), 0.2f));

     //   pDirectionLight->Set_LightDesc(NewLightDesc);


    };


    if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(ENUM_TO_UINT(LEVEL_ID::STATIC), PROTO_OBJ_NAME(L"EventTrigger"), ENUM_TO_UINT(LEVEL_ID::TOWN), strLayerTag, &EventDesc)))
        return E_FAIL;
    
#pragma endregion
   


#pragma region 타린트리거
    CEventTrigger::EventTriggerDesc NewChapter_EventDesc;
    NewChapter_EventDesc.vCenter = _float3(0.f, 0.f, 0.f);
    NewChapter_EventDesc.vExtents = _float3(1.f, 1.f, 1.f);
    NewChapter_EventDesc.ObjTag = L"Richard_Chapter_Trigger";    //리처드 챕터 트리거( 리처드 집이동 이벤트)
    NewChapter_EventDesc.m_iLevelID = m_iLevelID;

    CTransform::TRANSFORM_DESC NewChapter_EventTransform;
    NewChapter_EventTransform.vLocalPosition = _float4(38.f, 10.5f, 34.5f, 1.f);

    NewChapter_EventDesc.TransformDesc = &NewChapter_EventTransform;
   
    //!나중에 흐름연결필요!
    NewChapter_EventDesc.bActive_At_Begin = false;

    NewChapter_EventDesc.EnterFunc = [this]()
    {
        CLayer* pLayer = m_pGameInstance->Find_Layer(m_iLevelID, L"NPC_Layer");
        if (pLayer)
        {
            CGameObject* pNPC = pLayer->Find_GameObject(L"NPC_Tarin");
            if (pNPC)
            {
                CNPC_Tarin* pNpc_Tarin = dynamic_cast<CNPC_Tarin*>(pNPC);
                CheckNull(pNpc_Tarin);
                pNpc_Tarin->Set_Active(true);
                pNpc_Tarin->Set_StartEvent(true, CNPC_Tarin::State::WALK);

                CPlayer* pPlayer=CGameManager::GetInstance()->Get_MainPlayer();
                CheckNull(pPlayer);
                
                pPlayer->On_RichardChapterEvent(pNpc_Tarin);

              
                
            }
        }

        
    };

    if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(ENUM_TO_UINT(LEVEL_ID::STATIC), PROTO_OBJ_NAME(L"EventTrigger"), ENUM_TO_UINT(LEVEL_ID::TOWN), strLayerTag, &NewChapter_EventDesc)))
        return E_FAIL;


#pragma endregion

    return S_OK;
}

HRESULT CLevel_Town::Ready_EventListners()
{
    m_pGameInstance->RegisterListners("FadeScreen_Before_WitchRoom", [this](const GameEvent& evt)
        {
            pFadeScreen->PlayFadeIn();


        });

    m_pGameInstance->RegisterListners("Go_WitchRoom", [this](const GameEvent& evt)
        {
            CLayer* pNPCLayer = CGameInstance::GetInstance()->Find_Layer(ENUM_TO_UINT(LEVEL_ID::TOWN), L"NPC_Layer");
            CGameObject* pFairy = nullptr;
            if (pNPCLayer)
            {
                pFairy = pNPCLayer->Find_GameObject(L"NPC_Fairy");
                if (pFairy)
                    CheckFalse(pFairy->Is_Active());
            }

            m_pGameInstance->Clear_SceneColliders(ENUM_TO_UINT(LEVEL_ID::ROOM));
            //요정비활성화
            
            //마녀집 가기전 다시스폰하기 위한 전위치, 집앞
            
            _vector vPos = m_pGameInstance->Get_CellPos_By_MainCells(0);
            m_pGameManager->Set_LastPosition(vPos);

            pFadeScreen->Set_FadeInEndFunc([this]()
                {
                    LevelArgs args;
                    args.iNextLevelID = ENUM_TO_UINT(LEVEL_ID::ROOM);
                    args.changeType = LEVELCHANGETYPE::PUSH;
                    args.loadingChangeType = LEVELCHANGETYPE::PUSH;
                    args.m_iLevelID = ENUM_TO_UINT(LEVEL_ID::LOADING);

                    CRoom_Manager::GetInstance()->Request_Room("MagicPowerHouse");

                    if (FAILED(m_pGameInstance->Level_Changer(
                        ENUM_TO_UINT(LEVEL_ID::LOADING),
                        args)))
                        return;

                });

            pFadeScreen->PlayFadeIn();
            
            if(pFairy)
                pFairy->Set_Active(false);
           


        });

    /*보스 디버그전용*/
    CPlayer* pPlayer = m_pGameManager->Get_MainPlayer();

    m_EnterFirstEvent.Payload.Ptrs["Player"] = pPlayer;
    m_EnterFirstEvent.Payload.Floats["OffSet_X"] = 0.f;
    m_EnterFirstEvent.Payload.Floats["OffSet_Y"] = 10.5f;
    m_EnterFirstEvent.Payload.Floats["OffSet_Z"] = -7.5f;

    m_EnterFirstEvent.Payload.Floats["Rot_X"] = 76.f;
    m_EnterFirstEvent.Payload.Floats["Rot_Y"] = 0.f;
    m_EnterFirstEvent.Payload.Floats["Rot_Z"] = 0.f;

    m_EnterFirstEvent.Payload.Ints["bLock"] = true;


    m_EnterFirstEvent.Name = "Enter_DungeonRoom";

    m_pGameInstance->RegisterListners("Go_Boss", [this](const GameEvent& evt)
        {

            LevelArgs args;
            args.iNextLevelID = ENUM_TO_UINT(LEVEL_ID::BOSS);
            args.changeType = LEVELCHANGETYPE::PUSH;
            args.loadingChangeType = LEVELCHANGETYPE::PUSH;
            args.m_iLevelID = ENUM_TO_UINT(LEVEL_ID::LOADING);


            if (FAILED(m_pGameInstance->Level_Changer(
                ENUM_TO_UINT(LEVEL_ID::LOADING),
                args)))
                return;
        });


    //Richard_Chapter_Trigger

    m_pGameInstance->RegisterListners("Mom_Finish", [this](const GameEvent& evt)
        {

            CGameObject* pFindTrigger = m_pGameInstance->Find_GameObject(ENUM_TO_UINT(LEVEL_ID::TOWN), L"Trigger_Layer", L"Richard_Chapter_Trigger");
            if (pFindTrigger)
            {
                pFindTrigger->Set_Active(true);

            }

           
        });
    return S_OK;
}



void CLevel_Town::OnEnter()
{

    

    __super::OnEnter();

    m_pGameInstance->Set_MainCamera(CAMERA_TYPE::TARGET);

    CPlayer* pPlayer=m_pGameManager->Get_MainPlayer();
    CheckNull(pPlayer);

    pPlayer->Show_Weapons();
    vector<CCell*>* m_Cells = m_pGameInstance->Get_MainCells();
    if (m_Cells)
    {
         CCell* pSpawnCell = (*m_Cells)[0];//[1033];
        _vector vSpawnPoint = pSpawnCell->Get_CenterPos();

      
        pPlayer->Get_Transform()->Set_State(STATE::POSITION, vSpawnPoint);
        pPlayer->Change_MainNavMesh();

        m_pGameManager->Set_DefaultPosition(vSpawnPoint);
        m_pGameManager->Set_LastPosition(vSpawnPoint);

    }

    //미니맵 타겟 = 플레이어
    CCamera_Base* pMinimapCamera = m_pGameInstance->Find_Camera(CAMERA_TYPE::MINIMAP);
    if (pMinimapCamera)
    {
        pMinimapCamera->Set_Target(pPlayer);
    }




    CGameObject* pMainCamera = m_pGameInstance->Find_Camera(CAMERA_TYPE::TARGET);
    if (pMainCamera)
    {
        CMainCamera* ppMainCamera = dynamic_cast<CMainCamera*>(pMainCamera);
        CheckNull(ppMainCamera);
        ppMainCamera->Set_Target(pPlayer, true);
    }


#ifdef _DEBUG
    CStateDebugWindow* pWindow = dynamic_cast<CStateDebugWindow*>(CImGui_Manager::GetInstance()->Find_Window("StateDebugWindow"));
    if (pWindow)
    {
        CGameObject* pObj = m_pGameInstance->Find_GameObject(ENUM_TO_UINT(LEVEL_ID::STATIC), L"Player_Layer", L"Player");
        
        if (pObj)
        {
            CPlayer* pPlayer = dynamic_cast<CPlayer*>(pObj);
            pWindow->Set_Player(pPlayer);
   

        }
            

    }
#endif

    //////현재씬의 itneraction 등록
    CInteraction_Manager::GetInstance()->Change_Scene(ENUM_TO_UINT(LEVEL_ID::TOWN));

    
    CheckNull(pFadeScreen);
    pFadeScreen->PlayFadeOut();
    CGameInstance::GetInstance()->Set_EnalbeUpdateRender(true);

    wstring strKey = L"Town";
    CGameInstance::GetInstance()->BroadCastEvent(L"UpdateLevelUI", &strKey);
    m_pGameInstance->Invoke(6.f, 0.f, false, false, []()
        {
            CGameInstance::GetInstance()->BroadCastEvent(L"OnLevelUIHide", nullptr);


        },pPlayer);





}

void CLevel_Town::OnResume(_uint iPreLevel)
{
  
 
 
    //씬이 다시시작행슬때 메인 상호작용오브ㅈ게트들 설정
     //////현재씬의 itneraction 등록
    CInteraction_Manager::GetInstance()->Change_Scene(ENUM_TO_UINT(LEVEL_ID::TOWN));



    


    LEVEL_ID PrevID = (LEVEL_ID)iPreLevel;
    switch (PrevID)
    {

    case Client::LEVEL_ID::ROOM:
    {
        //카메라돌려놓기이벤트 실행
        CPlayer* pPlayer = m_pGameManager->Get_MainPlayer();
        pPlayer->Show_Weapons();

        GameEvent Event;
        EventPayload payload;
        Event.Payload = payload;

        Event.Payload.Ptrs["Player"] = pPlayer;
        Event.Name = "Complete_Init_Camera";
        m_pGameInstance->Emit(Event);

        m_pGameInstance->Set_EnalbeUpdateRender(true);
        m_pGameInstance->Set_EnableUpdate(true);
        CheckNull(pFadeScreen);

        m_pGameInstance->Invoke(1.f, 0.f, false, false, [m_pFadeScreen=pFadeScreen]()
            {
                m_pFadeScreen->PlayFadeOut();

            },CGameManager::GetInstance()->Get_MainPlayer());

       
        //NavMesh돌려놓기
        m_pGameInstance->Set_MainCells(ENUM_TO_UINT(LEVEL_ID::TOWN));
        pPlayer->Get_Transform()->Set_State(STATE::POSITION, m_pGameManager->Get_LastPosition_By_Vector());
        pPlayer->Change_MainNavMesh();
        wstring strKey = L"Town";
        CGameInstance::GetInstance()->BroadCastEvent(L"UpdateLevelUI", &strKey);
        m_pGameInstance->Invoke(6.f, 0.f, false, false, []()
            {
                CGameInstance::GetInstance()->BroadCastEvent(L"OnLevelUIHide", nullptr);


            }, pPlayer);

    }
    
    break;


    case Client::LEVEL_ID::UI:
        break;
         
    default:
        break;
    }


    __super::OnResume(iPreLevel);
    m_pGameInstance->Set_IsLoading(false);

    
}
 

void CLevel_Town::OnPause(_uint iNextLevel)
{
    /*2회 호출막기..*/
    if (Get_State() == LEVELSTATE::HIDDEN || Get_State() == LEVELSTATE::PAUSE)
        return;
    LEVEL_ID NextlevelID = (LEVEL_ID)iNextLevel;
    
    switch (NextlevelID)
    {
 
    case Client::LEVEL_ID::ROOM:
        m_pGameInstance->Set_EnalbeUpdateRender(false);
        m_pGameInstance->Set_EnableUpdate(false);
        CheckNull(pFadeScreen);
        pFadeScreen->PlayFadeIn();
        break;

    case Client::LEVEL_ID::UI:
        break;
 
    default:
        break;
    }

    
    int A=0;
}

void CLevel_Town::OnExit()
{
    CInteraction_Manager::GetInstance()->Clear();                                                          
    CGameInstance::GetInstance()->Set_EnalbeUpdateRender(false);
}

void CLevel_Town::Change_Area()
{
    wstring strKey = L"";

    if (m_ePreArea != m_eArea)
    {
        switch (m_eArea)
        {
        case Client::CLevel_Town::TOWN:
            strKey = L"Town";
            break;
        case Client::CLevel_Town::FOREST:
            strKey = L"Forest";
            break;
  
        default:
                break;
        }

        CGameInstance::GetInstance()->BroadCastEvent(L"UpdateLevelUI", &strKey);
        m_pGameInstance->Invoke(6.f, 0.f, false, false, []()
            {
                CGameInstance::GetInstance()->BroadCastEvent(L"OnLevelUIHide", nullptr);


            }, CGameManager::GetInstance()->Get_MainPlayer());

        m_ePreArea = m_eArea;
    }
}



void CLevel_Town::Teleport_RichardHouse()
{
    CPlayer* pPlayer = CGameManager::GetInstance()->Get_MainPlayer();
    CheckNull(pPlayer);


    //왠지모르지만 room트리거들이 밟히는거같아서 콜리전비홀성화
    CLayer* pTriggerLayer = m_pGameInstance->Find_Layer(m_iLevelID, L"Trigger_Layer");
    for (auto& pObj : pTriggerLayer->Get_ObjList())
    {
        CTrigger_Box* pRoomTrigger = dynamic_cast<CTrigger_Box*>(pObj);
        if (pRoomTrigger)
        {
            m_pGameInstance->UnRegister_Collider(pRoomTrigger->Get_Collider(), m_iLevelID);

        }
            
    }

    /*텔레포트 - 리차드집앞*/
    _vector vTeleportPos = XMVectorSet(79.f, 13.f, 95.f, 1.f);
    pPlayer->Get_Transform()->Set_State(STATE::POSITION, vTeleportPos);
    pPlayer->Change_MainNavMesh();

    //텔레포트

    /*타린도 함께 텔레포트*/

    CNPC* Tarin = dynamic_cast<CNPC*>(m_pGameInstance->Find_GameObject(m_iLevelID, L"NPC_Layer", L"NPC_Tarin"));
    if (Tarin)
    {

        _vector vTarinTelepos= XMVectorSet(78.f, 18.f, 97.f, 1.f);
        Tarin->Get_Transform()->Set_State(STATE::POSITION, vTarinTelepos);
        CNPC_Tarin* pTarin = dynamic_cast<CNPC_Tarin*>(Tarin);
        CheckNull(pTarin);

        pTarin->Get_Navigation()->Set_CurrentIdx(pTarin->Get_Transform()->Get_State(STATE::POSITION) );
        pTarin->Get_Transform()->Set_State(STATE::POSITION,
            pTarin->Get_Navigation()->SetUp_OnNavigation((pTarin->Get_Transform()->Get_State(STATE::POSITION))));

      pTarin->Start_SecondChapter();
    }
                                            
    m_pGameInstance->Invoke(2.f, false, false, false, []()
        {

            UIGroup* pGroup = CGameInstance::GetInstance()->Get_UIGroup(L"FadeScreenGroup");
            CFadeScreen* pFadeScreen = dynamic_cast<CFadeScreen*>(pGroup->Find(L"FadeScreen"));
            pFadeScreen->PlayFadeOut();


        },CGameManager::GetInstance()->Get_MainPlayer());

    
    m_pGameInstance->Invoke(3.f, false, false, false, []()
        {
               CNPC* Tarin = dynamic_cast<CNPC*>(CGameInstance::GetInstance()->Find_GameObject(ENUM_TO_UINT(LEVEL_ID::TOWN), L"NPC_Layer", L"NPC_Tarin"));
               CNPC_Tarin* pTarin = dynamic_cast<CNPC_Tarin*>(Tarin);
               CheckNull(pTarin);

               CInteraction_Manager::GetInstance()->Set_CurrentTarget(pTarin);
      
               /*도착했을때 다시켜주기.*/
               CLayer* pTriggerLayer = CGameInstance::GetInstance()->Find_Layer(ENUM_TO_UINT(LEVEL_ID::TOWN), L"Trigger_Layer");
               for (auto& pObj : pTriggerLayer->Get_ObjList())
               {
                   CTrigger_Box* pRoomTrigger = dynamic_cast<CTrigger_Box*>(pObj);
                   if (pRoomTrigger)
                   {
                       CGameInstance::GetInstance()->Register_Collider(pRoomTrigger->Get_Collider(), ENUM_TO_UINT(LEVEL_ID::TOWN));

                   }

               }
        }, CGameManager::GetInstance()->Get_MainPlayer());

}

CLevel_Town* CLevel_Town::Create(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pDeviceContext, LevelArgs& args)
{
    CLevel_Town* pInstance = new CLevel_Town(_pDevice, _pDeviceContext);
    if (FAILED(pInstance->Initialize(args)))
    {
        MSG_BOX("Failed to Create : Level_Town");
        Safe_Release(pInstance);
    }


    return pInstance;
}

void CLevel_Town::Free()
{
    __super::Free();
 

}
