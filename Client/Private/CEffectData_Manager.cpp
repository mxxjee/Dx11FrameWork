#include "CEffectData_Manager.h"
#include "Client_Defines.h"

IMPLEMENT_SINGLETON(CEffectData_Manager)

USING(Client)

HRESULT CEffectData_Manager::Initialize()
{
    Load_AllData();

    return S_OK;
}

void CEffectData_Manager::Register_Data(wstring& ModelName, EffectData& Data)
{
    EffectData* pData = Find_Data(ModelName);
    if (pData != nullptr)
        return;

    else
    {
        EffectData* pNewData = new EffectData(Data);

        stringID ID = stringID(ModelName);
        m_Datas.emplace(ID.m_Hash, pNewData);
        Save_To_Json(ModelName,Data);
    }
}


void CEffectData_Manager::Update_Data(wstring& ModelName, EffectData& Data)
{
    EffectData* pData = Find_Data(ModelName);
    if (pData)
    {
        //요걸수정..
        (*pData) = Data;
       
    }


    //새로만들어서 수정
    else
    {
        Register_Data(ModelName, Data);
    }
}

EffectData* CEffectData_Manager::Find_Data(wstring ModelName)
{
    stringID FindID=stringID(ModelName);

    auto iter = m_Datas.find(FindID.m_Hash);
    if (iter == m_Datas.end())
        return nullptr;

    return iter->second;
}

HRESULT CEffectData_Manager::Load_AllData()
{

    for (const auto& entry : fs::recursive_directory_iterator("../../Resource/Data/EffectData"))
    {
        if (entry.path().extension() == ".json")
        {
            string FullPath = entry.path().string();
            wstring Name = entry.path().stem().wstring();

            ifstream file(FullPath);
            json jEffectData = json::parse(file);

            EffectData* Data = nullptr;

            //타입에따라 new
            string Type = jEffectData["DataName"];

            if (Type == "MESH")
                Data = new MeshData();

            else
                Data= new ParticleData();
         
            Data->vColor.x= jEffectData["Color"][0].get<float>();
            Data->vColor.y = jEffectData["Color"][1].get<float>();
            Data->vColor.z = jEffectData["Color"][2].get<float>();
            Data->vColor.w = jEffectData["Color"][3].get<float>();

            Data->fLifeTime = jEffectData["LifeTime"].get<float>();

            Data->fSpeed = jEffectData["Speed"].get<float>();
            Data->m_bLoop = jEffectData["Loop"].get<bool>();


            Data->InitOffSet.x = jEffectData["OffSet"][0].get<float>();
            Data->InitOffSet.y = jEffectData["OffSet"][1].get<float>();
            Data->InitOffSet.z = jEffectData["OffSet"][2].get<float>();
            Data->InitOffSet.w = jEffectData["OffSet"][3].get<float>();



            Data->InitRotation.x = jEffectData["Rotation"][0].get<float>();
            Data->InitRotation.y = jEffectData["Rotation"][1].get<float>();
            Data->InitRotation.z = jEffectData["Rotation"][2].get<float>();
            Data->InitRotation.w = jEffectData["Rotation"][3].get<float>();


            Data->InitScale.x = jEffectData["Scale"][0].get<float>();
            Data->InitScale.y = jEffectData["Scale"][1].get<float>();
            Data->InitScale.z = jEffectData["Scale"][2].get<float>();
            Data->InitScale.w = jEffectData["Scale"][3].get<float>();

            Data->fIntensity = jEffectData["fIntensity"].get<float>();

           if(Data->eType==EFFECT_DESC_TYPE::MESH)
                Load_To_Json_Mesh(jEffectData, Data);

           else
               Load_To_Json_Particle(jEffectData, Data);

            
            stringID ID = stringID(Name);
            m_Datas.emplace(ID.m_Hash, Data);
        }
    }
    return S_OK;
}

