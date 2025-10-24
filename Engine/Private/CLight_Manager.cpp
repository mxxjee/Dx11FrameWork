#include "CLight_Manager.h"
#include "CLight.h"
#include "CShader.h"


CLight_Manager::CLight_Manager()
{
}

const LIGHT_DESC* CLight_Manager::Get_LightDesc(_uint iIndex)
{
    auto iter = m_Lights.begin();
    for (_uint i = 0; i < iIndex; ++i)
        ++iter;


    return (*iter)->Get_LightDesc();

}

HRESULT CLight_Manager::Bind_Lights(CShader* pShader)
{  


    for (auto& i : m_Lights)
    {   
        const LIGHT_DESC* pLightDesc = i->Get_LightDesc();

        if(FAILED(pShader->Bind_RawValue("g_vLightDirection", &pLightDesc->vDirection,sizeof(_float4))))
            return E_FAIL;

        if (FAILED(pShader->Bind_RawValue("g_vLightDiffuse", &pLightDesc->vDiffuse, sizeof(_float4))))
            return E_FAIL;

        if (FAILED(pShader->Bind_RawValue("g_vLightAmbient", &pLightDesc->vAmbient, sizeof(_float4))))
            return E_FAIL;

        if (FAILED(pShader->Bind_RawValue("g_vLightSpecular", &pLightDesc->vSpecular, sizeof(_float4))))
            return E_FAIL;


    }

    return S_OK;
}



HRESULT CLight_Manager::Add_Light(const LIGHT_DESC& LightDesc)
{
    CLight* pLight = CLight::Create(LightDesc);
    CheckNullResult(pLight, E_FAIL);

    m_Lights.push_back(pLight);
    return S_OK;
}

CLight_Manager* CLight_Manager::Create()
{
    return new CLight_Manager();
}

void CLight_Manager::Free()
{
    __super::Free();

    for (auto& i : m_Lights)
        Safe_Release(i);

    m_Lights.clear();
}
