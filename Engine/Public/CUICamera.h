#pragma once
#include "CGameObject.h"

namespace Engine
{
    class COrthographicCameraComponent;
}


NS_BEGIN(Engine)
class ENGINE_DLL CUICamera :
    public CGameObject
{
private:
    CUICamera(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    CUICamera(const CUICamera& rhs);
    virtual ~CUICamera() = default;

public:
    virtual HRESULT     Initialize_Prototype(); /*원형 객체가 생성될때 부르는 Initialize*/
    virtual HRESULT     Initialize_Copytype(void* pArg); /*사본 객체가 생성될때 부르는 Initialize*/

    virtual void        Update_Priority(_float fTimeDelta);
    virtual void        Update(_float fTimeDelta);
    virtual void        Update_Late(_float fTimeDelta);
    virtual void        Update_Render(_float fTimeDelta);

    virtual HRESULT Render();

public:
    void        Set_Target(CGameObject * pTarget);
    void        Follow_Target(_float fTimeDelta);

public:
    static CUICamera* Create(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pDeviceContext);
    virtual CGameObject* Clone(void* pArg);
    virtual void    Free() override;

private:
    COrthographicCameraComponent* m_pOrthographicCameraCom = { nullptr };
};
NS_END

