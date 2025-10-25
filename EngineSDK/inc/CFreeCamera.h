#pragma once
#include "CCamera_Base.h"

namespace Engine
{
    class CPerspectiveCameraComponent;
}

NS_BEGIN(Engine)
class ENGINE_DLL CFreeCamera :
    public CCamera_Base
{
private:
    CFreeCamera(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    CFreeCamera(const CFreeCamera& rhs);
    virtual ~CFreeCamera() = default;

public:
    virtual HRESULT     Initialize_Prototype(); /*원형 객체가 생성될때 부르는 Initialize*/
    virtual HRESULT     Initialize_Copytype(void* pArg); /*사본 객체가 생성될때 부르는 Initialize*/

    virtual void        Update_Priority(_float fTimeDelta);
    virtual void        Update(_float fTimeDelta);
    virtual void        Update_Late(_float fTimeDelta);
    virtual void        Update_Render(_float fTimeDelta);

    virtual HRESULT Render();


private:
    void    Mouse_Move();
    void    Mouse_Fix();
public:
    static CFreeCamera* Create(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pDeviceContext);
    virtual CGameObject* Clone(void* pArg);
    virtual void    Free() override;

private:
    long                    m_ScreenWidth = {};
    long                    m_ScreenHeight = {};

    float                   m_fInitSpeed = {};


};
NS_END
