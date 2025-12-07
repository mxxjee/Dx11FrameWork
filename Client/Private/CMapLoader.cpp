#include "CMapLoader.h"
#include "CTerrain_Manager.h"
#include "CTerrain.h"
#include "CBody.h"
#include "CGameInstance.h"

HRESULT CMapLoader::Load_Town()
{
    Load_Terrain("../../Resource/Data/Map/Final_Town.json");
    Load_NavMesh(LEVEL_ID::TOWN, "../../Resource/Data/Map/Terrain21_Nav.dat");

    return S_OK;
}



///////////////////
void CMapLoader::Load_Terrain(const string& LoadPath)
{
    vector<tagLoadTerrainData> LoadDatas = CGameInstance::GetInstance()->Load_Terrains_Runtime(LoadPath);
    for (int i = 0; i < LoadDatas.size(); ++i)
    {
        tagLoadTerrainData TargetData = LoadDatas[i];

        CTerrain::TERRAINOBJECT_DESC desc;

        CBody::BODY_DESC bodyDesc;
        bodyDesc.eRenderGroup = ENUM_TO_UINT(RENDERGROUP::PRIORITY);
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
    CGameInstance::GetInstance()->Set_EnableUpdate(false);

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
