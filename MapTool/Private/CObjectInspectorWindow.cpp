#include "CObjectInspectorWindow.h"
#include "CGameInstance.h"
#include "CMapObject.h"
#include "CImgui_Widget.h"
#include "CImgui_InputInt.h"
#include "CImgui_InputFloat.h"


USING(MapTool)
CObjectInspectorWindow::CObjectInspectorWindow(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    :CImgui_Window(pDevice,pContext),
    pGameInstance(CGameInstance::GetInstance())
{
    Safe_AddRef(pGameInstance);
}

HRESULT CObjectInspectorWindow::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Create_Widgets()))
        return E_FAIL;

    return S_OK;
}

void CObjectInspectorWindow::Update()
{
    ImGui::Begin(m_WindowTitle.c_str(), &m_bOpen);
    pSelectObject = pGameInstance->Get_SelectObject();
    if (pSelectObject)
    {
        ImGui::TextColored(ImVec4(1.f,0.f,0.f,1.f),"Name : %s", WStringToUTF8(pSelectObject->Get_Tag()).c_str());
        vScale.x = pSelectObject->Get_Transform()->Get_Scale_ByFloat3().x;
        vScale.y = pSelectObject->Get_Transform()->Get_Scale_ByFloat3().y;
        vScale.z = pSelectObject->Get_Transform()->Get_Scale_ByFloat3().z;

        for (int i = 0; i < 3; ++i)
        {
            if(ScaleInput[i])
                ScaleInput[i]->Set_Active(true);
        }
           
    }

    else
    {
        for (int i = 0; i < 3; ++i)
        {
            if (ScaleInput[i])
                ScaleInput[i]->Set_Active(false);
        }
    }
    ImGui::End();

    __super::Update();
}

void CObjectInspectorWindow::Render()
{
}

HRESULT CObjectInspectorWindow::Create_Widgets()
{
    CImgui_InputFloat::IMGUITEXTFLOAT_DESC InputIntDesc;
    InputIntDesc.Label = "ScaleX";
    InputIntDesc.Tag = InputIntDesc.Label;
    InputIntDesc.m_RelativePos = ImVec2(0,80);
    InputIntDesc.pData = &vScale.x;


    if (FAILED(Add_Widgets<CImgui_InputFloat>(&InputIntDesc, reinterpret_cast<CImgui_Widget**>(&ScaleInput[0]))))
        return E_FAIL;


    InputIntDesc.Label = "ScaleY";
    InputIntDesc.Tag = InputIntDesc.Label;
    InputIntDesc.m_RelativePos = ImVec2(0, 100);
    InputIntDesc.pData = &vScale.y;
    if (FAILED(Add_Widgets<CImgui_InputFloat>(&InputIntDesc, reinterpret_cast<CImgui_Widget**>(&ScaleInput[1]))))
        return E_FAIL;

    InputIntDesc.Label = "ScaleZ";
    InputIntDesc.Tag = InputIntDesc.Label;
    InputIntDesc.m_RelativePos = ImVec2(0, 120);
    InputIntDesc.pData = &vScale.z;
    if (FAILED(Add_Widgets<CImgui_InputFloat>(&InputIntDesc, reinterpret_cast<CImgui_Widget**>(&ScaleInput[2]))))
        return E_FAIL;

    for (int i = 0; i < 3; ++i)
    {
        if (ScaleInput[i])
        {
            ScaleInput[i]->Set_Callback([this]()
                {
                    if (pSelectObject)
                        pSelectObject->Get_Transform()->Set_Scale(_float4(vScale.x, vScale.y, vScale.z, 1.f));

                });
            ScaleInput[i]->Set_Active(false);
        }
           
    }
       
    return S_OK;
}

CObjectInspectorWindow* CObjectInspectorWindow::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, void* pArg)
{
    CObjectInspectorWindow* pInstance = new CObjectInspectorWindow(pDevice, pContext);


    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Create : CObjectInspectorWindow");
        Safe_Release(pInstance);
    }


    return pInstance;
}

void CObjectInspectorWindow::Free()
{
    __super::Free();
    for (int i = 0; i < 3; ++i)
        Safe_Release(ScaleInput[i]);

    Safe_Release(pGameInstance);
}
