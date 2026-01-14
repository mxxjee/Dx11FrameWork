#include "CLight_Manager.h"
#include "CLight.h"
#include "CShader.h"
#include "CGameInstance.h"
#include "CVIBuffer_Rect.h"




CLight_Manager::CLight_Manager()
{
}

HRESULT CLight_Manager::Initialize(_uint iLevelNum)
{
    m_DirectionalLights.resize(iLevelNum);
    m_Lights.resize(iLevelNum);

    return S_OK;
}

const LIGHT_DESC* CLight_Manager::Get_LightDesc(_uint iLevelID, _uint iIndex)
{
    auto iter = m_Lights[iLevelID].begin();
    for (_uint i = 0; i < iIndex; ++i)
        ++iter;


    return (*iter)->Get_LightDesc();

}

CLight* CLight_Manager::Get_Light(_uint iLevelID, _uint iIndex)
{
    auto iter = m_Lights[iLevelID].begin();
    for (_uint i = 0; i < iIndex; ++i)
        ++iter;


    return (*iter);

}

CLight* CLight_Manager::Get_Light(_uint iLevelID, wstring LightName)
{
    CheckTrueResult(m_Lights[iLevelID].empty(), nullptr);

    for (auto& pLight : m_Lights[iLevelID])
    {
        const LIGHT_DESC* pDesc = pLight->Get_LightDesc();
        if (pDesc->LightName == LightName)
            return pLight;
    }

    return nullptr;
}

CLight* CLight_Manager::Get_DirectionLight(_uint iLevelID)
{
    CLight* pLight = m_DirectionalLights[iLevelID];


    return pLight;
}

//레벨에 맞는 조명리스트를 모두가져온다.
list<CLight*> CLight_Manager::Get_Lights(_uint iLevelID)
{
    if (m_Lights[iLevelID].empty())
        return list<CLight*>();


    else
        return m_Lights[iLevelID];
}

HRESULT CLight_Manager::Bind_Lights(CShader* pShader)
{  

    //size_t LightSize = m_Lights.size();
    //m_iLevelID = CGameInstance::GetInstance()->Get_CurrentLevelID();
    //const LIGHT_DESC* pDirectional_LightDesc = nullptr;

    ////전역조명 바인딩.
    //if (m_DirectionalLights[m_iLevelID])
    //{
    //    pDirectional_LightDesc = m_DirectionalLights[m_iLevelID]->Get_LightDesc();

    //    Bind_Directional_Light(pShader, pDirectional_LightDesc);
    //}

    //점 조명들 바인딩.
    //현재 씬에있는 조명데이터 싹가져와
    //활성화 되어있는 애들만 셰이더에 보내자.
    


    return S_OK;
}



HRESULT CLight_Manager::Add_Light(_uint iLevelID, const LIGHT_DESC& LightDesc)
{
    if (iLevelID >= m_Lights.size())
        return E_FAIL;

    CLight* pLight = CLight::Create(LightDesc);
    CheckNullResult(pLight, E_FAIL);
    


    if (LightDesc.eType == LIGHT::POINT)
        m_Lights[iLevelID].push_back(pLight);
      

    else
        m_DirectionalLights[iLevelID]=pLight;

    return S_OK;
}

void CLight_Manager::Add_LightValue(const LIGHT_DESC* LightDesc)
{
    int CurrentIdx = m_PointLightNum;

    m_LightValues.g_vPL_Position[CurrentIdx]=LightDesc->vPosition;
    m_LightValues.g_vPL_Range[CurrentIdx]=LightDesc->fRange;

    m_LightValues.g_vPL_Diffuse[CurrentIdx]=LightDesc->vDiffuse;
    m_LightValues.g_vPL_Ambient[CurrentIdx]=LightDesc->vAmbient;
    m_LightValues.g_vPL_Specular[CurrentIdx]=LightDesc->vSpecular;
 
}

HRESULT CLight_Manager::Bind_Directional_Light(class CShader* pShader, class CVIBuffer_Rect* pVIBuffer, const LIGHT_DESC* pLightDesc)
{

    m_DirectionLightBuffer.g_vLightDirection = pLightDesc->vDirection;
    m_DirectionLightBuffer.g_vLightDiffuse = pLightDesc->vDiffuse;
    m_DirectionLightBuffer.g_vLightAmbient = pLightDesc->vAmbient;
    m_DirectionLightBuffer.g_vLightSpecular = pLightDesc->vSpecular;
  
   
    CGameInstance::GetInstance()->CopyData_Buffer("DirectionLightBuffer", &m_DirectionLightBuffer, sizeof(m_DirectionLightBuffer));

    pShader->Begin("DirectionalLight");
    pVIBuffer->Bind_Resource();
    pVIBuffer->Render();

    return S_OK;
}

