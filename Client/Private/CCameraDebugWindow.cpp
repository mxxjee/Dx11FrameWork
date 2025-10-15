#include "CCameraDebugWindow.h"
#include "CImgui_Button.h"
#include "CImgui_Slider.h"
#include "CGameInstance.h"
#include "CGameObject.h"
#include "COrthographicCameraComponent.h"
#include "CPerspectiveCameraComponent.h"
#include "CCamera_Base.h"




USING(Client)
CCameraDebugWindow::CCameraDebugWindow(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    :CImgui_Window(pDevice,pContext), m_pGameInstance(CGameInstance::GetInstance())
{
    Safe_AddRef(m_pGameInstance);

}

HRESULT CCameraDebugWindow::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Create_Widgets()))
        return E_FAIL;

   
    return S_OK;
}

void CCameraDebugWindow::Update()
{
    ImGui::Begin(m_WindowTitle.c_str(), &m_bOpen);

    ShowMainCameraDebug(m_bClickOrtho);

    for (auto& i : m_vWidgets)
        i->Update();

    

    ImGui::End();

    
}

void CCameraDebugWindow::Render()
{
}

HRESULT CCameraDebugWindow::Create_Widgets()
{
    CImgui_Button::IMGUIBUTTON_DESC PersDesc;
    PersDesc.Label = "PerspectiveCam";
    PersDesc.Tag = L"PerspectiveCam";
    PersDesc.m_RelativePos = ImVec2(10.f, 30.f);
    PersDesc.callback = [&]()
    {
        ToggleClickOrtho(false);
    };

    if (FAILED(Create_Button(&PersDesc, &m_pPerspectiveCamButton)))
        return E_FAIL;


    CImgui_Button::IMGUIBUTTON_DESC OrthoDesc;
    OrthoDesc.Label = "OrthoGraphicCam";
    OrthoDesc.Tag = L"OrthoGraphicCam";
    OrthoDesc.m_RelativePos = ImVec2(130.f, 30.f);
    OrthoDesc.callback = [&]()
    {
        ToggleClickOrtho(true);
    };

    if (FAILED(Create_Button(&OrthoDesc,&m_pOrthoGraphicCamButton)))
        return E_FAIL;

   
    ///Perspective 디버그용 슬라이더 생성
    m_Sliders.resize(3);

    //[NearClipPlane 슬라이더]
    CImgui_Slider::IMGUISLIDER_DESC SliderDesc;
    SliderDesc.m_LabelName = "Near Clip Plane";
    SliderDesc.m_RelativePos = ImVec2(0.f, 200.f);
    SliderDesc.vMin = 0.1f;
    SliderDesc.vMax = 999.f;


    if (FAILED(Create_Slider(&SliderDesc, &m_Sliders[0])))
        return E_FAIL;

    //[Plane 슬라이더]
    SliderDesc.m_LabelName = "Far Clip Plane";
    SliderDesc.m_RelativePos = ImVec2(0.f, 230.f);
    SliderDesc.vMin = 0.2f;
    SliderDesc.vMax = 1000.f;

    if (FAILED(Create_Slider(&SliderDesc, &m_Sliders[1])))
        return E_FAIL;

    //[NearClipPlane 슬라이더]
    SliderDesc.m_LabelName = "Fov Clip Plane";
    SliderDesc.m_RelativePos = ImVec2(0.f, 260.f);


    if (FAILED(Create_Slider(&SliderDesc, &m_Sliders[2])))
        return E_FAIL;


    ////////////////////////////////////////////////////
    //카메라 바꾸는 버튼 세팅
    m_CamButtons.resize(2);
    CImgui_Button::IMGUIBUTTON_DESC TargetCamDesc;
    TargetCamDesc.Label = "TargetCam";
    TargetCamDesc.m_RelativePos = ImVec2(0.f, 320.f);
    TargetCamDesc.Tag = L"TargetCamMode";
    TargetCamDesc.callback = [this]()
    {
        m_pGameInstance->Set_MainCamera(CAMERA_TYPE::TARGET);

    };
    if (FAILED(Create_Button(&TargetCamDesc, &m_CamButtons[0])))
        return E_FAIL;

    CImgui_Button::IMGUIBUTTON_DESC FreeCamDesc;
    FreeCamDesc.Label = "FreeCam";
    FreeCamDesc.m_RelativePos = ImVec2(150.f, 320.f);
    FreeCamDesc.Tag = L"FreeCam";
    FreeCamDesc.callback = [this]()
    {
        m_pGameInstance->Set_MainCamera(CAMERA_TYPE::FREE);

    };
    if (FAILED(Create_Button(&FreeCamDesc, &m_CamButtons[1])))
        return E_FAIL;


    return S_OK;
}

