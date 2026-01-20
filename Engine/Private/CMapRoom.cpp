#include "CMapRoom.h"
#include "CMapLayer.h"
#include "CMapRoomTrigger.h"
#include "CModel.h"
#include "CMapPosition.h"

CMapRoom::CMapRoom(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    :CMapModel(pDevice,pContext)
{
}

CMapRoom::CMapRoom(const CMapModel& rhs)
    : CMapModel(rhs)
{
}

CMapRoom* CMapRoom::Create(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pDeviceContext)
{
    CMapRoom* pInstance = new CMapRoom(_pDevice, _pDeviceContext);
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Create :CMapRoom ");
        Safe_Release(pInstance);

    }

   
 
  
    return pInstance;
}

CGameObject* CMapRoom::Clone(void* pArg)
{
    CMapRoom* pInstance = new CMapRoom(*this);
    if (FAILED(pInstance->Initialize_Copytype(pArg)))
    {
        MSG_BOX("Failed to Cloned :CMapRoom ");
        Safe_Release(pInstance);

    }

    wstring ModelStr = pInstance->m_pModel->Get_ModelData().name;
    string str;

    str.assign(ModelStr.begin(), ModelStr.end());

    pInstance->RoomName = string(str);
    return pInstance;
}

void CMapRoom::Free()
{

    __super::Free();

}

void CMapRoom::Save_To_Json(json& Json)
{

    json jRoomData;

    

    _float3 vPos;
    XMStoreFloat3(&vPos, m_pTransformCom->Get_State(STATE::POSITION));

    jRoomData["RoomName"] = RoomName;
    jRoomData["Position"] = { vPos.x,vPos.y,vPos.z };

    for (auto& pTrigger : RoomTriggers)
    {
        jRoomData["RoomTriggers"].push_back(pTrigger->Save_Data());

    }
    //Triggerbox부터 저장
    for (auto& pPos : MapPositions)
    {
        jRoomData["MapPositions"].push_back(pPos->Save_Data());

    }

    string FinalPath = "../../Resource/Data/Map/Room/"+ RoomName + "_room" + ".json";

    ofstream	file(FinalPath);
    file << std::setw(4) << jRoomData;

    file.close();


    MSG_BOX("Save Complete");

}

int CMapRoom::Add_RoomTrigger(CMapRoomTrigger* pTrigger)
{
    CheckNullResult(pTrigger, -1);

    RoomTriggers.push_back(pTrigger);
    return RoomTriggers.size()-1;
}

void CMapRoom::Remove_RoomTrigger(_uint iIdx)
{
    RoomTriggers.erase(RoomTriggers.begin() + iIdx);
    for (int i = 0; i < RoomTriggers.size(); ++i)
    {
        RoomTriggers[i]->Set_Idx(i);
       
    }
}

int CMapRoom::Add_Position(CMapPosition* pPos)
{
    CheckNullResult(pPos, -1);

    MapPositions.push_back(pPos);
    return MapPositions.size() - 1;
}

void CMapRoom::Remove_Position(_uint iIdx)
{
    MapPositions.erase(MapPositions.begin() + iIdx);
    for (int i = 0; i < MapPositions.size(); ++i)
    {
        MapPositions[i]->Set_Idx(i);

    }
}

void CMapRoom::Imgui_Render_Properties(_float3* vScale, _float3* vPosition, _float3* vRotation)
{
    __super::Imgui_Render_Properties(vScale, vPosition, vRotation);
    char buffer[128] = "";


    char buf[128];
    strcpy_s(buf, RoomName.c_str());

    if (ImGui::InputText("RoomName", buf, sizeof(buf)))
        RoomName = buf;
}