HRESULT CLight_Manager::Bind_Point_Light(CShader* pShader, class CVIBuffer_Rect* pVIBuffer)
{
   Clear_PointLightBuffer();

   for (auto& pLight : m_Lights[m_iLevelID])
   {
       if (!pLight || !pLight->IsActive())
           continue;

       const LIGHT_DESC* desc = pLight->Get_LightDesc();



       Add_LightValue(desc);
       m_PointLightNum += 1;

   }

   pShader->Bind_RawValue("g_PointLightNum", &m_PointLightNum, sizeof(int));

    CGameInstance::GetInstance()->CopyData_Buffer("PointLightBuffer", &m_LightValues, sizeof(m_LightValues));

    pShader->Begin("PointLight");
    pVIBuffer->Bind_Resource();
    pVIBuffer->Render();
    return S_OK;
}

void CLight_Manager::Render(CShader* pShader, CVIBuffer_Rect* pVIBuffer)
{
    m_iLevelID = CGameInstance::GetInstance()->Get_CurrentLevelID();
    CheckNull(m_DirectionalLights[m_iLevelID]);

    Bind_Directional_Light(pShader, pVIBuffer, m_DirectionalLights[m_iLevelID]->Get_LightDesc());
   

    ////점조명 렌더..
    Bind_Point_Light(pShader,pVIBuffer);

}

HRESULT CLight_Manager::Save_Data()
{
    string FileName = "";
    int FileNumber = 0;

    for (const auto& entry : fs::recursive_directory_iterator("../../Resource/Data/Map/Lights/"))
    {
        if (entry.path().extension() == ".json")
            ++FileNumber;
    }

    json jMeta;

    
    //Directional Light저장
    Save_DirectionLight(jMeta);
    Save_PointLights(jMeta);


    string FinalPath = "../../Resource/Data/Map/Lights/LightData"+ to_string(FileNumber) + ".json";
    ofstream file(FinalPath);
    file << std::setw(4) << jMeta;

    file.close();

    MSG_BOX("Complete to Save");

    return S_OK;
}

HRESULT CLight_Manager::Load_Data(_uint iLevelID,string FilePath)
{
    //DirectionData변경


	ifstream file(FilePath);
	json jLightData = json::parse(file);

	Load_DirectionLight(iLevelID, jLightData);
	Load_PointLights(iLevelID, jLightData);


    return S_OK;
}

void CLight_Manager::Save_DirectionLight(json& JSon)
{
    _uint iCurrentLevel = CGameInstance::GetInstance()->Get_CurrentLevelID();
    const LIGHT_DESC* pLight_Desc = m_DirectionalLights[iCurrentLevel]->Get_LightDesc();


    string Type;

    json DirectionInfo;
    DirectionInfo["Type"] = "DIRECTIONAL";

    string name = WStringToUTF8(pLight_Desc->LightName);
    DirectionInfo["LightName"] = name;
    
    json Color = json::array();
    Color.push_back(pLight_Desc->vDiffuse.x);
    Color.push_back(pLight_Desc->vDiffuse.y);
    Color.push_back(pLight_Desc->vDiffuse.z);
    Color.push_back(pLight_Desc->vDiffuse.w);
    DirectionInfo["Diffuse"] = Color;

    json Ambient = json::array();
    Ambient.push_back(pLight_Desc->vAmbient.x);
    Ambient.push_back(pLight_Desc->vAmbient.y);
    Ambient.push_back(pLight_Desc->vAmbient.z);
    Ambient.push_back(pLight_Desc->vAmbient.w);
    DirectionInfo["Ambient"] = Ambient;


    json Specular = json::array();
    Specular.push_back(pLight_Desc->vSpecular.x);
    Specular.push_back(pLight_Desc->vSpecular.y);
    Specular.push_back(pLight_Desc->vSpecular.z);
    Specular.push_back(pLight_Desc->vSpecular.w);
    DirectionInfo["Specular"] = Specular;

    json Direction = json::array();
    Direction.push_back(pLight_Desc->vDirection.x);
    Direction.push_back(pLight_Desc->vDirection.y);
    Direction.push_back(pLight_Desc->vDirection.z);
    Direction.push_back(pLight_Desc->vDirection.w);
    DirectionInfo["Direction"] = Direction;


    JSon["DirectionLight"] = DirectionInfo;

}

