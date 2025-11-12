#include "CObjectInspectorWindow.h"
#include "CGameInstance.h"
#include "CMapObject.h"
#include "CImgui_Widget.h"
#include "CImgui_InputInt.h"
#include "CImgui_InputFloat.h"
#include "MathUtils.h"

#include "CMapObject_Manager.h"
#include "IMapEditable.h"
#include "ImGuizmo.h"




USING(MapTool)
CObjectInspectorWindow::CObjectInspectorWindow(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    :CImgui_Window(pDevice,pContext),
    pGameInstance(CGameInstance::GetInstance()),
    m_pMapObject_Manager(CMapObject_Manager::GetInstance())
{
    Safe_AddRef(pGameInstance);
    Safe_AddRef(m_pMapObject_Manager);

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
  
    Update_SelectObject();
    m_fMoveSpeed = m_pMapObject_Manager->Get_MoveSpeed();


    if (pSelectObject)
    {
        IMapEditable* pMapEditable = dynamic_cast<IMapEditable*>(pSelectObject);
        if (pMapEditable)
            pMapEditable->Show_Gizmo(); 
    }
    ImGui::End();

  
    __super::Update();

   
}

void CObjectInspectorWindow::Render()
{
   /* if (pSelectObject)
    {
        IMapEditable* pMapEditable = dynamic_cast<IMapEditable*>(pSelectObject);
        if (pMapEditable)
            pMapEditable->Show_Gizmo();

    }*/



}

HRESULT CObjectInspectorWindow::Create_Widgets()
{
    float fScaleButtonY = 80;

    CImgui_InputFloat::IMGUITEXTFLOAT_DESC InputFloatDesc;
    InputFloatDesc.Label = "ScaleX";
    InputFloatDesc.Tag = InputFloatDesc.Label;
    InputFloatDesc.m_RelativePos = ImVec2(0, fScaleButtonY);
    InputFloatDesc.pData = &vScale.x;
    InputFloatDesc.Step = 0.1f;



    if (FAILED(Add_Widgets<CImgui_InputFloat>(&InputFloatDesc, reinterpret_cast<CImgui_Widget**>(&ScaleInput[0]))))
        return E_FAIL;


    InputFloatDesc.Label = "ScaleY";
    InputFloatDesc.Tag = InputFloatDesc.Label;
    InputFloatDesc.m_RelativePos = ImVec2(0, fScaleButtonY+20);
    InputFloatDesc.pData = &vScale.y;
    if (FAILED(Add_Widgets<CImgui_InputFloat>(&InputFloatDesc, reinterpret_cast<CImgui_Widget**>(&ScaleInput[1]))))
        return E_FAIL;

    InputFloatDesc.Label = "ScaleZ";
    InputFloatDesc.Tag = InputFloatDesc.Label;
    InputFloatDesc.m_RelativePos = ImVec2(0, fScaleButtonY+40);
    InputFloatDesc.pData = &vScale.z;  
    if (FAILED(Add_Widgets<CImgui_InputFloat>(&InputFloatDesc, reinterpret_cast<CImgui_Widget**>(&ScaleInput[2]))))
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

    /// //////////////////////POSITIONS
    float fPosButtonY = fScaleButtonY+80;
    CImgui_InputFloat::IMGUITEXTFLOAT_DESC InputFloatDesc_Pos;
    InputFloatDesc_Pos.Label = "PositionX";
    InputFloatDesc_Pos.Tag = InputFloatDesc_Pos.Label;
    InputFloatDesc_Pos.m_RelativePos = ImVec2(0, fPosButtonY);
    InputFloatDesc_Pos.pData = &vPosition.x;


    if (FAILED(Add_Widgets<CImgui_InputFloat>(&InputFloatDesc_Pos, reinterpret_cast<CImgui_Widget**>(&PositionInput[0]))))
        return E_FAIL;


    InputFloatDesc_Pos.Label = "PositionY";
    InputFloatDesc_Pos.Tag = InputFloatDesc_Pos.Label;
    InputFloatDesc_Pos.m_RelativePos = ImVec2(0, fPosButtonY+20);
    InputFloatDesc_Pos.pData = &vPosition.y;
    if (FAILED(Add_Widgets<CImgui_InputFloat>(&InputFloatDesc_Pos, reinterpret_cast<CImgui_Widget**>(&PositionInput[1]))))
        return E_FAIL;

    InputFloatDesc_Pos.Label = "PositionZ";
    InputFloatDesc_Pos.Tag = InputFloatDesc_Pos.Label;
    InputFloatDesc_Pos.m_RelativePos = ImVec2(0, fPosButtonY+40);
    InputFloatDesc_Pos.pData = &vPosition.z;
    if (FAILED(Add_Widgets<CImgui_InputFloat>(&InputFloatDesc_Pos, reinterpret_cast<CImgui_Widget**>(&PositionInput[2]))))
        return E_FAIL;

    for (int i = 0; i < 3; ++i)
    {
        if (PositionInput[i])
        {
            PositionInput[i]->Set_Callback([this]()
                {
                    if (pSelectObject)
                        pSelectObject->Get_Transform()->Set_State(STATE::POSITION,_float4(vPosition.x, vPosition.y, vPosition.z, 1.f));

                });
            PositionInput[i]->Set_Active(false);
        }

    }

    //////////////////Rotations
    float fRotationButtonY = fPosButtonY + 80;
    CImgui_InputFloat::IMGUITEXTFLOAT_DESC InputFloatDesc_Rot;
    InputFloatDesc_Rot.Label = "RotationX";
    InputFloatDesc_Rot.Tag = InputFloatDesc_Rot.Label;
    InputFloatDesc_Rot.m_RelativePos = ImVec2(0, fRotationButtonY);
    InputFloatDesc_Rot.pData = &vRotation.x;


    if (FAILED(Add_Widgets<CImgui_InputFloat>(&InputFloatDesc_Rot, reinterpret_cast<CImgui_Widget**>(&RotationInput[0]))))
        return E_FAIL;


    InputFloatDesc_Rot.Label = "RotationY";
    InputFloatDesc_Rot.Tag = InputFloatDesc_Rot.Label;
    InputFloatDesc_Rot.m_RelativePos = ImVec2(0, fRotationButtonY + 20);
    InputFloatDesc_Rot.pData = &vRotation.y;
    if (FAILED(Add_Widgets<CImgui_InputFloat>(&InputFloatDesc_Rot, reinterpret_cast<CImgui_Widget**>(&RotationInput[1]))))
        return E_FAIL;

    InputFloatDesc_Rot.Label = "RotationZ";
    InputFloatDesc_Rot.Tag = InputFloatDesc_Rot.Label;
    InputFloatDesc_Rot.m_RelativePos = ImVec2(0, fRotationButtonY + 40);
    InputFloatDesc_Rot.pData = &vRotation.z;
    if (FAILED(Add_Widgets<CImgui_InputFloat>(&InputFloatDesc_Rot, reinterpret_cast<CImgui_Widget**>(&RotationInput[2]))))
        return E_FAIL;

    for (int i = 0; i < 3; ++i)
    {
        if (RotationInput[i])
        {
            RotationInput[i]->Set_Callback([this]()
                {
                    if (pSelectObject)
                        pSelectObject->Get_Transform()->Rotation(vRotation);

                });
            RotationInput[i]->Set_Active(false);
        }

    }

    ///이동스피드
    CImgui_InputFloat::IMGUITEXTFLOAT_DESC InputFloatDesc_Speed;
    float fSpeedButtonY = fPosButtonY + 200;
    
    InputFloatDesc_Speed.Label = "FloatSpeed";
    InputFloatDesc_Speed.Tag = InputFloatDesc_Speed.Label;
    InputFloatDesc_Speed.m_RelativePos = ImVec2(0, fSpeedButtonY);
    InputFloatDesc_Speed.pData = &m_fMoveSpeed;
    if (FAILED(Add_Widgets<CImgui_InputFloat>(&InputFloatDesc_Speed, reinterpret_cast<CImgui_Widget**>(&SpeedButton))))
        return E_FAIL;

    if (SpeedButton)
    {
        SpeedButton->Set_Callback([this]()
            {
                m_pMapObject_Manager->Set_MoveSpeed(m_fMoveSpeed);

            });
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


    for (int i = 0; i < 3; ++i)
        Safe_Release(PositionInput[i]);

    for (int i = 0; i < 3; ++i)
        Safe_Release(RotationInput[i]);

    Safe_Release(SpeedButton);

    Safe_Release(m_pMapObject_Manager);
    Safe_Release(pGameInstance);
}

void CObjectInspectorWindow::Update_SelectObject()
{
    /*선택한 오브젝트에 따라서 바인딩값 변경*/

    if (m_pMapObject_Manager->Get_SelectObject())
        pSelectObject = dynamic_cast<CGameObject*>(m_pMapObject_Manager->Get_SelectObject());

    else
        pSelectObject = nullptr;

    CheckNull(m_pMapObject_Manager->Get_SelectObject());


   
    IMapEditable* ppSelectObject = dynamic_cast<IMapEditable*>(pSelectObject);
    if (ppSelectObject)
    {
        ppSelectObject->Imgui_Render_Properties(&vScale, &vPosition, &vRotation);

        for (int i = 0; i < 3; ++i)
        {
            if (ScaleInput[i])
                ScaleInput[i]->Set_Active(true);
        }

        for (int i = 0; i < 3; ++i)
        {
            if (PositionInput[i])
                PositionInput[i]->Set_Active(true);
        }

        for (int i = 0; i < 3; ++i)
        {
            if (RotationInput[i])
                RotationInput[i]->Set_Active(true);
        }

    }

    else
    {
        //선택된게 없으면 모두비활성화.
        for (int i = 0; i < 3; ++i)
        {
            if (ScaleInput[i])
                ScaleInput[i]->Set_Active(false);
        }

        //선택된게 없으면 모두비활성화.
        for (int i = 0; i < 3; ++i)
        {
            if (PositionInput[i])
                PositionInput[i]->Set_Active(false);
        }

        //선택된게 없으면 모두비활성화.
        for (int i = 0; i < 3; ++i)
        {
            if (RotationInput[i])
                RotationInput[i]->Set_Active(false);
        }
    }
}
