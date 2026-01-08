#include "CLevel_Boss.h"
#include "CGameManager.h"
#include "UIGroup.h"
#include "CGameInstance.h"
#include "CFadeScreen.h"

#include "CMapLoader.h"
#include "CPlayer.h"
#include "CRoom.h"

#include "CStaticBody.h"
#include "CCell.h"
#include "CCamera_Base.h"

#include "CWall.h"
#include "CInteraction_Manager.h"
#include "CBoxColliderComponent.h"
#include "CBounding_AABB.h"

#include "CClosedDoor.h"

#include "CMapLoader.h"
#include "CMonster.h"
#include "CMonster_Body.h"

#include "CM_Jacky.h"
#include "CInteraction_JackyBall.h"
#include "CEventTrigger.h"
#include "CRoom_Manager.h"

#include "CNPC.h"
#include "CNPC_KidRed.h"




USING(Client)
CLevel_Boss::CLevel_Boss(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pDeviceContext)
    :CLevel(_pDevice, _pDeviceContext)
{
}

HRESULT CLevel_Boss::Initialize(LevelArgs& args)
{
    __super::Initialize(args);

    m_pGameManager = CGameManager::GetInstance();

    if (FAILED(Ready_Lights()))
        return E_FAIL;

    if (FAILED(Ready_Layer_Enviroment(L"Enviroment_Layer")))
        return E_FAIL;

    if (FAILED(Ready_Layer_Monster(L"Monster_Layer")))
        return E_FAIL;

    if (FAILED(Ready_Layer_Interaction(L"Interaction_Layer")))
        return E_FAIL;

    if (FAILED(Ready_Layer_Trigger(L"Trigger_Layer")))
        return E_FAIL;

    if (FAILED(Ready_Layer_NPC(L"NPC_Layer")))
        return E_FAIL;

    if (FAILED(Ready_Events()))
        return E_FAIL;

    UIGroup* pGroup = m_pGameInstance->Get_UIGroup(L"FadeScreenGroup");
    pFadeScreen = dynamic_cast<CFadeScreen*>(pGroup->Find(L"FadeScreen"));


    return S_OK;
}

void CLevel_Boss::Update_Priority(_float fTimeDelta)
{
    __super::Update_Priority(fTimeDelta);
}

void CLevel_Boss::Update(const _float fTimeDelta)
{
    __super::Update(fTimeDelta);


}

void CLevel_Boss::Update_Late(_float fTimeDelta)
{
    __super::Update_Late(fTimeDelta);

}

void CLevel_Boss::Render()
{
    wchar_t szTitle[256];
    CheckNull(pJackyBall);
    swprintf_s(szTitle, L"Boss 씬입니다. FPS : %.1f , %s", m_pGameInstance->Get_FPS(L"Timer_60"), pJackyBall->Print_Owner().c_str());



    SetWindowText(g_hWnd, szTitle);
}

HRESULT CLevel_Boss::Ready_Lights()
{
    LIGHT_DESC      LightDesc{};
    LightDesc.eType = LIGHT::DIRECTIONAL;
    LightDesc.vDirection = _float4(1.f, -1.f, 1.f, 0.f);
    LightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
    LightDesc.vAmbient = _float4(0.5f, 0.5f, 0.5f, 1.f);
    LightDesc.vSpecular = _float4(0.5f, 0.5f, 0.5f, 1.f);

    if (FAILED(m_pGameInstance->Add_Light(m_iLevelID, LightDesc)))
        return E_FAIL;


    return S_OK;
}

HRESULT CLevel_Boss::Ready_Layer_Enviroment(const _wstring& strLayerTag)
{
    CRoom::MODELOBJECT_DESC     RoomDesc;
    RoomDesc.ObjTag = L"BossStage";

    CStaticBody::BODY_DESC BodyDesc;
    BodyDesc.eRenderGroup = ENUM_TO_UINT(RENDERGROUP::NONALPHA);
    BodyDesc.modelName = L"BossStage";
    RoomDesc.BodyDesc = &BodyDesc;

    CTransform::TRANSFORM_DESC TransDesc;
    TransDesc.vLocalPosition = _float4(12.692f,0.f,12.789f,1.f);
    RoomDesc.TransformDesc = &TransDesc;

    CBase* pBaseRoom = m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_TO_UINT(LEVEL_ID::STATIC), PROTO_OBJ_NAME(L"Room"), &RoomDesc);
    CGameObject* ppBaseRoom = dynamic_cast<CGameObject*>(pBaseRoom);

    if (ppBaseRoom)
    {
        if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(ENUM_TO_UINT(LEVEL_ID::BOSS), strLayerTag, ppBaseRoom)))
            return E_FAIL;



    }
    if (FAILED(CMapLoader::Load_Boss()))
        return E_FAIL;