void CLight_Manager::Save_PointLights(json& JSon)
{
    _uint iCurrentLevel = CGameInstance::GetInstance()->Get_CurrentLevelID();

    list<class CLight*>         CurrentLevelLights = m_Lights[iCurrentLevel];
    
    for (auto& pLight : CurrentLevelLights)
    {
        const LIGHT_DESC* pLight_Desc = pLight->Get_LightDesc();
        json LightInfo;

        LightInfo["Type"] = "POINT";

        string name = WStringToUTF8(pLight_Desc->LightName);
        LightInfo["LightName"] = name;

        json Color = json::array();
        Color.push_back(pLight_Desc->vDiffuse.x);
        Color.push_back(pLight_Desc->vDiffuse.y);
        Color.push_back(pLight_Desc->vDiffuse.z);
        Color.push_back(pLight_Desc->vDiffuse.w);
        LightInfo["Diffuse"] = Color;

        json Ambient = json::array();
        Ambient.push_back(pLight_Desc->vAmbient.x);
        Ambient.push_back(pLight_Desc->vAmbient.y);
        Ambient.push_back(pLight_Desc->vAmbient.z);
        Ambient.push_back(pLight_Desc->vAmbient.w);
        LightInfo["Ambient"] = Ambient;


        json Specular = json::array();
        Specular.push_back(pLight_Desc->vSpecular.x);
        Specular.push_back(pLight_Desc->vSpecular.y);
        Specular.push_back(pLight_Desc->vSpecular.z);
        Specular.push_back(pLight_Desc->vSpecular.w);
        LightInfo["Specular"] = Specular;

        json Direction = json::array();
        Direction.push_back(pLight_Desc->vDirection.x);
        Direction.push_back(pLight_Desc->vDirection.y);
        Direction.push_back(pLight_Desc->vDirection.z);
        Direction.push_back(pLight_Desc->vDirection.w);
        LightInfo["Direction"] = Direction;


        json Position = json::array();
        Position.push_back(pLight_Desc->vPosition.x);
        Position.push_back(pLight_Desc->vPosition.y);
        Position.push_back(pLight_Desc->vPosition.z);
        Position.push_back(pLight_Desc->vPosition.w);
        LightInfo["Position"] = Position;

        json Range = json::array();
        Range.push_back(pLight_Desc->fRange.x);
        Range.push_back(pLight_Desc->fRange.y);
        Range.push_back(pLight_Desc->fRange.z);
        Range.push_back(pLight_Desc->fRange.w);
        LightInfo["Range"] = Range;

        JSon["PointLight"].push_back(LightInfo);
    }
}

HRESULT CLight_Manager::Load_DirectionLight(_uint iLevelID, json& JSon)
{
    if (m_DirectionalLights[iLevelID])
        Safe_Release(m_DirectionalLights[iLevelID]);

    if (JSon.contains("DirectionLight"))
    {
        json DirectionLightInfo = JSon["DirectionLight"];

        LIGHT_DESC Desc;
        Desc.eType = LIGHT::DIRECTIONAL;

        if (DirectionLightInfo.contains("LightName"))
        {
            string strLightName = DirectionLightInfo["LightName"].get<string>();
            Desc.LightName = StringToWString(strLightName);
        }

        if (DirectionLightInfo.contains("Diffuse"))
        {
            Desc.vDiffuse.x = DirectionLightInfo["Diffuse"][0].get<float>();
            Desc.vDiffuse.y = DirectionLightInfo["Diffuse"][1].get<float>();
            Desc.vDiffuse.z = DirectionLightInfo["Diffuse"][2].get<float>();
            Desc.vDiffuse.w = DirectionLightInfo["Diffuse"][3].get<float>();

        }

        if (DirectionLightInfo.contains("Ambient"))
        {
            Desc.vAmbient.x = DirectionLightInfo["Ambient"][0].get<float>();
            Desc.vAmbient.y = DirectionLightInfo["Ambient"][1].get<float>();
            Desc.vAmbient.z = DirectionLightInfo["Ambient"][2].get<float>();
            Desc.vAmbient.w = DirectionLightInfo["Ambient"][3].get<float>();

        }

        if (DirectionLightInfo.contains("Direction"))
        {
            Desc.vDirection.x = DirectionLightInfo["Direction"][0].get<float>();
            Desc.vDirection.y = DirectionLightInfo["Direction"][1].get<float>();
            Desc.vDirection.z = DirectionLightInfo["Direction"][2].get<float>();
            Desc.vDirection.w = DirectionLightInfo["Direction"][3].get<float>();

        }

        if (DirectionLightInfo.contains("Specular"))
        {
            Desc.vSpecular.x = DirectionLightInfo["Specular"][0].get<float>();
            Desc.vSpecular.y = DirectionLightInfo["Specular"][1].get<float>();
            Desc.vSpecular.z = DirectionLightInfo["Specular"][2].get<float>();
            Desc.vSpecular.w = DirectionLightInfo["Specular"][3].get<float>();

        }


        CLight* pLight = CLight::Create(Desc);
        CheckNullResult(pLight, E_FAIL);
        m_DirectionalLights[iLevelID] = pLight;

        return S_OK;
    }

    return E_FAIL;
}