HRESULT CEffectData_Manager::Load_To_Json_Mesh(json& Json, EffectData* Data)
{
    MeshData* pDesc = static_cast<MeshData*>(Data);


	pDesc->TargetScale.x = Json["TargetScale"][0].get<float>();
	pDesc->TargetScale.y = Json["TargetScale"][1].get<float>();
	pDesc->TargetScale.z = Json["TargetScale"][2].get<float>();
	pDesc->TargetScale.w = Json["TargetScale"][3].get<float>();

	pDesc->fScaleSpeed = Json["fScaleSpeed"].get<float>();

	pDesc->vRotationAxis.x = Json["vRotationAxis"][0].get<float>();
	pDesc->vRotationAxis.y = Json["vRotationAxis"][1].get<float>();
	pDesc->vRotationAxis.z = Json["vRotationAxis"][2].get<float>();
	pDesc->vRotationAxis.w = Json["vRotationAxis"][3].get<float>();
	pDesc->fRotationSpeed = Json["fRotationSpeed"].get<float>();

	pDesc->vMoveDir.x = Json["vMoveDir"][0].get<float>();
	pDesc->vMoveDir.y = Json["vMoveDir"][1].get<float>();
	pDesc->vMoveDir.z = Json["vMoveDir"][2].get<float>();
	pDesc->vMoveDir.w = Json["vMoveDir"][3].get<float>();
	pDesc->fMoveSpeed = Json["fMoveSpeed"].get<float>();


	pDesc->bUseScaleAnim = Json["bUseScaleAnim"].get<bool>();
	pDesc->bUseRotationAnim = Json["bUseRotationAnim"].get<bool>();
	pDesc->bUseMoveAnim = Json["bUseMoveAnim"].get<bool>();







    return S_OK;
}

HRESULT CEffectData_Manager::Load_To_Json_Particle(json& Json, EffectData* Data)
{
    return S_OK;
}

HRESULT CEffectData_Manager::Save_To_Json(wstring& ModelName, EffectData Data)
{
    json jMeta;

    string strModelName = WStringToUTF8(ModelName);
    jMeta["DataName"] = strModelName;

    string Type;
    switch (Data.eType)
    {
    case EFFECT_DESC_TYPE::MESH:
        Type = "Mesh";
        break;

    case EFFECT_DESC_TYPE::PARTICLE:
        Type = "Particle";
        break;
    }
    jMeta["Type"] = Type;

    json Color = json::array();
    Color.push_back(Data.vColor.x);
    Color.push_back(Data.vColor.y);
    Color.push_back(Data.vColor.z);
    Color.push_back(Data.vColor.w);
    jMeta["Color"] = Color;


    jMeta["LifeTime"] = Data.fLifeTime;

    jMeta["Speed"] = Data.fSpeed;
    jMeta["Loop"] = Data.m_bLoop;

    json OffSet = json::array();
    OffSet.push_back(Data.InitOffSet.x);
    OffSet.push_back(Data.InitOffSet.y);
    OffSet.push_back(Data.InitOffSet.z);
    OffSet.push_back(Data.InitOffSet.w);
    jMeta["OffSet"] = OffSet;


    json Rotation = json::array();
    Rotation.push_back(Data.InitRotation.x);
    Rotation.push_back(Data.InitRotation.y);
    Rotation.push_back(Data.InitRotation.z);
    Rotation.push_back(Data.InitRotation.w);
    jMeta["Rotation"] = Rotation;

    json Scale = json::array();
    Scale.push_back(Data.InitScale.x);
    Scale.push_back(Data.InitScale.y);
    Scale.push_back(Data.InitScale.z);
    Scale.push_back(Data.InitScale.w);
    jMeta["Scale"] = Scale;
    jMeta["fIntensity"] = Data.fIntensity;

    //타입에 따른 저장방식 변경

    if (Data.eType == EFFECT_DESC_TYPE::MESH)
        Save_To_Json_Mesh(jMeta, Data);

    else
        Save_To_Json_Particle(jMeta, Data);

    string FinalPath = "../../Resource/Data/EffectData/" + WStringToUTF8(ModelName) + ".json";
    ofstream file(FinalPath);
    file << std::setw(4) << jMeta;

    file.close();

    return S_OK;
}

