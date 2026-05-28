#include "CCameraDebugWindow.h"
#include "CImgui_Button.h"
#include "CImgui_Slider.h"
#include "CGameInstance.h"
#include "CGameObject.h"
#include "COrthographicCameraComponent.h"
#include "CPerspectiveCameraComponent.h"
#include "CCamera_Base.h"
#include "Client_Defines.h"
#include "CImgui_Checkbox.h"
#include "CImgui_InputFloat.h"
#include "CMainCamera.h"




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


    if (pTargetCamera)
    {

		fDebugNear = pTargetCamera->Get_Near();
		fDebugFar = pTargetCamera->Get_Far();
		fDebugFov = pTargetCamera->Get_Fovy();
		fDebugOffSet = pTargetCamera->Get_Offset();


    }

    else
    {

        pTargetCamera = m_pGameInstance->Find_Camera(CAMERA_TYPE::TARGET);
        if (pTargetCamera)
        {
            fDefaultOffSet = pTargetCamera->Get_Offset();
            m_pDefaultOffSetButton->Set_Callback([this]() {
                pTargetCamera->Set_Offset(fDefaultOffSet);
                });

        }
     
    }

   
    

    ImGui::Begin(m_WindowTitle.c_str(), &m_bOpen);

    
    for (auto& i : m_vWidgets)
        i->Update();

    
    ShowMainCameraDebug(m_bClickOrtho);

    ImGui::End();

    
}

void CCameraDebugWindow::Render()
{
}

