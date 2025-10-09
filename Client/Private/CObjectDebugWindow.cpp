#include "CObjectDebugWindow.h"
#include "CGameObject.h"
#include "CTransform.h"
#include "MathUtils.h"
#include "ColorUtils.h"

USING(Client)
CObjectDebugWindow::CObjectDebugWindow(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    :CImgui_Window(pDevice,pContext)
{
}

HRESULT CObjectDebugWindow::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    return S_OK;
}

void CObjectDebugWindow::Update()
{
    ImGui::Begin(m_WindowTitle.c_str(), &m_bOpen);

    if (pSelectObject)
    {
        ImGui::Text("Name : %s", WStringToUTF8(pSelectObject->Get_Tag()).c_str());


        CTransform* pTransform = dynamic_cast<CTransform*>(pSelectObject->Get_Component(L"Transform"));
        if (pTransform)
        {
            ImGui::Separator();

            _float4 s, t, r;

            XMStoreFloat4(&s, pTransform->Get_SRT(SRTType::SCALE));
            XMStoreFloat4(&t, pTransform->Get_SRT(SRTType::TRANSFORM));
            XMStoreFloat4(&r, pTransform->Get_SRT(SRTType::ROTATION));


          
            _float3 rResult = MathUtils::QuaternionToEuler(XMLoadFloat4(&r));
            
            ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 0, 0, 255));
            ImGui::BulletText("Position X:%f, Y:%f, Z:%f", t.x, t.y, t.z);
            ImGui::PopStyleColor();

            ImGui::Separator();
            ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 255, 0, 255));
            ImGui::BulletText("Scale X:%f, Y:%f, Z:%f", s.x, s.y, s.z);
            ImGui::PopStyleColor();

            ImGui::Separator();
            ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 0, 255, 255));
            ImGui::BulletText("Rotation X:%f, Y:%f, Z:%f", 
                rResult.x,
                rResult.y,
                rResult.z);
            ImGui::PopStyleColor();
        }
    }
        

   
    

    ImGui::End();

}

void CObjectDebugWindow::Render()
{
}

CObjectDebugWindow* CObjectDebugWindow::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, void* pArg)
{
    CObjectDebugWindow* pInstance = new CObjectDebugWindow(pDevice, pContext);


    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Create : ObjectDebugWindow");
        Safe_Release(pInstance);
    }


    return pInstance;
}

void CObjectDebugWindow::Free()
{
    __super::Free();

}
