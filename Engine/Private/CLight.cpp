#include "CLight.h"

CLight::CLight()
{
}

HRESULT CLight::Initialize(const LIGHT_DESC& LightDesc)
{
    m_LightDesc = LightDesc;

    return S_OK;
}

void CLight::Render(CShader* pShader, CVIBuffer_Rect* pVIBuffer)
{
    
}

void CLight::Render_DebugImgui()
{

    const char* items[] = { "DIRECTION", "POINT" };
    int current_item = (int)m_LightDesc.eType;

    if (ImGui::Combo("Light Type", &current_item, items, IM_ARRAYSIZE(items)))
    {
        LIGHT eNewType = (LIGHT)current_item;
        m_LightDesc.eType = eNewType;
       
    }

    //이름
    char szBuff[128] = "";
    if (ImGui::InputText("Name", szBuff, 128))
    {
        m_LightDesc.LightName = StringToWString(szBuff);

    }

    //디퓨즈값
    ImGui::ColorEdit4("Diffuse", (float*)&m_LightDesc.vDiffuse, ImGuiColorEditFlags_Float);
    ImGui::ColorEdit4("Ambient", (float*)&m_LightDesc.vAmbient, ImGuiColorEditFlags_Float);
    ImGui::ColorEdit4("Specular", (float*)&m_LightDesc.vSpecular, ImGuiColorEditFlags_Float);


    if (m_LightDesc.eType == LIGHT::POINT)
    {
        // 위치
        ImGui::DragFloat4("Position", (float*)&m_LightDesc.vPosition, 0.1f);

        // 범위 (fRange는 XMFLOAT4지만 x값)
        // vRange의 .x값만 제어
        ImGui::DragFloat("Range", &m_LightDesc.fRange.x, 0.1f, 0.0f, 1000.0f);
    }

    else
    {
        ImGui::DragFloat4("Direction", (float*)&m_LightDesc.vDirection, 0.1f);

    } 
}

CLight* CLight::Create(const LIGHT_DESC& LightDesc)
{
    CLight* pInstance = new CLight();
    if (FAILED(pInstance->Initialize(LightDesc)))
    {
        MSG_BOX("Failed to Created : CLight");
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CLight::Free()
{
}
