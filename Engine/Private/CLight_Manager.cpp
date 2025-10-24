#include "CLight_Manager.h"
#include "CLight.h"
#include "CShader.h"
#include "CGameInstance.h"



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

HRESULT CLight_Manager::Bind_Lights(CShader* pShader)
{  

    size_t LightSize = m_Lights.size();
    _uint  LevelID = CGameInstance::GetInstance()->Get_CurrentLevelID();

    //전역조명 바인딩.
    const LIGHT_DESC* pDirectional_LightDesc = m_DirectionalLights[LevelID]->Get_LightDesc();
    Bind_Directional_Light(pShader, pDirectional_LightDesc);

    //점 조명들 바인딩.
    //현재 씬에있는 조명데이터 싹가져와
    //활성화 되어있는 애들만 셰이더에 보내자.
    
    m_LightValues.clear();

    for (auto& pLight : m_Lights[LevelID])
    {
        
        const LIGHT_DESC* desc = pLight->Get_LightDesc();
        if (!pLight || !pLight->IsActive())
            continue;

        
        Add_LightValue(desc);
        m_LightValues.m_LightsNum += 1;

    }

    Bind_Point_Light(pShader);
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
    m_LightValues.m_LightPositions.push_back(LightDesc->vPosition);
    m_LightValues.m_LightRanges.push_back(LightDesc->fRange);

    m_LightValues.m_LightDiffuses.push_back(LightDesc->vDiffuse);
    m_LightValues.m_LightAmbients.push_back(LightDesc->vAmbient);
    m_LightValues.m_LightSpeculars.push_back(LightDesc->vSpecular);
 
}

HRESULT CLight_Manager::Bind_Directional_Light(class CShader* pShader,const LIGHT_DESC* pLightDesc)
{

    //전역 조명 
    if (FAILED(pShader->Bind_RawValue("g_vLightDirection", &pLightDesc->vDirection, sizeof(_float4))))
        return E_FAIL;

    if (FAILED(pShader->Bind_RawValue("g_vLightDiffuse", &pLightDesc->vDiffuse, sizeof(_float4))))
        return E_FAIL;

    if (FAILED(pShader->Bind_RawValue("g_vLightAmbient", &pLightDesc->vAmbient, sizeof(_float4))))
        return E_FAIL;

    if (FAILED(pShader->Bind_RawValue("g_vLightSpecular", &pLightDesc->vSpecular, sizeof(_float4))))
        return E_FAIL;

    return S_OK;
}

HRESULT CLight_Manager::Bind_Point_Light(class CShader* pShader)
{

    //포인트 라이트 조명 
    if (FAILED(pShader->Bind_RawValue("g_PointLightNum", &m_LightValues.m_LightsNum, sizeof(int))))
        return E_FAIL;

    if (FAILED(pShader->Bind_RawValue("g_vPL_Position", &m_LightValues.m_LightPositions.front(), sizeof(_float4)*(_uint)m_LightValues.m_LightPositions.size())))
        return E_FAIL;

    if (FAILED(pShader->Bind_RawValue("g_vPL_Diffuse", &m_LightValues.m_LightDiffuses.front(), sizeof(_float4) * (_uint)m_LightValues.m_LightDiffuses.size())))
        return E_FAIL;

    if (FAILED(pShader->Bind_RawValue("g_vPL_Ambient", &m_LightValues.m_LightAmbients.front(), sizeof(_float4) * (_uint)m_LightValues.m_LightAmbients.size())))
        return E_FAIL;

    if (FAILED(pShader->Bind_RawValue("g_vPL_Specular", &m_LightValues.m_LightSpeculars.front(), sizeof(_float4) * (_uint)m_LightValues.m_LightSpeculars.size())))
        return E_FAIL;

    return S_OK;
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
