#pragma once
#include "CGameObject.h"

namespace Engine
{
    class CPerspectiveCameraComponent;
}

NS_BEGIN(Engine)
class ENGINE_DLL CMainCamera :
    public CGameObject
{
private:
    CMainCamera(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    CMainCamera(const CMainCamera& rhs);
    virtual ~CMainCamera() = default;

public:
    virtual HRESULT     Initialize_Prototype(); /*원형 객체가 생성될때 부르는 Initialize*/
    virtual HRESULT     Initialize_Copytype(void* pArg); /*사본 객체가 생성될때 부르는 Initialize*/

    virtual void        Update_Priority(_float fTimeDelta);
    virtual void        Update(_float fTimeDelta);
    virtual void        Update_Late(_float fTimeDelta);
    virtual void        Update_Render(_float fTimeDelta);

    virtual HRESULT Render();

public:
    static CMainCamera* Create(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pDeviceContext);
    virtual CGameObject* Clone(void* pArg);
    virtual void    Free() override;

private:
    CPerspectiveCameraComponent* m_pPerspectiveCameraCom = { nullptr };
};

NS_END

