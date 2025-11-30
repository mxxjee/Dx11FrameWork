#include "CLevel_Town.h"
#include "CLoader.h"
#include "CGameInstance.h"
#include "CBackGround.h"
#include "CMainCamera.h"
#include "CPerspectiveCameraComponent.h"
#include "CTerrain.h"
#include "CMonster.h"
#include "CBody.h"
#include "CInput_Manager.h"
#include "CLight.h"

#include "CStateDebugWindow.h"
#include "CImGui_Manager.h"
#include "CPlayer.h"

#include "MathUtils.h"
#include "CNPC_Richard.h"
#include "CIInteractable.h"

#include "CInteraction_Manager.h"



USING(Client)

CLevel_Town::CLevel_Town(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pDeviceContext)
    :CLevel(_pDevice,_pDeviceContext)
{
}

HRESULT CLevel_Town::Initialize(LevelArgs& args)
{
    __super::Initialize(args);

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

    return S_OK;
}

void CLevel_Town::Update_Priority(_float fTimeDelta)
{
    __super::Update_Priority(fTimeDelta);
    if (CInput_Manager::GetInstance()->IsKeyPressed(KeyCode::P))
    {
        LevelArgs args;
        args.iNextLevelID = ENUM_TO_UINT(LEVEL_ID::UI);
        args.changeType = LEVELCHANGETYPE::PUSH;
        //args.loadingChangeType = LEVELCHANGETYPE::PUSH;
        args.m_iLevelID = ENUM_TO_UINT(LEVEL_ID::UI);


        if (FAILED(m_pGameInstance->Level_Changer(
            ENUM_TO_UINT(LEVEL_ID::UI),
            args)))
            return;
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

    return;

}

void CLevel_Town::Update_Late(_float fTimeDelta)
{
    __super::Update_Late(fTimeDelta);
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

    /*카메라 변경 테스트*/
   if (CInput_Manager::GetInstance()->IsKeyPressed(KeyCode::C))
   {
       if (iTargetIdx == 1)
           iTargetIdx = 0;
       else
           ++iTargetIdx;

       iTargetIdx = MathUtils::Clamp(iTargetIdx, 0, 1);

       switch (iTargetIdx)
       {
       case 0:
           m_pGameInstance->Set_MainCamera(CAMERA_TYPE::TARGET);
           break;

       case 1:
           m_pGameInstance->Set_MainCamera(CAMERA_TYPE::FREE);

           break;
       }

   }

}

void CLevel_Town::Render()
{
    //UI렌더. (로딩바)
    SetWindowText(g_hWnd, L"게임플레이 씬입니다.");

}

HRESULT CLevel_Town::Ready_Lights()
{
    LIGHT_DESC      LightDesc{};
    LightDesc.eType = LIGHT::DIRECTIONAL;
    LightDesc.vDirection = _float4(1.f, 1.f, 1.f, 0.f);
    LightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
    LightDesc.vAmbient = _float4(1.f, 1.f, 1.f, 1.f);
    LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);

    if (FAILED(m_pGameInstance->Add_Light(m_iLevelID, LightDesc)))
        return E_FAIL;


    ////////////빨간 점조명 테스트//////////
    LIGHT_DESC       RedLight;
    RedLight.eType = LIGHT::POINT;

    RedLight.vDiffuse = _float4(10.f, 0.f, 0.f, 1.f);//빨간색
    RedLight.fRange = _float4(10.f, 0.f, 0.f, 1.f);
    RedLight.vPosition = _float4(0.f, 3.f, 0.f, 1.f);
    RedLight.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);
    RedLight.vAmbient = _float4(1.f, 1.f, 1.f, 1.f);


    if (FAILED(m_pGameInstance->Add_Light(m_iLevelID, RedLight)))
        return E_FAIL;


    return S_OK;
}


HRESULT CLevel_Town::Ready_Layer_Enviroment(const _wstring& strLayerTag)
{

    ////////////////Terrain불러오기//////////////////
    ///////////////////////////////////
#pragma region Loading Terrain
    vector<tagLoadTerrainData> LoadDatas=m_pGameInstance->Load_Terrains_Runtime("../../Resource/Data/Map/Final_Town.json");
    for (int i = 0; i < LoadDatas.size(); ++i)
    {
        tagLoadTerrainData TargetData = LoadDatas[i];

        CTerrain::TERRAINOBJECT_DESC desc;

        CBody::BODY_DESC bodyDesc;
        bodyDesc.eRenderGroup = ENUM_TO_UINT(RENDERGROUP::PRIORITY);
        bodyDesc.modelName =StringToWString(TargetData.ModelName);
        desc.BodyDesc = &bodyDesc;

        desc.ObjTag = L"Terrain"+bodyDesc.modelName;
        desc.vIndex = TargetData.vIndex;

        CTransform::TRANSFORM_DESC TransDesc = {};
        TransDesc.vLocalPosition = TargetData.vPosition;
        TransDesc.vLocalRotation = TargetData.vRotation;
        TransDesc.vLocalScale = TargetData.vScale;

        desc.TransformDesc = &TransDesc;

        CTerrain_Base* pTerrain = dynamic_cast<CTerrain_Base*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_TO_UINT(LEVEL_ID::STATIC),
            PROTO_OBJ_NAME(L"Terrain"), &desc));

        if (pTerrain)
            m_pGameInstance->Register_Terrain(pTerrain->Get_Tag(),pTerrain);
    }

  