HRESULT CCameraDebugWindow::Create_Widgets()
{
    /*원근 투영 카메라 버튼*/
    CImgui_Button::IMGUIBUTTON_DESC PersDesc;
    PersDesc.Label = "PerspectiveCam";
    PersDesc.Tag = "PerspectiveCam";
    PersDesc.m_RelativePos = ImVec2(10.f, 30.f);
    PersDesc.callback = [&]()
    {
        ToggleClickOrtho(false);
    };

    if (FAILED(Add_Widgets<CImgui_Button>(&PersDesc, reinterpret_cast<CImgui_Widget**>(&m_pPerspectiveCamButton))))
        return E_FAIL;

    /*직교 투영 카메라 버튼*/
    CImgui_Button::IMGUIBUTTON_DESC OrthoDesc;
    OrthoDesc.Label = "OrthoGraphicCam";
    OrthoDesc.Tag = "OrthoGraphicCam";
    OrthoDesc.m_RelativePos = ImVec2(130.f, 30.f);
    OrthoDesc.callback = [&]()
    {
        ToggleClickOrtho(true);
    };

    if (FAILED(Add_Widgets<CImgui_Button>(&OrthoDesc, reinterpret_cast<CImgui_Widget**>(&m_pOrthoGraphicCamButton))))
        return E_FAIL;

   
    ///Perspective 디버그용 슬라이더 생성
    m_Sliders.resize(END);
    m_InuptFloats.resize(3);

    //[NearClipPlane 슬라이더]
    CImgui_Slider::IMGUISLIDER_DESC SliderDesc;
    SliderDesc.m_LabelName = "Near Clip Plane";
    SliderDesc.m_RelativePos = ImVec2(0.f, 210.f);
    SliderDesc.vMin = 0.1f;
    SliderDesc.vMax = 999.f;


    if (FAILED(Add_Widgets<CImgui_Slider>(&SliderDesc, reinterpret_cast<CImgui_Widget**>(&m_Sliders[0]))))
        return E_FAIL;

    //[Plane 슬라이더]
    SliderDesc.m_LabelName = "Far Clip Plane";
    SliderDesc.m_RelativePos = ImVec2(0.f, 240.f);
    SliderDesc.vMin = 0.2f;
    SliderDesc.vMax = 1000.f;
    if (FAILED(Add_Widgets<CImgui_Slider>(&SliderDesc, reinterpret_cast<CImgui_Widget**>(&m_Sliders[1]))))
        return E_FAIL;

    //[Fov 슬라이더]
    SliderDesc.m_LabelName = "Fov Clip Plane";
    SliderDesc.m_RelativePos = ImVec2(0.f, 270.f);
    if(FAILED(Add_Widgets<CImgui_Slider>(&SliderDesc, reinterpret_cast<CImgui_Widget**>(&m_Sliders[2]))))
        return E_FAIL;

    //RotationSlider
    SliderDesc.m_LabelName = "Rotation";
    SliderDesc.m_RelativePos = ImVec2(0.f, 300.f);
    SliderDesc.vMin = 0.2f;
    SliderDesc.vMax = 1000.f;
    if (FAILED(Add_Widgets<CImgui_Slider>(&SliderDesc, reinterpret_cast<CImgui_Widget**>(&m_CulRotationSlider))))
        return E_FAIL;

    //////////////////////////////////////////////////////
    // 
    //[CImgui_InputFloat]
    const char* pLabelNames[3] = { "OffSet_X","OffSet_Y","OffSet_Z" };
    for (int i = OFFSET_X; i <= OFFSET_Z; ++i)
    {
        CImgui_InputFloat::ImguiTextFloatDesc   InuptDesc;

        InuptDesc.Label = pLabelNames[i - OFFSET_X];
        InuptDesc.m_RelativePos = ImVec2(0.f, 330.f + ((i-OFFSET_X)*20));
        InuptDesc.pData = nullptr;

        if (FAILED(Add_Widgets<CImgui_InputFloat>(&InuptDesc, reinterpret_cast<CImgui_Widget**>(&m_InuptFloats[i - OFFSET_X]))))
            return E_FAIL;

    }
    ////[OffSetDefault Button]
    CImgui_Button::IMGUIBUTTON_DESC DefaultOffSetDesc;
    DefaultOffSetDesc.Label = "OffSetDefaultButton";
    DefaultOffSetDesc.Tag = "OffSetDefaultButton";
    DefaultOffSetDesc.m_RelativePos = ImVec2(230.f, 330.f + ((END-OFFSET_X) * 20));
    DefaultOffSetDesc.callback = nullptr;

    if (FAILED(Add_Widgets<CImgui_Button>(&DefaultOffSetDesc, reinterpret_cast<CImgui_Widget**>(&m_pDefaultOffSetButton))))
        return E_FAIL;
    m_pDefaultOffSetButton->Set_Active(false);

    /////////////////////////////////////////////////////

    //렌더그룹체크박스
    m_CheckBoxs.resize(ENUM_TO_UINT(RENDERGROUP::END));
    const char* pElemets[ENUM_TO_UINT(RENDERGROUP::END)] = {
        "PRIORITY","SHADOW","NONALPHA","LIGHT","COMBINED","NONLIGHT","ALPHA","WORLD_UI_MINIMAP","WORLD_UI_MINIMAP", "UI"
    };

    //Camera Culling Dist슬라이더
    CImgui_Slider::IMGUISLIDER_DESC CulSliderDesc;
    CulSliderDesc.m_LabelName = "Culling Dist";
    CulSliderDesc.m_RelativePos = ImVec2(230.f, 400.f);
    CulSliderDesc.vMin = 0.1f;
    CulSliderDesc.vMax = 999.f;
    if (FAILED(Add_Widgets<CImgui_Slider>(&CulSliderDesc, reinterpret_cast<CImgui_Widget**>(&m_CulDistSlider))))
        return E_FAIL;


    for (int i = 0; i < ENUM_TO_UINT(RENDERGROUP::END); ++i)
    {
        CImgui_Checkbox::IMGUICHECKBOX_DESC CheckboxDesc;
        CheckboxDesc.Tag = pElemets[i];
        CheckboxDesc.Label = pElemets[i];

        CheckboxDesc.Label = pElemets[i];
        CheckboxDesc.m_RelativePos = ImVec2((150.f) * (i%3), 500.f + (i / 3) * 30.f);

        CheckboxDesc.bValueGetter = [i,this]()
        {
           
            if (m_pSelectCamera)
            {
                
                return reinterpret_cast<bool*>(m_pSelectCamera->Get_RenderMaskValue(i));
            }

            static bool dummy = false; // 안전한 기본값
            return &dummy;
            
        };

        
        if (FAILED(Add_Widgets<CImgui_Checkbox>(&CheckboxDesc, reinterpret_cast<CImgui_Widget**>(&m_CheckBoxs[i]))))
            return E_FAIL;
        m_CheckBoxs[i]->Set_Active(false);

        
    }

        
    
    ////////////////////////////////////////////////////
    //카메라 바꾸는 버튼 세팅
    m_CamButtons.resize(2);
    CImgui_Button::IMGUIBUTTON_DESC TargetCamDesc;
    TargetCamDesc.Label = "TargetCam";
    TargetCamDesc.m_RelativePos = ImVec2(0.f, 620.f);
    TargetCamDesc.Tag = "TargetCamMode";
    TargetCamDesc.callback = [this]()
    {
        m_pGameInstance->Find_Camera(CAMERA_TYPE::TARGET)->Set_Active(true);
        m_pGameInstance->Find_Camera(CAMERA_TYPE::FREE)->Set_Active(false);

    };
    if (FAILED(Add_Widgets<CImgui_Button>(&TargetCamDesc, reinterpret_cast<CImgui_Widget**>(&m_CamButtons[0]))))
        return E_FAIL;

    CImgui_Button::IMGUIBUTTON_DESC FreeCamDesc;
    FreeCamDesc.Label = "FreeCam";
    FreeCamDesc.m_RelativePos = ImVec2(150.f, 620.f);
    FreeCamDesc.Tag = "FreeCam";
    FreeCamDesc.callback = [this]()
    {
        m_pGameInstance->Find_Camera(CAMERA_TYPE::TARGET)->Set_Active(false);
        m_pGameInstance->Find_Camera(CAMERA_TYPE::FREE)->Set_Active(true);

    };
    if (FAILED(Add_Widgets<CImgui_Button>(&FreeCamDesc, reinterpret_cast<CImgui_Widget**>(&m_CamButtons[1]))))
        return E_FAIL;

    return S_OK;
}


