#include "CTerrainBuildWindow.h"
#include "CImgui_InputInt.h"
#include "CImgui_Button.h"
#include "CImgui_InputFloat.h"
#include "CGameInstance.h"
#include "CModel.h"
#include "CMapTerrain.h"
#include "Client_Defines.h"
#include "CMapObject_Manager.h"

USING(MapTool)
CTerrainBuildWindow::CTerrainBuildWindow(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    :CImgui_Window(pDevice,pContext), m_pGameInstance(CGameInstance::GetInstance()), m_pMapObjectManager(CMapObject_Manager::GetInstance())
{
    Safe_AddRef(m_pGameInstance);
    Safe_AddRef(m_pMapObjectManager);
}

HRESULT CTerrainBuildWindow::Create_Widgets()
{
    //시작모델 번호설정
    CImgui_InputInt::ImguiTextInputDesc StartNumDesc;
    StartNumDesc.Label = "StartNumber";
    StartNumDesc.pData = &m_iStartModelNum;
    StartNumDesc.callback = nullptr;
    StartNumDesc.m_RelativePos = ImVec2(0, 50);
    if (FAILED(Add_Widgets<CImgui_InputInt>(&StartNumDesc, reinterpret_cast<CImgui_Widget**>(&ModelNumInput[0]))))
        return E_FAIL;

    //마지막모델 번호
    CImgui_InputInt::ImguiTextInputDesc EndNumDesc;
    EndNumDesc.Label = "EndNumber";                 
    EndNumDesc.pData = &m_iEndModelNum;
    EndNumDesc.callback = nullptr;      
    EndNumDesc.m_RelativePos = ImVec2(0,  70);
    if (FAILED(Add_Widgets<CImgui_InputInt>(&EndNumDesc, reinterpret_cast<CImgui_Widget**>(&ModelNumInput[1]))))
        return E_FAIL;


    //오프셋 설정(X,Z)
    CImgui_InputFloat::IMGUITEXTFLOAT_DESC OffsetXDesc;
    OffsetXDesc.Label = "X OffSet";
    OffsetXDesc.Tag = "X OffSet";
    OffsetXDesc.m_RelativePos = ImVec2(0, 100);
    OffsetXDesc.pData = &m_fOffSet.x;
    OffsetXDesc.Step = 0.1f;
    if (FAILED(Add_Widgets<CImgui_InputFloat>(&OffsetXDesc, reinterpret_cast<CImgui_Widget**>(&OffSet[0]))))
        return E_FAIL;


    CImgui_InputFloat::IMGUITEXTFLOAT_DESC OffsetZDesc;
    OffsetZDesc.Label = "Z OffSet";
    OffsetZDesc.Tag = "Z OffSet";
    OffsetZDesc.m_RelativePos = ImVec2(0, 120);
    OffsetZDesc.pData = &m_fOffSet.z;
    OffsetZDesc.Step = 0.1f;
    if (FAILED(Add_Widgets<CImgui_InputFloat>(&OffsetZDesc, reinterpret_cast<CImgui_Widget**>(&OffSet[1]))))
        return E_FAIL;


    //빌드 시작좌표설정
    CImgui_InputFloat::IMGUITEXTFLOAT_DESC StartPosXDesc;
    StartPosXDesc.Label = "Pos X";
    StartPosXDesc.Tag = "Pos X";
    StartPosXDesc.m_RelativePos = ImVec2(0, 150);
    StartPosXDesc.pData = &m_fStartPos.x;
    StartPosXDesc.Step = 0.1f;
    if (FAILED(Add_Widgets<CImgui_InputFloat>(&StartPosXDesc, reinterpret_cast<CImgui_Widget**>(&buildPosition[0]))))
        return E_FAIL;


    CImgui_InputFloat::IMGUITEXTFLOAT_DESC EndPosZDesc;
    EndPosZDesc.Label = "Pos Z";
    EndPosZDesc.Tag = "Pos Z";
    EndPosZDesc.m_RelativePos = ImVec2(0, 180);
    EndPosZDesc.pData = &m_fStartPos.z;
    EndPosZDesc.Step = 0.1f;
    if (FAILED(Add_Widgets<CImgui_InputFloat>(&EndPosZDesc, reinterpret_cast<CImgui_Widget**>(&buildPosition[1]))))
        return E_FAIL;

    ///최종Buil버튼
    CImgui_Button::ImguiButton_Desc ButtonDesc;
    ButtonDesc.Tag = "Build";
    ButtonDesc.Label = "Build";
    ButtonDesc.m_RelativePos = ImVec2(200, 200);
    ButtonDesc.callback = [this]()
    {
        Start_Build();
    };

    if (FAILED(Add_Widgets<CImgui_Button>(&ButtonDesc, reinterpret_cast<CImgui_Widget**>(&m_pBuildButton))))
        return E_FAIL;
    
    return S_OK;
}

HRESULT CTerrainBuildWindow::Create_Terrain(const wstring& Tag, _float3 vPosition)
{
    CMapTerrain::MAPTERRAIN_DESC Desc;
    Desc.eRenderGroup = ENUM_TO_UINT(RENDERGROUP::PRIORITY);
    Desc.ObjTag = m_pMapObjectManager->Generate_UniqueTag(MapObjType::TERRAIN, Tag);
    Desc.modelName = Tag;
    Desc.ObjType = MapObjType::TERRAIN;
      
    CTransform::TRANSFORM_DESC TransDesc;
    TransDesc.vLocalPosition = _float4(vPosition.x, vPosition.y, vPosition.z, 1.f);

    CModel::MODEL_DSC modelDesc;
    Desc.modelDesc = &modelDesc;
    Desc.TransformDesc = &TransDesc;

    wstring LayerTag = L"Terrain_Layer";
    wstring ProtoTag = L"MapTerrain";

    CGameObject* pCloneObj = dynamic_cast<CGameObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_TO_UINT(LEVEL_ID::STATIC), PROTO_OBJ_NAME(ProtoTag), &Desc));
    if (pCloneObj)
    {
        CTerrain_Base* pTerrain = dynamic_cast<CTerrain_Base*>(pCloneObj);
        if (pTerrain)
        {
            m_pGameInstance->Register_Terrain(Desc.ObjTag, pTerrain);
            return S_OK;
        }
    }
    return E_FAIL;
}