#pragma endregion


    ///////////////////////Navigation 불러오기
    m_pGameInstance->Load_NavMesh(ENUM_TO_UINT(LEVEL_ID::TOWN), "../../Resource/Data/Map/Terrain8_Nav.dat");
#ifdef _DEBUG
    m_pGameInstance->Set_NavMeshShader(m_pGameInstance->Find_Shader(L"VtxPos"));
#endif
    m_pGameInstance->Set_MainCells(ENUM_TO_UINT(LEVEL_ID::TOWN));

    return S_OK;
}


HRESULT CLevel_Town::Ready_Layer_UI(const _wstring& strLayerTag)
{
    UIGroup     HeartGroup;
    HeartGroup.Key = L"HeartGroup";

    for (int i = 0; i < 5; ++i)
    {
        CUI::tagUIDesc        Desc = {};

        Desc.ObjTag = L"Hp_UI" + to_wstring(i);
        Desc.eRenderGroup = ENUM_TO_UINT(RENDERGROUP::UI);
        Desc.TextureKey = L"Hp";

        Desc.iIdx = i;

        Desc.fSizeX = 38.f;
        Desc.fSizeY = 38.f;
        Desc.fX = 50.f + (i * 45.f);
        Desc.fY = 50.f;

        CTransform::TRANSFORM_DESC TransDesc = {};
        TransDesc.fRotationPerSec = 10.f;
        TransDesc.fSpeedPerSec = 5.f;
        Desc.TransformDesc = &TransDesc;

        //AlphaAnim등록
        CUIComponent::UICOMP_DESC UIDesc = {};
        UIDesc._AnimInfo[ENUM_TO_UINT(UIAnimType::POSITION)].fStart = _float4(Desc.fX, Desc.fY, 1.f, 1.f);
        UIDesc._AnimInfo[ENUM_TO_UINT(UIAnimType::POSITION)].fTarget = _float4(Desc.fX - 10.f, Desc.fY, 1.f, 1.f);
        UIDesc._AnimInfo[ENUM_TO_UINT(UIAnimType::POSITION)].m_fSpeed = 0.1f;
        UIDesc._AnimInfo[ENUM_TO_UINT(UIAnimType::POSITION)].bLoop = true;

        UIDesc._AnimInfo[ENUM_TO_UINT(UIAnimType::ALPHA)].fStart = _float4(1.f, 0.f, 0.f, 0.f);
        UIDesc._AnimInfo[ENUM_TO_UINT(UIAnimType::ALPHA)].fTarget = _float4(0.f, 0.f, 0.f, 0.f);
        UIDesc._AnimInfo[ENUM_TO_UINT(UIAnimType::ALPHA)].m_fSpeed = 5.f;
        UIDesc._AnimInfo[ENUM_TO_UINT(UIAnimType::ALPHA)].bLoop = true;
        Desc.UICompDesc = &UIDesc;





        CBase* pObj = m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_TO_UINT(LEVEL_ID::STATIC), PROTO_OBJ_NAME(L"Panel"), &Desc);
        if (pObj)
        {
            CGameObject* pInstance = dynamic_cast<CGameObject*>(pObj);
            if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(ENUM_TO_UINT(LEVEL_ID::STATIC), strLayerTag, pInstance)))
                return E_FAIL;


            HeartGroup.Objects.push_back(pInstance);

        }

    }

    m_pGameInstance->Register_UIGroup(HeartGroup);
    m_pGameInstance->RegisterEvent(L"OnHeartDamaged", [this](void* pData)
        {
            int* iHp = reinterpret_cast<int*>(pData);
            UIGroup* pGroup = m_pGameInstance->Get_UIGroup(L"HeartGroup");
            if (pGroup)
            {
                for (auto& i : pGroup->Objects)
                {
                    CUI* pUI = dynamic_cast<CUI*>(i);
                    if (pUI)
                    {
                        if (pUI->Get_Idx() == *iHp)
                        {
                            pUI->Set_ActiveAnim(1, [pUI]()
                                {
                                    pUI->Get_UIComp()->PlayAnim(UIAnimType::ALPHA);
                                    pUI->Get_UIComp()->PlayAnim(UIAnimType::POSITION);
                                });


                            pUI->OnActivated(false);
                        }
                    }
                }
            }
        });


    ///////////////////Minimapquad생성
    CUI::tagUIDesc        MinimapDesc = {};
    MinimapDesc.ObjTag = L"MinimapQuad";
    MinimapDesc.passName = "Blur";
    MinimapDesc.eRenderGroup = ENUM_TO_UINT(RENDERGROUP::UI);
    MinimapDesc.fSizeX = 150;
    MinimapDesc.fSizeY = 150;
    MinimapDesc.fX = g_iWinSizeX - 100;
    MinimapDesc.fY = 100;

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


    return S_OK;

}

