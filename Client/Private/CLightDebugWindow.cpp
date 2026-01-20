#include "CLightDebugWindow.h"
#include "CGameInstance.h"

USING(Client)
CLightDebugWindow::CLightDebugWindow(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    :CImgui_Window(pDevice,pContext)
{
}

HRESULT CLightDebugWindow::Initialize(void* pArg)
{
    m_pGameInstance = CGameInstance::GetInstance();

    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;





    return S_OK;
}

void CLightDebugWindow::Update()
{
    ImGui::Begin(m_WindowTitle.c_str(), &m_bOpen);

    

    Save();
    Add_Lights();


    ImGui::End();
}

void CLightDebugWindow::Render()
{
}

void CLightDebugWindow::Save()
{
    if(ImGui::Button("Save"))
        m_pGameInstance->Save_LightData();
}

void CLightDebugWindow::Load()
{
}

void CLightDebugWindow::Add_Lights()
{
    if (ImGui::Button("AddLight"))
    {
        LIGHT_DESC Desc;
        Desc.eType = LIGHT::POINT;
        Desc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
        Desc.vAmbient = _float4(1.f, 1.f, 1.f, 1.f);
        Desc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);

        Desc.LightName = L"Dummy";

        int iCurrentLevelID = m_pGameInstance->Get_CurrentLevelID();

        if (FAILED(m_pGameInstance->Add_Light(iCurrentLevelID, Desc)))
            return;

    }
       
}


CLightDebugWindow* CLightDebugWindow::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, void* pArg)
{
    CLightDebugWindow* pInstance = new CLightDebugWindow(pDevice, pContext);


    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Create : CLightDebugWindow");
        Safe_Release(pInstance);
    }


    return pInstance;
}

void CLightDebugWindow::Free()
{
    __super::Free();
}