#pragma region closedDoor
    _float4 vPlayerPos[] = {
        _float4(12.520f,1.2f,19.f,1.f),
        _float4(12.699f,1.2f,6.673f,1.f)

    };

    _float4 vRotation[] = {
       _float4(0.f,0.f,0.f,1.f),
       _float4(0.f,180.f,0.f,1.f)

    };

    for (int i = 0; i < 2; ++i)
    {
        CClosedDoor::MODELOBJECT_DESC Desc;
        Desc.m_iLevelID = m_iLevelID;
        Desc.ObjTag = L"ClosedDoor"+to_wstring(i);
        
        CTransform::TRANSFORM_DESC TransDesc;
        TransDesc.vLocalPosition = vPlayerPos[i];
        TransDesc.vLocalRotation = vRotation[i];

        Desc.TransformDesc = &TransDesc;

        CGameObject* pObj = dynamic_cast<CGameObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_TO_UINT(LEVEL_ID::STATIC),  PROTO_OBJ_NAME(L"CloseDoor"), &Desc));

        if (pObj)
        {
            if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(ENUM_TO_UINT(LEVEL_ID::BOSS),strLayerTag, pObj)))
                return E_FAIL;

        }
        
        
    }

#pragma endregion
    m_pGameInstance->Set_MainCells(ENUM_TO_UINT(LEVEL_ID::BOSS));


#pragma region 벽 충돌체 생성
    _float4 vPos[] = {
        _float4(12.444f,-0.361f,    7.917f,1.f),
        _float4(12.611f,-0.361f,    18.303f,1.f),
        _float4(6.364f,-1.1f,   12.938f,1.f),
        _float4(18.734f,-1.0f,  12.738f,1.f),
        _float4(12.687f,-0.7f,   12.685f,1.f)
    };

    _float3 vExtents[] = {
        _float3(6.8f,3.4f,0.2f),
        _float3(6.8f,3.4f,0.2f),
        _float3(0.2f,4.0f,5.8f),
        _float3(0.2f,4.0f,5.8f),
        _float3(6.f,0.1f,4.5f)

    };

    for (int i = 0; i < 5; ++i)
    {
        CWall::WALL_DESC WallDesc;

        WallDesc.m_iLevelID = m_iLevelID;
        WallDesc.ObjTag = L"Wall" + to_wstring(i);
        WallDesc.vExtents = vExtents[i];

        CTransform::TRANSFORM_DESC TransDesc;
        TransDesc.vLocalPosition = vPos[i];
        
        WallDesc.TransformDesc = &TransDesc;

        CGameObject* pObj = dynamic_cast<CGameObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_TO_UINT(LEVEL_ID::STATIC), PROTO_OBJ_NAME(L"Wall"), &WallDesc));

        if (pObj)
        {
            if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(ENUM_TO_UINT(LEVEL_ID::BOSS), strLayerTag, pObj)))
                return E_FAIL;

            m_pWalls.push_back(dynamic_cast<CWall*>(pObj));

        }

    }
#pragma endregion

    return S_OK;
}

HRESULT CLevel_Boss::Ready_Layer_Monster(const _wstring& strLayerTag)
{
    vector<CCell*>* m_Cells = m_pGameInstance->Get_MainCells();
    CheckTrueResult(m_Cells->empty(), E_FAIL);

    _vector vPos = (*m_Cells)[29]->Get_CenterPos();

    CMonster::MonsterDesc JackyDesc;

    CMonster_Body::MONSTER_BODY_DESC JackybodyDesc;
    JackybodyDesc.eRenderGroup = ENUM_TO_UINT(RENDERGROUP::NONALPHA);
    JackybodyDesc.modelName = L"Jacky";
    JackybodyDesc.m_iLevelID = m_iLevelID;
    JackyDesc.BodyDesc = &JackybodyDesc;
    JackyDesc.ObjTag =L"Jacky";

    JackyDesc.iAttack = 10;
    JackyDesc.MaxHp = 1;//5;//테스트
    JackyDesc.fActionRange = 3.f;
    JackyDesc.m_iLevelID = m_iLevelID;
    JackyDesc.RoamRadius = 0.f;

    CTransform::TRANSFORM_DESC JackyTransDesc;
    JackyTransDesc.vLocalRotation = { 0.f,180.f,0.f,1.f };
    XMStoreFloat4(&JackyTransDesc.vLocalPosition, vPos);
    JackyTransDesc.fSpeedPerSec =2.5f;
    JackyDesc.TransformDesc = &JackyTransDesc;


    if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(ENUM_TO_UINT(LEVEL_ID::STATIC),
        PROTO_OBJ_NAME(L"CM_Jacky"),
        ENUM_TO_UINT(LEVEL_ID::BOSS),
        strLayerTag, &JackyDesc)))
        return E_FAIL;



    return S_OK;
}

