#pragma once
#include "CMapObject.h"

NS_BEGIN(Engine)
class CCollider_Base;
class ENGINE_DLL CMapTrigger :
    public CMapObject
{

protected:
    CMapTrigger(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    CMapTrigger(const CMapObject& rhs);
    virtual ~CMapTrigger() = default;
public:
    HRESULT     Initialize_Prototype(); /*원형 객체가 생성될때 부르는 Initialize*/
    virtual HRESULT     Initialize_Copytype(void* pArg); /*사본 객체가 생성될때 부르는 Initialize*/

    void        Update_Priority(_float fTimeDelta);
    void        Update(_float fTimeDelta);
    void        Update_Late(_float fTimeDelta);
    void        Update_Render(_float fTimeDelta);


    virtual HRESULT Render();

public:
    static CMapTrigger* Create(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pDeviceContext);
    virtual CGameObject* Clone(void* pArg) override;
    virtual void Free() override;

public:
    //픽킹 확인
    bool            Is_Picked(_vector Origin, _vector Dir, float& Dist);
    virtual void Save_To_Json(json& Json) override;

public:
    virtual void Imgui_Render_Properties(_float3* vScale, _float3* vPosition, _float3* vRotation) override;

public:
    virtual HRESULT     Ready_Component(void* pArg);


};


NS_END