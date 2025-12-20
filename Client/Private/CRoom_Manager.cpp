#include "CRoom_Manager.h"
#include "CGameObject.h"
#include "CLayer.h"
#include "CRoom.h"
#include "CStaticBody.h"
#include "CGameInstance.h"
#include "CTrigger_Box.h"
#include "CRoomTrigger.h"

#include "Parsing_RoomInfo.h"
#include "Parsing_TriggerInfo.h"
#include "CNPC.h"
#include "CNPC_Richard.h"

#include "CNavMesh_Manager.h"




IMPLEMENT_SINGLETON(CRoom_Manager)

USING(Client)
HRESULT CRoom_Manager::Initialize(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pContext, CLayer* pEnv, CLayer* pNPC, CLayer* pTrigger)
{
    m_pGameInstance = CGameInstance::GetInstance();
    Safe_AddRef(m_pGameInstance);


    m_pDevice = _pDevice;
    m_pContext = _pContext;

    CheckNullResult(pEnv,E_FAIL);
    CheckNullResult(pNPC,E_FAIL);
    CheckNullResult(pTrigger,E_FAIL);

    m_pEnviromentLayer = pEnv;
    Safe_AddRef(pEnv);

    m_pNPCLayer = pNPC;
    Safe_AddRef(pNPC);

    m_pTriggerLayer = pTrigger;
    Safe_AddRef(pTrigger);

    return S_OK;
}

void CRoom_Manager::Switch_Room(const string& strRoomName)
{
    //같은방이라면 네브메쉬요청만바꾼다.
    if (m_strCurrentRoomID == strRoomName)
    {
        m_pGameInstance->Set_MainCells(ENUM_TO_UINT(LEVEL_ID::ROOM));
        return;
    }

    /////기존꺼 지우기(퇴장처리)
    Clear_Room();


    //불러올 방 자료가져오기
    RoomPackage* pNextPackage = nullptr;
    size_t HashKey = hash<string>()(strRoomName);

    auto iter = m_mapCachedRooms.find(HashKey);
    
    //재사용
    if (iter != m_mapCachedRooms.end())
    {
        pNextPackage = iter->second;
       
        m_pGameInstance->Reset_NaveMesh(ENUM_TO_UINT(LEVEL_ID::ROOM));
        if (FAILED(m_pGameInstance->Load_NavMesh(ENUM_TO_UINT(LEVEL_ID::ROOM), pNextPackage->m_navMeshFile)))
            return;
        m_pGameInstance->Set_MainCells(ENUM_TO_UINT(LEVEL_ID::ROOM));

        m_vSpawnPosition = _float4(pNextPackage->m_vPlayerSpawnPoin.x,
            pNextPackage->m_vPlayerSpawnPoin.y,
            pNextPackage->m_vPlayerSpawnPoin.z,
            1.f);
                        
        pNextPackage->Set_Active(true);

        //NPC의 메인셀 다시 설정
        for (auto& pNpc : pNextPackage->NPCs)
        {
            CNPC* ppNpc = dynamic_cast<CNPC*>(pNpc);
            ppNpc->Change_NavMesh();
        }
        
    }
    //새로운 로드
    else
    {
        pNextPackage = new RoomPackage();
        if (FAILED(Load_Room_From_Json(strRoomName, pNextPackage)))
        {
            MSG_BOX("Room Load Failed!");
            Safe_Delete(pNextPackage);
            return;
        }

        m_mapCachedRooms.emplace(HashKey, pNextPackage);

    }

    Enter_Room(pNextPackage,true);

}

void CRoom_Manager::Clear_Cache()
{
    for (auto& pair : m_mapCachedRooms)
    {
        if (pair.second)
            Safe_Delete(pair.second);
    }

    m_mapCachedRooms.clear();
    m_strCurrentRoomID = "";

}