HRESULT CLevel_Boss::Ready_Layer_Interaction(const _wstring& strLayerTag)
{

    if (FAILED(m_pGameInstance->Make_New_Layer(ENUM_TO_UINT(LEVEL_ID::BOSS), strLayerTag)))
        return E_FAIL;

    CLayer* pInteractionLayer = m_pGameInstance->Find_Layer(ENUM_TO_UINT(LEVEL_ID::BOSS), strLayerTag);
    CMapLoader::Make_Object_By_LoadData("Level_Boss", pInteractionLayer);


    return S_OK;
}

HRESULT CLevel_Boss::Ready_Layer_Trigger(const _wstring& strLayerTag)
{
#pragma region 연출
    CEventTrigger::EventTriggerDesc Enter_Boss_Desc;
    Enter_Boss_Desc.vCenter = _float3(0.f, 0.f, 0.f);
    Enter_Boss_Desc.vExtents = _float3(1.6f, 0.3f, 0.6f);
    Enter_Boss_Desc.ObjTag = L"Enter_BossStage";
    Enter_Boss_Desc.m_iLevelID = m_iLevelID;

    CTransform::TRANSFORM_DESC EventTransform;
    EventTransform.vLocalPosition = _float4(12.861f, 0.2f, 16.065f, 1.f);

    Enter_Boss_Desc.TransformDesc = &EventTransform;
    Enter_Boss_Desc.EnterFunc = [this]()
    {
        //여러이벤트 실행
        GameEvent Event;
        Event.Name = "CloseDoor";
        m_pGameInstance->Emit(Event);

        Event.Name = "Start_Boss";
        m_pGameInstance->Emit(Event);



    };

    if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(ENUM_TO_UINT(LEVEL_ID::STATIC), PROTO_OBJ_NAME(L"EventTrigger"), ENUM_TO_UINT(LEVEL_ID::BOSS), strLayerTag, &Enter_Boss_Desc)))
        return E_FAIL;

#pragma endregion
    return S_OK;
}

HRESULT CLevel_Boss::Ready_Layer_NPC(const _wstring& strLayerTag)
{
    CNPC::NPC_DESC pNPCDesc;

    CTransform::TRANSFORM_DESC pNPCTransDesc;

    pNPCTransDesc.fSpeedPerSec = 0.7f;
    pNPCTransDesc.vLocalPosition = _float4(12.699f, 0.06f, 6.673f, 1.f);
    pNPCTransDesc.vLocalScale = _float4(0.8f, 0.8f, 0.8f, 1.f);

    pNPCDesc.ObjTag = L"NPC_Kid_Red";
    pNPCDesc.pTarget = nullptr;
    pNPCDesc.ModelName = L"Kid_Red";

    pNPCDesc.m_iLevelID = m_iLevelID;
    pNPCDesc.TalkRange = 4.f;
    pNPCDesc.bUseNavMesh = false;


    _uint Expression_Eye[ENUM_TO_UINT(CNPC::EXPRESSION::END)] = {};
    _uint Expression_Mouth[ENUM_TO_UINT(CNPC::EXPRESSION::END)] = {};
    _uint Expression_Mouth_Open[ENUM_TO_UINT(CNPC::EXPRESSION::END)] = {};

    CRoom_Manager::GetInstance()->Load_NPC_ExpressionData(L"Kid_Red", Expression_Eye, Expression_Mouth, Expression_Mouth_Open);

    memcpy(&pNPCDesc.iExpressionIdxEye, Expression_Eye, sizeof(_uint) * CNPC::EXPRESSION::END);
    memcpy(&pNPCDesc.iExpressionIdx_Mouth, Expression_Mouth, sizeof(_uint) * CNPC::EXPRESSION::END);
    memcpy(&pNPCDesc.iOpenIdx_Mouth, Expression_Mouth_Open, sizeof(_uint) * CNPC::EXPRESSION::END);



    pNPCDesc.TransformDesc = &pNPCTransDesc;

    CNPC* pNpc_KidRed = CNPC_KidRed::Create(m_pDevice, m_pContext, &pNPCDesc);
    if (pNpc_KidRed)
    {
        if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(ENUM_TO_UINT(LEVEL_ID::STATIC), strLayerTag, pNpc_KidRed)))
            return E_FAIL;

        pNpc_KidRed->Set_Active(false);
    }

    return S_OK;
}

