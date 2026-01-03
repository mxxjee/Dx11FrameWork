#include "CEffectData_Manager.h"

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

            Data->fLifeTime = jEffectData["Speed"].get<float>();

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