HRESULT CCameraDebugWindow::Create_Button(void* pArg, CImgui_Button** ppOut)
{
    *ppOut = CImgui_Button::Create(m_pDevice, m_pContext, pArg);
    m_vWidgets.push_back(*ppOut);

    return S_OK;
}

HRESULT CCameraDebugWindow::Create_Slider(void* pArg, CImgui_Slider** ppOut)
{
    *ppOut = CImgui_Slider::Create(m_pDevice, m_pContext, pArg);
    m_vWidgets.push_back(*ppOut);

    return S_OK;
}

void CCameraDebugWindow::ShowMainCameraDebug(bool isOrtho)
{


    ImGui::SetCursorPos(ImVec2(0.f, 60.f));
    ImGui::Separator();
    ImGui::SetCursorPos(ImVec2(0.f, 0.f));


    CCamera_Base* pMainCam = (m_bClickOrtho) ? m_pGameInstance->Find_Camera(CAMERA_TYPE::UI) : m_pGameInstance->Find_Camera(CAMERA_TYPE::MINIMAP);
    CCameraComponent* pCameracomp = nullptr;

    if (pMainCam == nullptr)
    {

        for (auto& i : m_CamButtons)
            i->Set_Active(false);
        ImGui::SetCursorPos(ImVec2(0.f, 80.f));
        ImGui::TextColored(ImVec4(255, 0, 0, 255), "MainCam  is Null!!");
        return;
        
    }

    CheckTrue(m_bClickOrtho == -1);

    for (auto& i : m_CamButtons)
        i->Set_Active(true);

    if (m_bClickOrtho)
    {
        pCameracomp = dynamic_cast<CCameraComponent*>(pMainCam->Get_Component(COMPONENT_TYPE::ORTHOGRAPHIC_CAM));

        ImGui::SetCursorPos(ImVec2(0.f, 70.f));
        wstring Name = pMainCam->Get_Tag();
        ImGui::TextColored(ImVec4(0, 255, 0, 255), "Name : %s", WStringToUTF8(Name).c_str());

        ImGui::SetCursorPos(ImVec2(0.f, 85.f));
        ImGui::TextColored(ImVec4(0, 255, 0, 255), "Projection : Orthographic");
    }

    else
    {
        pCameracomp = dynamic_cast<CCameraComponent*>(pMainCam->Get_Component(COMPONENT_TYPE::ORTHOGRAPHIC_CAM));

        ImGui::SetCursorPos(ImVec2(0.f, 70.f));
        wstring Name = pMainCam->Get_Tag();
        ImGui::TextColored(ImVec4(0, 255, 0, 255), "Name : %s", WStringToUTF8(Name).c_str());

        ImGui::SetCursorPos(ImVec2(0.f, 85.f));
        ImGui::TextColored(ImVec4(0, 255, 0, 255), "Projection : Minimap");

    }


    ImGui::SetCursorPos(ImVec2(0.f, 110.f));
    ImGui::Separator();

 
    CheckNull(pCameracomp);
    _float4 vAt, vEye;
    class CGameObject* Target = pCameracomp->Get_Target();

  
    XMStoreFloat4(&vEye, pCameracomp->Get_Eye());
    XMStoreFloat4(&vAt, pCameracomp->Get_At());

    ImGui::SetCursorPos(ImVec2(0.f, 130.f));

    ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 0, 0, 255));
    ImGui::BulletText("Eye X:%f, Y:%f, Z:%f", vEye.x, vEye.y, vEye.z);
    ImGui::PopStyleColor();


    ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 0, 255, 255));
    ImGui::BulletText("At X:%f, Y:%f, Z:%f", vAt.x, vAt.y, vAt.z);
    ImGui::PopStyleColor();

    CheckNull(Target);
    ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 255, 0, 255));
    ImGui::BulletText("Target : %s", WStringToUTF8(Target->Get_Tag()).c_str());
    ImGui::PopStyleColor();

    ImGui::SetCursorPos(ImVec2(0.f, 190.f));
    ImGui::Separator();

    ///카메라 바꾸는 버튼 구별 선
    ImGui::SetCursorPos(ImVec2(0.f, 300.f));
    ImGui::Separator();
}