HRESULT CEffectData_Manager::Save_To_Json_Mesh(json& Json,EffectData Data)
{
    /////////메쉬자체의 애니값///////
    ///////Scale//////////
    MeshData* pMeshData = static_cast<MeshData*>(&Data);

    Json["bUseScaleAnim"] = pMeshData->bUseScaleAnim;

    json TargetScale = json::array();
    TargetScale.push_back(pMeshData->TargetScale.x);
    TargetScale.push_back(pMeshData->TargetScale.y);
    TargetScale.push_back(pMeshData->TargetScale.z);
    TargetScale.push_back(1.f);
    Json["TargetScale"] = TargetScale;
    Json["fScaleSpeed"] = pMeshData->fScaleSpeed;

    ///////Rotation//////////
    Json["bUseRotationAnim"] = pMeshData->bUseRotationAnim;

    json vRotationAxis = json::array();
    vRotationAxis.push_back(pMeshData->vRotationAxis.x);
    vRotationAxis.push_back(pMeshData->vRotationAxis.y);
    vRotationAxis.push_back(pMeshData->vRotationAxis.z);
    vRotationAxis.push_back(1.f);
    Json["vRotationAxis"] = vRotationAxis;
    Json["fRotationSpeed"] = pMeshData->fRotationSpeed;

    ///////Rotation//////////
    Json["bUseMoveAnim"] = pMeshData->bUseMoveAnim;
    json vMoveDir = json::array();
    vMoveDir.push_back(pMeshData->vMoveDir.x);
    vMoveDir.push_back(pMeshData->vMoveDir.y);
    vMoveDir.push_back(pMeshData->vMoveDir.z);
    vMoveDir.push_back(0.f);

    Json["vMoveDir"] = vMoveDir;
    Json["fMoveSpeed"] = pMeshData->fMoveSpeed;


    return S_OK;
}

HRESULT CEffectData_Manager::Save_To_Json_Particle(json& Json,EffectData Data)
{
    /////////메쉬자체의 애니값///////
   ///////Scale//////////
    ParticleData* pParticleData = static_cast<ParticleData*>(&Data);

    Json["NumInstance"] = pParticleData->iNumInstance;

    json Range = json::array();
    Range.push_back(pParticleData->vRange.x);
    Range.push_back(pParticleData->vRange.y);
    Range.push_back(pParticleData->vRange.z);
    Json["vRange"] = Range;

    json SizeRange = json::array();
    SizeRange.push_back(pParticleData->vSizeRange.x);
    SizeRange.push_back(pParticleData->vSizeRange.y);
    Json["vSizeRange"] = SizeRange;

    json SpeedRange = json::array();
    SpeedRange.push_back(pParticleData->vSpeedRange.x);
    SpeedRange.push_back(pParticleData->vSpeedRange.y);
    Json["vSpeedRange"] = SpeedRange;

    json LifeTimeRange = json::array();
    LifeTimeRange.push_back(pParticleData->vLifeTimeRange.x);
    LifeTimeRange.push_back(pParticleData->vLifeTimeRange.y);
    Json["vLifeTimeRange"] = LifeTimeRange;

    Json["fGravity"] = pParticleData->fGravity;


    return S_OK;
}

void CEffectData_Manager::Free()
{
    __super::Free();
    //끄기전에 저장한번해주고 끄기
    for (auto& pData : m_Datas)
    {
        if (pData.second)
        {
            wstring DataName = StringToWString(pData.second->DataName);

            Save_To_Json(DataName, *pData.second);
            Safe_Delete(pData.second);;

        }
    }
}
