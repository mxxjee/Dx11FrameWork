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
    swprintf_s(szTitle, L"Boss ¾ÀÀÔ´Ï´Ù. FPS : %.1f", m_pGameInstance->Get_FPS(L"Timer_60"));

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

    m_pGameInstance->Set_MainCells(ENUM_TO_UINT(LEVEL_ID::BOSS));


    return S_OK;
}

HRESULT CLevel_Boss::Ready_Layer_Monster(const _wstring& strLayerTag)
{
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



    return S_OK;
}

void CLevel_Boss::OnEnter()
{
    __super::OnEnter();
    m_pGameInstance->Set_MainCamera(CAMERA_TYPE::TARGET);

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