HRESULT CLight_Manager::Load_PointLights(_uint iLevelID, json& Json)
{
    if (!m_Lights[iLevelID].empty())
    {
        for (auto& pLight : m_Lights[iLevelID])
            Safe_Release(pLight);
    }

    if (Json.contains("PointLight"))
    {
        for (auto& PointLightData : Json["PointLight"])
        {
            LIGHT_DESC Desc;
            Desc.eType = LIGHT::POINT;
            if (PointLightData.contains("LightName"))
            {
                string strLightName = PointLightData["LightName"].get<string>();
                Desc.LightName = StringToWString(strLightName);
            }

            if (PointLightData.contains("Diffuse"))
            {
                Desc.vDiffuse.x = PointLightData["Diffuse"][0].get<float>();
                Desc.vDiffuse.y = PointLightData["Diffuse"][1].get<float>();
                Desc.vDiffuse.z = PointLightData["Diffuse"][2].get<float>();
                Desc.vDiffuse.w = PointLightData["Diffuse"][3].get<float>();

            }

            if (PointLightData.contains("Ambient"))
            {
                Desc.vAmbient.x = PointLightData["Ambient"][0].get<float>();
                Desc.vAmbient.y = PointLightData["Ambient"][1].get<float>();
                Desc.vAmbient.z = PointLightData["Ambient"][2].get<float>();
                Desc.vAmbient.w = PointLightData["Ambient"][3].get<float>();

            }

            if (PointLightData.contains("Direction"))
            {
                Desc.vDirection.x = PointLightData["Direction"][0].get<float>();
                Desc.vDirection.y = PointLightData["Direction"][1].get<float>();
                Desc.vDirection.z = PointLightData["Direction"][2].get<float>();
                Desc.vDirection.w = PointLightData["Direction"][3].get<float>();

            }

            if (PointLightData.contains("Specular"))
            {
                Desc.vSpecular.x = PointLightData["Specular"][0].get<float>();
                Desc.vSpecular.y = PointLightData["Specular"][1].get<float>();
                Desc.vSpecular.z = PointLightData["Specular"][2].get<float>();
                Desc.vSpecular.w = PointLightData["Specular"][3].get<float>();

            }

            if (PointLightData.contains("Position"))
            {
                Desc.vPosition.x = PointLightData["Position"][0].get<float>();
                Desc.vPosition.y = PointLightData["Position"][1].get<float>();
                Desc.vPosition.z = PointLightData["Position"][2].get<float>();
                Desc.vPosition.w = PointLightData["Position"][3].get<float>();

            }


            if (PointLightData.contains("Range"))
            {
                Desc.fRange.x = PointLightData["Range"][0].get<float>();
                Desc.fRange.y = PointLightData["Range"][1].get<float>();
                Desc.fRange.z = PointLightData["Range"][2].get<float>();
                Desc.fRange.w = PointLightData["Range"][3].get<float>();

            }


            CLight* pLight = CLight::Create(Desc);
            CheckNullResult(pLight, E_FAIL);

            m_Lights[iLevelID].push_back(pLight);


        }
    }
    return S_OK;
}

void CLight_Manager::Clear_PointLightBuffer()
{
    m_PointLightNum = 0;
    ZeroMemory(&m_LightValues.g_vPL_Position, sizeof(_float4)*16);
    ZeroMemory(&m_LightValues.g_vPL_Range, sizeof(_float4)*16);
    ZeroMemory(&m_LightValues.g_vPL_Diffuse, sizeof(_float4)*16);
    ZeroMemory(&m_LightValues.g_vPL_Ambient, sizeof(_float4)*16);
    ZeroMemory(&m_LightValues.g_vPL_Position, sizeof(_float4)*16);

}

CLight_Manager* CLight_Manager::Create(_uint iLevelNum)
{
    CLight_Manager* pInstance = new CLight_Manager;
    if (FAILED(pInstance->Initialize(iLevelNum)))
    {
        MSG_BOX("Failed to Create : CTexture_Manager");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CLight_Manager::Free()
{
    __super::Free();

    for (int i = 0; i < m_DirectionalLights.size(); ++i)
        Safe_Release(m_DirectionalLights[i]);


    for (int i = 0; i < m_Lights.size(); ++i)
    {
        for (auto& pLight : m_Lights[i])
            Safe_Release(pLight);

        m_Lights[i].clear();
    }


}