void CCameraDebugWindow::ToggleClickOrtho(bool _b)
{
    m_bClickOrtho = _b;

    //CGameObject* pMainCam = (m_bClickOrtho) ? m_pGameInstance->Find_Camera(CAMERA_TYPE::UI) : m_pGameInstance->Get_MainCamera();
    //CheckNull(pMainCam);

    ////orthographic 선택했을떄  바인딩정보
    //if (_b)
    //{

    //    COrthographicCameraComponent* pOrthoComp = dynamic_cast<COrthographicCameraComponent*>(pMainCam->Get_Component(COMPONENT_TYPE::ORTHOGRAPHIC_CAM));

    //    if (pOrthoComp)
    //    {
    //        fDebugNear = pOrthoComp->Get_Near();
    //        fDebugFar = pOrthoComp->Get_Far();

    //        m_Sliders[NearClip]->Set_BindValue(&fDebugNear);
    //        m_Sliders[NearClip]->Set_Min(0.1f);
    //        m_Sliders[NearClip]->Set_Max(999.f);

    //        m_Sliders[FarClip]->Set_BindValue(&fDebugFar);
    //        m_Sliders[FarClip]->Set_Min(0.2f);
    //        m_Sliders[FarClip]->Set_Max(1000.f);

    //        m_Sliders[FOV]->Set_Active(false);

    //        for (int i = 0; i < 3; ++i)
    //        {
    //            m_Sliders[i]->Set_Callback([pOrthoComp, this]() {
    //                float fNear = m_Sliders[NearClip]->Get_BindValue();
    //                float fFar = m_Sliders[FarClip]->Get_BindValue();
    //                pOrthoComp->Set_CameraValue(fNear, fFar);
    //                });
    //        }





    //    }

    //}

    //else
    //{
    //  
    //    CPerspectiveCameraComponent* pPersComp = dynamic_cast<CPerspectiveCameraComponent*>(pMainCam->Get_Component(COMPONENT_TYPE::PERSPECTIVE_CACM));

    //    if (pPersComp)
    //    {
    //        fDebugNear = pPersComp->Get_Near();
    //        fDebugFar = pPersComp->Get_Far();
    //        fDebugFov = pPersComp->Get_Fov();
    //        fDebugFov = XMConvertToDegrees(fDebugFov);

    //        m_Sliders[NearClip]->Set_BindValue(&fDebugNear);
    //        m_Sliders[NearClip]->Set_Min(0.1f);
    //        m_Sliders[NearClip]->Set_Max(999.f);

    //        m_Sliders[FarClip]->Set_BindValue(&fDebugFar);
    //        m_Sliders[FarClip]->Set_Min(0.2f);
    //        m_Sliders[FarClip]->Set_Max(1000.f);

    //        m_Sliders[FOV]->Set_Active(true);


    //        m_Sliders[FOV]->Set_BindValue(&fDebugFov);
    //        m_Sliders[FOV]->Set_Min(0.f);
    //        m_Sliders[FOV]->Set_Max(1000.f);

    //        for (int i = 0; i < 3; ++i)
    //        {
    //            m_Sliders[i]->Set_Callback([pPersComp, this]() {
    //                float fNear = m_Sliders[NearClip]->Get_BindValue();
    //                float fFar = m_Sliders[FarClip]->Get_BindValue();
    //                float fFov = m_Sliders[FOV]->Get_BindValue();

    //                fFov = XMConvertToRadians(fFov);
    //                pPersComp->Set_CameraValue(fNear, fFar, fFov);
    //                });
    //        }





    //    }
    //}

}

CCameraDebugWindow* CCameraDebugWindow::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, void* pArg)
{
    CCameraDebugWindow* pInstance = new CCameraDebugWindow(pDevice, pContext);


    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Create : CCameraDebugWindow");
        Safe_Release(pInstance);
    }


    return pInstance;
}

void CCameraDebugWindow::Free()
{
    __super::Free();
    Safe_Release(m_pGameInstance);

}
