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

            EffectData* Data=new EffectData();
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

            if (jEffectData.contains("TargetScale"))
            {
                Data->TargetScale.x = jEffectData["TargetScale"][0].get<float>();
                Data->TargetScale.y = jEffectData["TargetScale"][1].get<float>();
                Data->TargetScale.z = jEffectData["TargetScale"][2].get<float>();
                Data->TargetScale.w = jEffectData["TargetScale"][3].get<float>();

                Data->fScaleSpeed = jEffectData["fScaleSpeed"].get<float>();

                Data->vRotationAxis.x = jEffectData["vRotationAxis"][0].get<float>();
                Data->vRotationAxis.y = jEffectData["vRotationAxis"][1].get<float>();
                Data->vRotationAxis.z = jEffectData["vRotationAxis"][2].get<float>();
                Data->vRotationAxis.w = jEffectData["vRotationAxis"][3].get<float>();
                Data->fRotationSpeed = jEffectData["fRotationSpeed"].get<float>();

                Data->vMoveDir.x = jEffectData["vMoveDir"][0].get<float>();
                Data->vMoveDir.y = jEffectData["vMoveDir"][1].get<float>();
                Data->vMoveDir.z = jEffectData["vMoveDir"][2].get<float>();
                Data->vMoveDir.w = jEffectData["vMoveDir"][3].get<float>();
                Data->fMoveSpeed = jEffectData["fMoveSpeed"].get<float>();


                Data->bUseScaleAnim = jEffectData["bUseScaleAnim"].get<bool>();
                Data->bUseRotationAnim = jEffectData["bUseRotationAnim"].get<bool>();
                Data->bUseMoveAnim = jEffectData["bUseMoveAnim"].get<bool>();
               
            }

            if (jEffectData.contains("fIntensity"))
                Data->fIntensity = jEffectData["fIntensity"].get<float>();


            stringID ID = stringID(Name);
            m_Datas.emplace(ID.m_Hash, Data);
        }
    }
    return S_OK;
}

HRESULT CEffectData_Manager::Save_To_Json(wstring& ModelName, EffectData Data)
{
    json jMeta;

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

    /////////메쉬자체의 애니값///////
    ///////Scale//////////
    jMeta["bUseScaleAnim"] = Data.bUseScaleAnim;

    json TargetScale = json::array();
    TargetScale.push_back(Data.TargetScale.x);
    TargetScale.push_back(Data.TargetScale.y);
    TargetScale.push_back(Data.TargetScale.z);
    TargetScale.push_back(1.f);
    jMeta["TargetScale"] = TargetScale;
    jMeta["fScaleSpeed"] = Data.fScaleSpeed;

    ///////Rotation//////////
    jMeta["bUseRotationAnim"] = Data.bUseRotationAnim;

    json vRotationAxis = json::array();
    vRotationAxis.push_back(Data.vRotationAxis.x);
    vRotationAxis.push_back(Data.vRotationAxis.y);
    vRotationAxis.push_back(Data.vRotationAxis.z);
    vRotationAxis.push_back(1.f);
    jMeta["vRotationAxis"] = vRotationAxis;
    jMeta["fRotationSpeed"] = Data.fRotationSpeed;

    ///////Rotation//////////
    jMeta["bUseMoveAnim"] = Data.bUseMoveAnim;
    json vMoveDir = json::array();
    vMoveDir.push_back(Data.vMoveDir.x);
    vMoveDir.push_back(Data.vMoveDir.y);
    vMoveDir.push_back(Data.vMoveDir.z);
    vMoveDir.push_back(0.f);

    jMeta["vMoveDir"] = vMoveDir;
    jMeta["fMoveSpeed"] = Data.fMoveSpeed;
    jMeta["fIntensity"] = Data.fIntensity;
    string FinalPath = "../../Resource/Data/EffectData/"+WStringToUTF8(ModelName)+".json";
    ofstream file(FinalPath);
    file << std::setw(4) << jMeta;

    file.close();

    return S_OK;
}

void CEffectData_Manager::Free()
{
    __super::Free();
    for (auto& pData : m_Datas)
    {
        if (pData.second)
            Safe_Delete(pData.second);
    }
}