HRESULT CLevel_Boss::Ready_Events()
{
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

    //엔딩으로갈땐 하얀색페이드
    m_pGameInstance->RegisterListners("Go_to_Ending", [this](GameEvent Evnet)
        {
            pFadeScreen->Set_Texture(L"Default");

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
        });

    return S_OK;
}

void CLevel_Boss::OnEnter()
{
    __super::OnEnter();
    m_pGameInstance->Set_MainCamera(CAMERA_TYPE::TARGET);
    CInteraction_Manager::GetInstance()->Change_Scene(ENUM_TO_UINT(LEVEL_ID::BOSS));

    CPlayer* pPlayer = m_pGameManager->Get_MainPlayer();
    CheckNull(pPlayer);

    pPlayer->Show_Weapons();
    vector<CCell*>* m_Cells = m_pGameInstance->Get_MainCells();
    if (m_Cells)
    {
        CCell* pSpawnCell = (*m_Cells)[17];
        _vector vSpawnPoint = pSpawnCell->Get_CenterPos();


        pPlayer->Get_Transform()->Set_State(STATE::POSITION, vSpawnPoint);
        pPlayer->Change_MainNavMesh();
    }


    CheckNull(pFadeScreen);
    pFadeScreen->PlayFadeOut();

    
    m_pGameInstance->Emit(m_EnterFirstEvent);

    CCamera_Base* pBase = m_pGameInstance->Find_Camera(CAMERA_TYPE::TARGET);
    if (pBase)
        pBase->Set_Lock(true);

    ////보스몬스터가 쫓아야할 ball 세팅해주기.
    CGameObject* pMonster = m_pGameInstance->Find_GameObject(m_iLevelID, L"Monster_Layer", L"Jacky");
    if (pMonster)
    {
        CGameObject* pInteraction = m_pGameInstance->Find_GameObject(m_iLevelID, L"Interaction_Layer", L"JackyIronBall0");
        if (pInteraction)
        {
            CM_Jacky* pJacky = dynamic_cast<CM_Jacky*>(pMonster);
            pJackyBall = dynamic_cast<CInteraction_JackyBall*>(pInteraction);

            if (pJacky && pJackyBall)
            {
                pJacky->Set_ChaseTargetObj(pJackyBall);
                pJacky->Set_DeadEvent([this]()
                    {
                        
                        GameEvent Event;
                        Event.Name = "OpenDoor";

                        m_pGameInstance->Emit(Event);

                        for (auto& pObj : m_pWalls)
                            pObj->Set_Active(false);
                        m_pGameInstance->Invoke(1.f, 0.f, false, false, [pGameInstance = m_pGameInstance]()
                            {

                                CGameObject* pNpc = pGameInstance->Find_GameObject(ENUM_TO_UINT(LEVEL_ID::STATIC), L"NPC_Layer", L"NPC_Kid_Red");
                                pNpc->Set_Active(true);
                                if (pNpc)
                                {
                                    CNPC_KidRed* ppNpc = dynamic_cast<CNPC_KidRed*>(pNpc);
                                    if (ppNpc)
                                    {
                                        CGameManager::GetInstance()->Set_EndingStep(EndingStep::START_DIALOGUE);
                                        ppNpc->Set_StartEvent(true, CNPC_KidRed::State::WALK);
                                        CGameManager::GetInstance()->Get_MainPlayer()->Enter_EndCutScene();

                                        
                                    }
                                }
                            }, CGameManager::GetInstance()->Get_MainPlayer());

                      

                        
                    });
            }
        }
    }
}

void CLevel_Boss::OnResume(_uint iPreLevel)
{
}

void CLevel_Boss::OnPause(_uint iNextLeve)
{
}

void CLevel_Boss::OnExit()
{
}

void CLevel_Boss::Close_Door()
{
    
}

CLevel_Boss* CLevel_Boss::Create(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pDeviceContext, LevelArgs& args)
{
    CLevel_Boss* pInstance = new CLevel_Boss(_pDevice, _pDeviceContext);
    if (FAILED(pInstance->Initialize(args)))
    {
        MSG_BOX("Failed to Create : CLevel_Boss");
        Safe_Release(pInstance);
    }


    return pInstance;
}

void CLevel_Boss::Free()
{
    __super::Free();
}