HRESULT CRoom_Manager::Load_Room_From_Json(const string& strRoomName, RoomPackage* pOutPackage)
{
    string BaseFilePath = "../../Resource/Data/Map/Room/";
    string FullPath = BaseFilePath + strRoomName + "_room.json";
    ifstream file(FullPath);
    json jRoomData = json::parse(file);

    ///Room모델생성
    CRoom::MODELOBJECT_DESC     RoomDesc;
    string RoomName = jRoomData["RoomName"].get<string>();
    _float4 vRoomPos = _float4(jRoomData["Position"][0].get<float>(),
                                jRoomData["Position"][1].get<float>(),
                                jRoomData["Position"][2].get<float>(),
                                1.f);

 
    wstring WRoomName = StringToWString(RoomName);
    RoomDesc.ObjTag = WRoomName + L"_Room";
    
    CStaticBody::BODY_DESC  BodyDesc;
    BodyDesc.eRenderGroup = ENUM_TO_UINT(RENDERGROUP::NONALPHA);
    BodyDesc.modelName = WRoomName;
    RoomDesc.BodyDesc = &BodyDesc;


    CTransform::TRANSFORM_DESC TransDesc;
    TransDesc.vLocalPosition = vRoomPos;
    RoomDesc.TransformDesc = &TransDesc;


    CBase* pBaseRoom = m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_TO_UINT(LEVEL_ID::STATIC), PROTO_OBJ_NAME(L"Room"), &RoomDesc);
    CGameObject* pRoom = dynamic_cast<CGameObject*>(pBaseRoom);
    if (pRoom)
    {
       // Safe_AddRef(pRoom);
        pOutPackage->m_RoomName = RoomName;
        pOutPackage->EnvObjs.push_back(pRoom);

    }
    //Navmesh읽어오기
    pOutPackage->m_navMeshFile = jRoomData["NavData"];
    m_pGameInstance->Reset_NaveMesh(ENUM_TO_UINT(LEVEL_ID::ROOM));
    if (FAILED(m_pGameInstance->Load_NavMesh(ENUM_TO_UINT(LEVEL_ID::ROOM), pOutPackage->m_navMeshFile)))
        return E_FAIL;

    m_pGameInstance->Set_MainCells(ENUM_TO_UINT(LEVEL_ID::ROOM));

    
    ///Trigger생성
    vector<RoomTrigger> TriggerInfos;
    for (auto& pTriggers : jRoomData["RoomTriggers"])
    {
        TriggerInfos.push_back(RoomTrigger::LoadJson(pTriggers));
    }

    int TriggeriIdx = 0;
    for (auto& pTriggerInfo : TriggerInfos)
    {
        CRoomTrigger::RoomTriggerDesc RoomTriggerDesc;
        RoomTriggerDesc.vCenter = pTriggerInfo.vCenter;
        RoomTriggerDesc.vExtents = pTriggerInfo.vExtents;
        RoomTriggerDesc.ObjTag = L"Trigger" + to_wstring(TriggeriIdx);
        RoomTriggerDesc.m_nextKey = pTriggerInfo.m_NextRoomID;
        RoomTriggerDesc.m_iLevelID = ENUM_TO_UINT(LEVEL_ID::ROOM);

        CTransform::TRANSFORM_DESC TransDesc;
        TransDesc.vLocalPosition = _float4(pTriggerInfo.vPos.x, pTriggerInfo.vPos.y, pTriggerInfo.vPos.z, 1.f);
        TransDesc.vLocalRotation = _float4(pTriggerInfo.vRotation.x, pTriggerInfo.vRotation.y, pTriggerInfo.vRotation.z, 1.f);
        TransDesc.vLocalScale = _float4(pTriggerInfo.vScale.x, pTriggerInfo.vScale.y, pTriggerInfo.vScale.z, 1.f);

        RoomTriggerDesc.TransformDesc = &TransDesc;
        CBase* pBaseTrigger= m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_TO_UINT(LEVEL_ID::STATIC), PROTO_OBJ_NAME(L"RoomTrigger"),&RoomTriggerDesc);
        CGameObject* pRoomTrigger = dynamic_cast<CGameObject*>(pBaseTrigger);
        if (pRoomTrigger)
        {
            //Safe_AddRef(pRoomTrigger);
            pOutPackage->Triggers.push_back(pRoomTrigger);

        }

        ++TriggeriIdx;
    }


    ///NPC생성
    //일단 포지션들불러온다.
    vector<PositionInfo> PositionIfos;
    for (auto& pTriggers : jRoomData["MapPositions"])
    {
        PositionIfos.push_back(PositionInfo::LoadJson(pTriggers));
    }

    //모델이름불러온다.
    vector<string>  NPCModelNames;
    for (auto& ModelName : jRoomData["NPC_ModelNames"])
        NPCModelNames.push_back(ModelName);


    for (auto& PosInfo : PositionIfos)
    {
        if (PosInfo.TargetName == "Player_SpawnPoint")
        {
            pOutPackage->m_vPlayerSpawnPoin = _float3(PosInfo.vPos.x, PosInfo.vPos.y, PosInfo.vPos.z);
            m_vSpawnPosition = _float4(PosInfo.vPos.x, PosInfo.vPos.y, PosInfo.vPos.z, 1.f);

        }
        else 
        {
            for (auto& ModelName : NPCModelNames)
            {
                if (PosInfo.TargetName.find(ModelName) != string::npos)
                {
                    _float3 NPCPos= PosInfo.vPos;
                    
                    //NPC소환.(일단 임시로 리차드)
                    if (FAILED(Load_NPC(RoomName,StringToWString(ModelName), NPCPos, pOutPackage)))
                        return E_FAIL;

                }
              
            }
           
        }
    }



    return S_OK;
}

