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
    
  /*  Clear_PointLightBuffer();

    for (auto& pLight : m_Lights[m_iLevelID])
    {
        if (!pLight || !pLight->IsActive())
            continue;

        const LIGHT_DESC* desc = pLight->Get_LightDesc();
    

        
        Add_LightValue(desc);
        m_PointLightNum += 1;

    }

     Bind_Point_Light(pShader);*/

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

    //m_DirectionLightBuffer.g_vLightDirection = pLightDesc->vDirection;
    //m_DirectionLightBuffer.g_vLightDiffuse = pLightDesc->vDiffuse;
    //m_DirectionLightBuffer.g_vLightAmbient = pLightDesc->vAmbient;
    //m_DirectionLightBuffer.g_vLightSpecular = pLightDesc->vSpecular;
  
    //pShader->Bind_RawValue("g_PointLightNum", &m_PointLightNum, sizeof(int));

    //CGameInstance::GetInstance()->CopyData_Buffer("DirectionLightBuffer", &m_DirectionLightBuffer, sizeof(m_DirectionLightBuffer));

    pShader->Begin("DirectionalLight");
    pVIBuffer->Bind_Resource();
    pVIBuffer->Render();

    return S_OK;
}

HRESULT CLight_Manager::Bind_Point_Light(CShader* pShader)
{
   
    CGameInstance::GetInstance()->CopyData_Buffer("PointLightBuffer", &m_LightValues, sizeof(m_LightValues));


    return S_OK;
}

void CLight_Manager::Render(CShader* pShader, CVIBuffer_Rect* pVIBuffer)
{
    m_iLevelID = CGameInstance::GetInstance()->Get_CurrentLevelID();
    //CheckNull(m_DirectionalLights[m_iLevelID]);

    Bind_Directional_Light(pShader, pVIBuffer, m_DirectionalLights[m_iLevelID]->Get_LightDesc());
   

    ////점조명 렌더..
    //for(auto& pLight: m_Lights[m_iLevelID])

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
