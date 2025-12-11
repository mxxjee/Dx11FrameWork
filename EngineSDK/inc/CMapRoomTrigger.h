#pragma once
#include "CMapTrigger.h"
#include "Parsing_TriggerInfo.h"

NS_BEGIN(Engine)
class CBounding_AABB;

class ENGINE_DLL CMapRoomTrigger :
    public CMapTrigger
{
protected:
    CMapRoomTrigger(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    CMapRoomTrigger(const CMapObject& rhs);
    virtual ~CMapRoomTrigger() = default;

public:
    HRESULT     Initialize_Prototype(); /*원형 객체가 생성될때 부르는 Initialize*/
    virtual HRESULT     Initialize_Copytype(void* pArg); /*사본 객체가 생성될때 부르는 Initialize*/

    void        Update_Priority(_float fTimeDelta);
    void        Update(_float fTimeDelta);
    void        Update_Late(_float fTimeDelta);
    void        Update_Render(_float fTimeDelta);


    virtual HRESULT Render();

    virtual void Save_To_Json(json& Json) override;
    virtual void Update_SelectMode(float _fTimeDelta);
public:
    virtual void Imgui_Render_Properties(_float3* vScale, _float3* vPosition, _float3* vRotation) override;

public:
    static CMapRoomTrigger* Create(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pDeviceContext);
    virtual CGameObject* Clone(void* pArg) override;
    virtual void Free() override;

public:
    RoomTrigger     Get_Info() { return m_tInfo; }
    void            Set_Idx(_uint i) { m_idx = i; }
    virtual         void OnDestory();
    json            Save_Data();
private:
    CMapObject* pOwner = nullptr;
    string  m_NextRoomID = "";
    _uint m_idx = 0;
private:
    RoomTrigger     m_tInfo;
    CBounding_AABB* pAABB;


};
NS_END