void CCameraDebugWindow::ShowMainCameraDebug(bool isOrtho)
{

    
    ImGui::SetCursorPos(ImVec2(0.f, 60.f));
    ImGui::Separator();
    ImGui::SetCursorPos(ImVec2(0.f, 0.f));
    

    m_pSelectCamera = (m_bClickOrtho) ? m_pGameInstance->Find_Camera(CAMERA_TYPE::MINIMAP) : m_pGameInstance->Find_Camera(CAMERA_TYPE::TARGET);

	if (m_pSelectCamera == nullptr)
	{

		for (auto& i : m_CamButtons)
			i->Set_Active(false);

		ImGui::TextColored(ImVec4(255, 0, 0, 255), "MainCam  is Null!!");
		return;

	}

	CheckTrue(m_bClickOrtho == -1);

	for (auto& i : m_CamButtons)
		i->Set_Active(true);


    for (auto& i : m_CheckBoxs)
        i->Set_Active(true);
    
    if (m_bClickOrtho)
       {
         ImGui::SetCursorPos(ImVec2(0.f, 70.f));
         wstring Name = m_pSelectCamera->Get_Tag();
         ImGui::TextColored(ImVec4(0, 255, 0, 255), "Name : %s", WStringToUTF8(Name).c_str());
    
         ImGui::SetCursorPos(ImVec2(0.f, 85.f));
         ImGui::TextColored(ImVec4(0, 255, 0, 255), "Projection : Orthographic");
        }
    
    else
        {
            ImGui::SetCursorPos(ImVec2(0.f, 70.f));
            wstring Name = m_pSelectCamera->Get_Tag();
            ImGui::TextColored(ImVec4(0, 255, 0, 255), "Name : %s", WStringToUTF8(Name).c_str());
    
            ImGui::SetCursorPos(ImVec2(0.f, 85.f));
            ImGui::TextColored(ImVec4(0, 255, 0, 255), "Projection : Target");
    
        }
    
    
      ImGui::SetCursorPos(ImVec2(0.f, 110.f));
      ImGui::Separator();
      ImGui::SetCursorPos(ImVec2(0.f, 0.f));


      
      _float4 vEye = m_pGameInstance->Get_CamPosition(ENUM_TO_UINT(m_pSelectCamera->Get_CameraType()));
      _float4 vAt;

      XMStoreFloat4(&vAt, XMVector4Normalize(m_pSelectCamera->Get_Transform()->Get_State(STATE::LOOK)));
     
      class CGameObject* Target = m_pSelectCamera->Get_Target();
   
      
   
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
    
    /* OffSet Debug*/
     ImGui::SetCursorPos(ImVec2(0.f, 310.f));
      ImGui::Separator();
    
      //RenderGroup
      ImGui::SetCursorPos(ImVec2(0.f, 420.f));
      ImGui::Separator();
    
      //카메라 바꾸는 버튼 구별 선
      ImGui::SetCursorPos(ImVec2(0.f, 600.f));
      ImGui::Separator();
      ImGui::SetCursorPos(ImVec2(0.f, 0.f));
}

