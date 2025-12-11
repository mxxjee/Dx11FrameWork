#include "CMapRoomTrigger.h"
#include "CMapObject_Manager.h"
#include "CMapLayer.h"
#include "CMapRoom.h"
#include "CCollider_Base.h"
#include "CBounding_AABB.h"
#include "CInput_Manager.h"


CMapRoomTrigger::CMapRoomTrigger(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    :CMapTrigger(pDevice, pContext)
{
}

CMapRoomTrigger::CMapRoomTrigger(const CMapObject& rhs)
    : CMapTrigger(rhs)
{
}

HRESULT CMapRoomTrigger::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;


    return S_OK;
}

HRESULT CMapRoomTrigger::Initialize_Copytype(void* pArg)
{
    if (FAILED(__super::Initialize_Copytype(pArg)))
        return E_FAIL;

    m_tInfo.pOwner = this;

    CMapLayer* pLayer=CMapObject_Manager::GetInstance()->Get_Layer_By_MapObjType(MapObjType::ROOM);
    CheckNullResult(pLayer,E_FAIL);

    //룸트리거들을 owner=룸 을 가진다.
    pOwner = pLayer->Find_GameObject(0);
    CheckNullResult(pOwner,E_FAIL);


    CMapRoom* pRoom=dynamic_cast<CMapRoom*>(pOwner);
    CheckNullResult(pRoom, E_FAIL);

    m_idx=pRoom->Add_RoomTrigger(this);

    pAABB = dynamic_cast<CBounding_AABB*>(pColliderComp->Get_Bounding());

    return S_OK;
}

void CMapRoomTrigger::Update_Priority(_float fTimeDelta)
{
    __super::Update_Priority(fTimeDelta);

}

void CMapRoomTrigger::Update(_float fTimeDelta)
{
    __super::Update(fTimeDelta);

}

void CMapRoomTrigger::Update_Late(_float fTimeDelta)
{
    __super::Update_Late(fTimeDelta);

    CheckNull(pAABB);
    m_tInfo.vCenter = pAABB->Get_OrignialDesc()->Center;
    m_tInfo.vExtents = pAABB->Get_OrignialDesc()->Extents;

 
    
}

void CMapRoomTrigger::Update_Render(_float fTimeDelta)
{
    __super::Update_Render(fTimeDelta);

}

HRESULT CMapRoomTrigger::Render()
{
    __super::Render();

    return S_OK;
}

void CMapRoomTrigger::Save_To_Json(json& Json)
{
   
}

void CMapRoomTrigger::Update_SelectMode(float _fTimeDelta)
{
    __super::Update_SelectMode(_fTimeDelta);
    if (CInput_Manager::GetInstance()->IsKeyPressed(KeyCode::Delete))
    {
        CMapRoom* pRoom = dynamic_cast<CMapRoom*>(pOwner);
        CheckNull(pRoom);

        pRoom->Remove_RoomTrigger(m_idx);
    }
}

void CMapRoomTrigger::Imgui_Render_Properties(_float3* vScale, _float3* vPosition, _float3* vRotation)
{
    __super::Imgui_Render_Properties(vScale, vPosition, vRotation);

    char buffer[128] = "";


    char buf[128];
    strcpy_s(buf, m_NextRoomID.c_str());

    if (ImGui::InputText("NextRoom", buf, sizeof(buf)))
        m_NextRoomID = buf;

}

CMapRoomTrigger* CMapRoomTrigger::Create(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pDeviceContext)
{
    CMapRoomTrigger* pInstance = new CMapRoomTrigger(_pDevice, _pDeviceContext);
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Create :CMapRoomTrigger ");
        Safe_Release(pInstance);

    }



    return pInstance;
}

CGameObject* CMapRoomTrigger::Clone(void* pArg)
{
    CMapRoomTrigger* pInstance = new CMapRoomTrigger(*this);
    if (FAILED(pInstance->Initialize_Copytype(pArg)))
    {
        MSG_BOX("Failed to Cloned :CMapRoomTrigger ");
        Safe_Release(pInstance);

    }
    return pInstance;
}

void CMapRoomTrigger::Free()
{
    __super::Free();

    
}
