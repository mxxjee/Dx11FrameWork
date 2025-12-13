#include "CMapPosition.h"
#include "CMapLayer.h"
#include "CMapRoom.h"
#include "CMapObject_Manager.h"

CMapPosition::CMapPosition(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    :CMapObject(pDevice, pContext)
{
}

CMapPosition::CMapPosition(const CMapObject& rhs)
    : CMapObject(rhs)
{
}

HRESULT CMapPosition::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;


    return S_OK;
}

HRESULT CMapPosition::Initialize_Copytype(void* pArg)
{

    if (FAILED(__super::Initialize_Copytype(pArg)))
        return E_FAIL;

    MapPositionDesc* pDesc = static_cast<MapPositionDesc*>(pArg);
    Info.TargetName = pDesc->TargetName;

    CMapLayer* pLayer = CMapObject_Manager::GetInstance()->Get_Layer_By_MapObjType(MapObjType::ROOM);
    CheckNullResult(pLayer, E_FAIL);

    //룸트리거들을 owner=룸 을 가진다.
    pOwner = pLayer->Find_GameObject(0);
    CheckNullResult(pOwner, E_FAIL);


    CMapRoom* pRoom = dynamic_cast<CMapRoom*>(pOwner);
    CheckNullResult(pRoom, E_FAIL);

    m_idx = pRoom->Add_Position(this);
    m_bAblePicking = false;

    return S_OK;
}

CMapPosition* CMapPosition::Create(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pDeviceContext)
{
    CMapPosition* pInstance = new CMapPosition(_pDevice, _pDeviceContext);
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Create :CMapPosition ");
        Safe_Release(pInstance);

    }
    return pInstance;
}

CGameObject* CMapPosition::Clone(void* pArg)
{
    CMapPosition* pInstance = new CMapPosition(*this);
    if (FAILED(pInstance->Initialize_Copytype(pArg)))
    {
        MSG_BOX("Failed to Cloned :CMapPosition ");
        Safe_Release(pInstance);

    }

    return pInstance;
}

void CMapPosition::Free()
{
}

void CMapPosition::Save_To_Json(json& Json)
{
    Info.ToJson();
}

void CMapPosition::Update_Late(_float fTimeDelta)
{
    __super::Update_Late(fTimeDelta);
   
}

void CMapPosition::Imgui_Render_Properties(_float3* vScale, _float3* vPosition, _float3* vRotation)
{
    __super::Imgui_Render_Properties(vScale, vPosition, vRotation);
    char buffer[128] = "";


    char buf[128];
    strcpy_s(buf, Info.TargetName.c_str());

    if (ImGui::InputText("Target", buf, sizeof(buf)))
        Info.TargetName = buf;

}

void CMapPosition::OnDestroy()
{
    CheckNull(pOwner);


    CMapRoom* pRoom = dynamic_cast<CMapRoom*>(pOwner);
    CheckNull(pRoom);
    pRoom->Remove_Position(m_idx);
}

json CMapPosition::Save_Data()
{
    XMStoreFloat3(&Info.vPos, m_pTransformCom->Get_State(STATE::POSITION));

    return Info.ToJson();
}