void CRoom_Manager::Clear_Room()
{
    if (m_pEnviromentLayer)
        m_pEnviromentLayer->Clear();


    if (m_pNPCLayer)
        m_pNPCLayer->Clear();


    if (m_pTriggerLayer)
        m_pTriggerLayer->Clear();


    for (auto& pair : m_mapCachedRooms)
    {
        if (pair.second)
        {
            pair.second->Set_Active(false);
        }
    }

}

void CRoom_Manager::Enter_Room(RoomPackage* pPackage, bool bCached)
{
    for (auto& pObj : pPackage->EnvObjs)
    {
        m_pEnviromentLayer->Add_GameObject(pObj);
        Safe_AddRef(pObj);
        
    }

    for (auto& pObj : pPackage->NPCs)
    {
        m_pNPCLayer->Add_GameObject(pObj);
        Safe_AddRef(pObj);
    }


    for (auto& pObj : pPackage->Triggers)
    {
        m_pTriggerLayer->Add_GameObject(pObj);
        Safe_AddRef(pObj);
    }

    m_strCurrentRoomID = pPackage->m_RoomName;

}

HRESULT CRoom_Manager::Load_NPC(const string& RoomName, const wstring& ModelName, _float3 vPos, RoomPackage* pOut)
{

    CNPC::NPC_DESC pDesc;

    CTransform::TRANSFORM_DESC pTransDesc;
    pTransDesc.vLocalPosition = _float4(vPos.x, vPos.y,vPos.z, 1.f);
    pTransDesc.vLocalRotation = _float4(0.f, 180.f, 0.f, 1.f);
    pDesc.ObjTag = L"NPC_" + ModelName;
    pDesc.pTarget = nullptr;
    pDesc.ModelName = ModelName;
    pDesc.SceneName = RoomName;

    pDesc.m_iLevelID = ENUM_TO_UINT(LEVEL_ID::ROOM);
    pDesc.TransformDesc = &pTransDesc;


    CNPC* pNpc = CNPC::Create(m_pDevice, m_pContext, &pDesc);
    if (pNpc)
    {
        //Safe_AddRef(pNpc);
        pOut->NPCs.push_back(pNpc);
        return S_OK;
    }
   
    return E_FAIL;

}

void CRoom_Manager::Free()
{
    __super::Free();


    Clear_Cache();

    Safe_Release(m_pEnviromentLayer);
    Safe_Release(m_pNPCLayer);
    Safe_Release(m_pTriggerLayer);

    Safe_Release(m_pGameInstance);
}
