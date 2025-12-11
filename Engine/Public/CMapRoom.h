#pragma once
#include "CMapModel.h"
#include "Parsing_TriggerInfo.h"

NS_BEGIN(Engine)
class CMapRoomTrigger;
class CMapPosition;

class ENGINE_DLL CMapRoom :
    public CMapModel
{
protected:
    CMapRoom(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    CMapRoom(const CMapModel& rhs);
    virtual ~CMapRoom() = default;

public:
    static CMapRoom* Create(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pDeviceContext);
    virtual CGameObject* Clone(void* pArg) override;
    virtual void Free() override;

public:
    void        Save_To_Json(const string& filePath);
    void        Set_RoomName(string roomName) { RoomName = roomName; }

public:
    int Add_RoomTrigger(CMapRoomTrigger* pTrigger);
    void Remove_RoomTrigger(_uint iIdx);


    int Add_Position(CMapPosition* pPos);
    void Remove_Position(_uint iIdx);

private:
    string      RoomName = "";

public:
    virtual void Imgui_Render_Properties(_float3* vScale, _float3* vPosition, _float3* vRotation) override;

private:
    vector<CMapRoomTrigger*> RoomTriggers;
    vector< CMapPosition*> MapPositions;

    _uint               iIdx = 0;


};

NS_END