void CCameraDebugWindow::ToggleOffSetDebug()
{
    /*토글설정*/
    if (m_bClickOrtho)
    {
        m_InuptFloats[0]->Set_Active(false);
        m_InuptFloats[1]->Set_Active(false);
        m_InuptFloats[2]->Set_Active(false);
        m_CulDistSlider->Set_Active(false);

        m_pDefaultOffSetButton->Set_Active(false);

    }

    else
    { 
        m_InuptFloats[0]->Set_Active(true);
        m_InuptFloats[1]->Set_Active(true);
        m_InuptFloats[2]->Set_Active(true);

        m_pDefaultOffSetButton->Set_Active(true);


    }
}  

void CCameraDebugWindow::ToggleClickOrtho(bool _b)
{
    m_bClickOrtho = _b;

    ToggleOffSetDebug();

    CGameObject* pMainCam = (m_bClickOrtho) ? m_pGameInstance->Find_Camera(CAMERA_TYPE::UI) : m_pGameInstance->Get_MainCamera();
    CheckNull(pMainCam);

    //orthographic 선택했을떄  바인딩정보
    if (_b)
    {

        CCamera_Base* pMinimap = m_pGameInstance->Find_Camera(CAMERA_TYPE::MINIMAP);

        if (pMinimap)
        {
            fDebugNear = pMinimap->Get_Near();
            fDebugFar = pMinimap->Get_Far();

            m_Sliders[NearClip]->Set_BindValue(&fDebugNear);
            m_Sliders[NearClip]->Set_Min(0.1f);
            m_Sliders[NearClip]->Set_Max(999.f);

            m_Sliders[FarClip]->Set_BindValue(&fDebugFar);
            m_Sliders[FarClip]->Set_Min(0.2f);
            m_Sliders[FarClip]->Set_Max(1000.f);

            m_Sliders[FOV]->Set_Active(false);

            for (int i = 0; i < 3; ++i)
            {
                m_Sliders[i]->Set_Callback([pMinimap, this]() {
                    float fNear = m_Sliders[NearClip]->Get_BindValue();
                    float fFar = m_Sliders[FarClip]->Get_BindValue();
                    pMinimap->Set_Near(fNear);
                    pMinimap->Set_Far(fFar);
                    });
            }





        }

    }

    else
    {
        CCamera_Base* pTarget = m_pGameInstance->Find_Camera(CAMERA_TYPE::TARGET);
        if (pTarget)
        {
            fDebugNear = pTarget->Get_Near();
            fDebugFar = pTarget->Get_Far();
            fDebugFov = pTarget->Get_Fovy();
            fDebugOffSet = pTarget->Get_Offset();
            fCulRotationX = pTarget->Get_Transform()->Get_Rotation_ByEular().x;


            ///////////////////////////////
            m_Sliders[NearClip]->Set_BindValue(&fDebugNear);
            m_Sliders[NearClip]->Set_Min(0.1f);
            m_Sliders[NearClip]->Set_Max(999.f);
            ///////////////////////////////


             ///////////////////////////////
            m_Sliders[FarClip]->Set_BindValue(&fDebugFar);
            m_Sliders[FarClip]->Set_Min(0.2f);
            m_Sliders[FarClip]->Set_Max(1000.f);
            ///////////////////////////////
            
            ///////////////////////////////
            m_Sliders[FOV]->Set_Active(true);
            m_Sliders[FOV]->Set_BindValue(&fDebugFov);
            m_Sliders[FOV]->Set_Min(0.f);
            m_Sliders[FOV]->Set_Max(1000.f);
            ///////////////////////////////

            ///////////////////////////////
            m_InuptFloats[0]->Set_BindValue(&(fDebugOffSet.x));


            m_InuptFloats[1]->Set_BindValue(&(fDebugOffSet.y));


            m_InuptFloats[2]->Set_BindValue(&(fDebugOffSet.z));

            m_CulRotationSlider->Set_BindValue(&(fCulRotationX));
            m_CulRotationSlider->Set_Callback([pTarget, this]() {
                CMainCamera* pMainCamera = dynamic_cast<CMainCamera*>(pTarget);
                if (pMainCamera)
                    pMainCamera->Set_LocalRoation(_float4(fCulRotationX, 0.f, 0.f, 0.f));

                });
            ///////////////////////////////
            m_CulDistSlider->Set_BindValue(&fCulDist);
            m_CulDistSlider->Set_Callback([pTarget, this]() {
                pTarget->Set_Distance(fCulDist);
                });

            ////////////최종 세팅/////////
            for (int i = NearClip; i <= FOV; ++i)
            {
                m_Sliders[i]->Set_Callback([pTarget, this]() {
                    float fNear = m_Sliders[NearClip]->Get_BindValue();
                    float fFar = m_Sliders[FarClip]->Get_BindValue();
                    float fFov = m_Sliders[FOV]->Get_BindValue();

                    pTarget->Set_Near(fNear);
                    pTarget->Set_Far(fFar);
                    pTarget->Set_Fovy(fFov);
                    });
            }

            for (int i = 0; i < 3; ++i)
            {
                m_InuptFloats[i]->Set_Callback([pTarget,i, this]() {
                    float fX = m_InuptFloats[0]->Get_Bindvalue();
                    float fY = m_InuptFloats[1]->Get_Bindvalue();
                    float fZ = m_InuptFloats[2]->Get_Bindvalue();

                    
                    pTarget->Set_Offset(_float3(fX, fY, fZ));
                    });
            }

            //////////////////////////////////

            


        }
    }

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
    Safe_Release(m_pPerspectiveCamButton);
    Safe_Release(m_pOrthoGraphicCamButton);
    Safe_Release(m_pDefaultOffSetButton);
    Safe_Release(m_CulRotationSlider);
    Safe_Release(m_CulDistSlider);

    for (auto& i : m_InuptFloats)
        Safe_Release(i);


    for (auto& i : m_Sliders)
        Safe_Release(i);

    for (auto& i: m_CamButtons)
        Safe_Release(i);

    for (auto& i : m_CheckBoxs)
        Safe_Release(i);


}
