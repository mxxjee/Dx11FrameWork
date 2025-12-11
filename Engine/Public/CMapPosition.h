#pragma once
#include "CMapObject.h"
#include "Parsing_RoomInfo.h"

NS_BEGIN(Engine)
class CCollider_Base;

class ENGINE_DLL CMapPosition :
    public CMapObject
{
protected:
    CMapPosition(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    CMapPosition(const CMapObject& rhs);
    virtual ~CMapPosition() = default;
public:
    HRESULT     Initialize_Prototype(); /*원형 객체가 생성될때 부르는 Initialize*/
    virtual HRESULT     Initialize_Copytype(void* pArg); /*사본 객체가 생성될때 부르는 Initialize*/


public:
    static CMapPosition* Create(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pDeviceContext);
    virtual CGameObject* Clone(void* pArg) override;
    virtual void Free() override;

public:
    //픽킹 확인
    virtual void Save_To_Json(json & Json) override;
public:
    virtual void        Update_Late(_float fTimeDelta);
    PositionInfo        Get_Info() { return Info; }
    void            Set_Idx(_uint i) { m_idx = i; }
public:
    virtual void Imgui_Render_Properties(_float3 * vScale, _float3 * vPosition, _float3 * vRotation) override;
    virtual void        OnDestroy();

private:
    CMapObject* pOwner = nullptr;
    PositionInfo Info;
    _uint m_idx = 0;
};

NS_END