HRESULT CTerrainBuildWindow::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Create_Widgets()))
        return E_FAIL;


    return S_OK;

}

void CTerrainBuildWindow::Update()
{
    __super::Update();
}

void CTerrainBuildWindow::Render()
{
}

CTerrainBuildWindow* CTerrainBuildWindow::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, void* pArg)
{
    CTerrainBuildWindow* pInstance = new CTerrainBuildWindow(pDevice, pContext);


    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Create : CTerrainBuildWindow");
        Safe_Release(pInstance);
    }


    return pInstance;
}

void CTerrainBuildWindow::Free()
{
    __super::Free();

    for (int i = 0; i < 2; ++i) 
    {
        Safe_Release(ModelNumInput[i]);
        Safe_Release(OffSet[i]);
    }

    for (int i = 0; i < 3; ++i)
    {
        Safe_Release(buildPosition[i]);
    }

    Safe_Release(m_pBuildButton);

    Safe_Release(m_pGameInstance);
    Safe_Release(m_pMapObjectManager);

}

void CTerrainBuildWindow::Start_Build()
{
  
    //1.모델로드 
    wchar_t      pAlphabets[16] = { L'A',L'B',L'C',L'D',L'E',L'F',L'G',L'H',L'I',L'J',L'K',L'L',L'M',L'N',L'O',L'P'};
    for (int i = m_iStartModelNum; i <= m_iEndModelNum; ++i)
    {
        //시작위치 설정
        _float3 m_fTargetStartPos = m_fStartPos;
        m_fTargetStartPos.z = m_fStartPos.z - (m_fOffSet.z * (i-m_iStartModelNum));


        //현재 번호에 맞는 모델모두로드
        vector<CModel*>     m_Models(16);
        wstring Base = L"Field_" + to_wstring(i);
       
        //알파벳순서대로..
        for (int j = 0; j< 16; ++j)
        {
           
            wstring ModelName = Base + pAlphabets[j];

            CModel* pFindModel = m_pGameInstance->Find_Model(ModelName);

            if (pFindModel)
                m_Models[j] = pFindModel;

            else
                m_Models[j] = nullptr;          //없는 부분은 그냥 비워놓을려고.

        }

        //한줄씩 설치..
        if (m_Models.empty())
            continue;

        for (size_t k = 0; k < m_Models.size(); ++k)
        {
            m_fTargetStartPos.x = m_fStartPos.x + (m_fOffSet.x * k);
            if (m_Models[k])
            {
                Create_Terrain(m_Models[k]->Get_ModelData().name, m_fTargetStartPos);

            }
        
        }

    }



}