HRESULT CLevel_Town::Ready_Layer_Player(const _wstring& strLayerTag)
{

    if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(ENUM_TO_UINT(LEVEL_ID::STATIC),
        PROTO_OBJ_NAME(L"Player"),
        ENUM_TO_UINT(LEVEL_ID::STATIC),
        strLayerTag, nullptr)))
        return E_FAIL;

    return S_OK;

}

HRESULT CLevel_Town::Ready_Layer_Monster(const _wstring& strLayerTag)
{  
    CMonster::MonsterDesc desc;

    CBody::BODY_DESC bodyDesc;
    bodyDesc.eRenderGroup = ENUM_TO_UINT(RENDERGROUP::NONALPHA);
    bodyDesc.modelName = L"ZolGreenAnim";

    desc.BodyDesc = &bodyDesc;

     
    desc.iAttack = 10;
    desc.MaxHp = 5;
    desc.fActionRange = 10.f;

    desc.ObjTag = L"CM_GreenZol" + to_wstring(0);
    CTransform::TRANSFORM_DESC TransDesc = {};
    TransDesc.vLocalPosition = { 30.f,5.f,20.f,1.f };
    TransDesc.vLocalRotation = { 0.f,180.f,0.f,1.f };

    TransDesc.fSpeedPerSec = 2.f;
    TransDesc.fRotationPerSec = 10.f;

    desc.TransformDesc = &TransDesc;



    if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(ENUM_TO_UINT(LEVEL_ID::STATIC),
        PROTO_OBJ_NAME(L"CM_GreenZol"),
        ENUM_TO_UINT(LEVEL_ID::TOWN),
        strLayerTag, &desc)))
        return E_FAIL;

    

    return S_OK;

}

HRESULT CLevel_Town::Ready_Layer_NPC(const _wstring& strLayerTag)
{
    CNPC::NPC_DESC pDesc;

    CTransform::TRANSFORM_DESC pTransDesc;
    pTransDesc.vLocalPosition= { 40.f,9.5f,19.f,1.f };
    pDesc.ObjTag = L"NPC_Richard";
    pDesc.pTarget = nullptr;
    pDesc.ModelName = L"RichardAnim";

    pDesc.TransformDesc = &pTransDesc;

    CNPC_Richard* pNpc_Richard = CNPC_Richard::Create(m_pDevice, m_pContext, &pDesc);
    if (pNpc_Richard)
    {
        if(FAILED(m_pGameInstance->Add_GameObject_To_Layer(ENUM_TO_UINT(LEVEL_ID::TOWN), strLayerTag, pNpc_Richard)))
        return E_FAIL;

    }

    CInteraction_Manager::GetInstance()->RegisterInteractable(dynamic_cast<CIInteractable*>(pNpc_Richard));

    return S_OK;
}


void CLevel_Town::OnEnter()
{
    m_pGameInstance->Set_MainCamera(CAMERA_TYPE::TARGET);
    CGameObject* pPlayerObject = m_pGameInstance->Find_GameObject(ENUM_TO_UINT(LEVEL_ID::STATIC), L"Player_Layer", L"Player");


    //미니맵 타겟 = 플레이어
    CCamera_Base* pMinimapCamera = m_pGameInstance->Find_Camera(CAMERA_TYPE::MINIMAP);
    if (pMinimapCamera)
    {
        pMinimapCamera->Set_Target(pPlayerObject);
    }




    CGameObject* pMainCamera = m_pGameInstance->Find_Camera(CAMERA_TYPE::TARGET);
    if (pMainCamera)
    {
        CMainCamera* ppMainCamera = dynamic_cast<CMainCamera*>(pMainCamera);
        CheckNull(ppMainCamera);
        ppMainCamera->Set_Target(pPlayerObject, true);
    }


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
}

void CLevel_Town::OnResume()
{ 
    int A=0;
}

void CLevel_Town::OnPause()
{
    /*2회 호출막기..*/
    if (Get_State() == LEVELSTATE::HIDDEN || Get_State() == LEVELSTATE::PAUSE)
        return;
    int A=0;
}

void CLevel_Town::OnExit()
{
    int A=0;
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
