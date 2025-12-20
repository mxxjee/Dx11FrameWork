#include "CMapLoader.h"
#include "CTerrain_Manager.h"
#include "CTerrain.h"
#include "CBody.h"
#include "CGameInstance.h"
#include "CInteraction_Manager.h"
#include "CBounding_AABB.h"
#include "CBoxColliderComponent.h"


unordered_map<size_t, vector<DefaultInteractionData>> CMapLoader::m_sceneInteractbles = {};



USING(Client)

HRESULT CMapLoader::Load_Town()
{
    Load_Terrain("../../Resource/Data/Map/Final_Town.json");
    Load_NavMesh(LEVEL_ID::TOWN, "../../Resource/Data/Map/Terrain11_Nav.dat");
    Load_Interaction("Level_Town", "../../Resource/Data/Map/Interaction3.json");
    return S_OK;
}



///////////////////읽어와서 구조체만 모아둠.
void CMapLoader::Load_Terrain(const string& LoadPath)
{
    vector<tagLoadTerrainData> LoadDatas = CGameInstance::GetInstance()->Load_Terrains_Runtime(LoadPath);
    for (int i = 0; i < LoadDatas.size(); ++i)
    {
        tagLoadTerrainData TargetData = LoadDatas[i];

        CTerrain::TERRAINOBJECT_DESC desc;

        CBody::BODY_DESC bodyDesc;
        bodyDesc.eRenderGroup = ENUM_TO_UINT(RENDERGROUP::NONALPHA);
        bodyDesc.modelName = StringToWString(TargetData.ModelName);
        desc.BodyDesc = &bodyDesc;

        desc.ObjTag = L"Terrain" + bodyDesc.modelName;
        desc.vIndex = TargetData.vIndex;

        CTransform::TRANSFORM_DESC TransDesc = {};
        TransDesc.vLocalPosition = TargetData.vPosition;
        TransDesc.vLocalRotation = TargetData.vRotation;
        TransDesc.vLocalScale = TargetData.vScale;

        desc.TransformDesc = &TransDesc;

        CTerrain_Base* pTerrain = dynamic_cast<CTerrain_Base*>(CGameInstance::GetInstance()->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_TO_UINT(LEVEL_ID::STATIC),
            PROTO_OBJ_NAME(L"Terrain"), &desc));

        if (pTerrain)
            CGameInstance::GetInstance()->Register_Terrain(pTerrain->Get_Tag(), pTerrain);
    }

    //씬에 들어와야 활성화

    //CGameInstance::GetInstance()->Set_EnableUpdate(false);
    //CGameInstance::GetInstance()->Set_EnalbeUpdateRender(false);

}

void CMapLoader::Load_NavMesh(LEVEL_ID LevelID,const string& LoadPath)
{
    CGameInstance* pGameInstance = CGameInstance::GetInstance();

    ///////////////////////Navigation 불러오기
    pGameInstance->Load_NavMesh(ENUM_TO_UINT(LevelID), LoadPath);

#ifdef _DEBUG
    pGameInstance->Set_NavMeshShader(pGameInstance->Find_Shader(L"VtxPos"));
#endif
}

void CMapLoader::Load_Interaction(string LevelName, const string& LoadPath)
{
    size_t hashKey = hash<string>()(LevelName);

    CInteraction_Manager::GetInstance()->Load_Data(LevelName, m_sceneInteractbles[hashKey], LoadPath);

 


}

HRESULT CMapLoader::Make_Object_By_LoadData(string SceneName, CLayer* pLayer)
{

    
    
    size_t hashKey = hash<string>()(SceneName);
    vector< DefaultInteractionData> Infos = m_sceneInteractbles[hashKey];
    CheckTrueResult(Infos.empty(), E_FAIL);

    CGameInstance* pGameInstance = CGameInstance::GetInstance();
    _uint SceneID = 0;


    if (SceneName == "Level_Town")
        SceneID = ENUM_TO_UINT(LEVEL_ID::TOWN);


    int iIdx = 0;
    for (auto& Info : Infos)
    {
        CInteractionObject::Interaction_DESC Desc;
        Desc.eInteractionType = ENUM_TO_UINT(InteractionType::OBJECT);
        Desc.eInteract_Object_Type = Info.InteractionType;
        Desc.m_iLevelID = SceneID;

        Desc.eRenderGroup = ENUM_TO_UINT(RENDERGROUP::NONALPHA);
        Desc.ModelName = StringToWString(Info.ModelName);

        Desc.SceneName = SceneName;
        Desc.bAnimated = false;
        Desc.ObjTag = StringToWString(Info.ModelName) + to_wstring(iIdx);

        CTransform::TRANSFORM_DESC TransDesc;
        TransDesc.vLocalPosition = _float4(Info.vPos.x, Info.vPos.y, Info.vPos.z, 1.f);
        TransDesc.vLocalRotation = _float4(Info.vRotation.x, Info.vRotation.y, Info.vRotation.z, 0.f);
        TransDesc.vLocalScale = _float4(Info.vScale.x, Info.vScale.y, Info.vScale.z, 1.f);
        Desc.TransformDesc = &TransDesc;


        CBoxColliderComponent::COLLIDER_DESC ColDesc;
        ColDesc.m_eColGroup = ENUM_TO_UINT(COLLISION_GROUP::INTERACTION);
        CBounding_AABB::BOUNDING_AABB_DESC aabbDesc;
        aabbDesc.vCenter = Info.ColliderCenter;
        aabbDesc.Extents = Info.ColliderExtent;
        ColDesc.m_BoundingDesc = &aabbDesc;
        ColDesc.m_iLevelID = SceneID;
        Desc.pColliderComp = &ColDesc;


        wstring ProtoTag = L"";

        switch (Interact_Object_Type(Desc.eInteract_Object_Type))
        {
        case Interact_Object_Type::LAWN:
        {
            Desc.fTargetDistance = 2.f;
            ProtoTag = L"Interaction_Lawn";
        }
            break;


        case Interact_Object_Type::ROCK:
        {
            Desc.fTargetDistance = 3.f;
            ProtoTag = L"Interaction_Rock";
        }
            break;

        }
       if(FAILED(pGameInstance->Add_GameObject_To_Layer(ENUM_TO_UINT(LEVEL_ID::STATIC),
            PROTO_OBJ_NAME(ProtoTag),
            SceneID,
            L"Interaction_Layer", &Desc)))
            return E_FAIL;


       ++iIdx;
        

    }

    return S_OK;
}

