#pragma once
#include "CCamera_Base.h"

namespace Engine
{
    class COrthographicCameraComponent;
    class CShader;
}

NS_BEGIN(Engine)
class ENGINE_DLL CPostProcessCamera :
    public CCamera_Base
{
private:
    CPostProcessCamera(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    CPostProcessCamera(const CPostProcessCamera& rhs);
    virtual ~CPostProcessCamera() = default;

public:
    virtual HRESULT     Initialize_Prototype(); /*원형 객체가 생성될때 부르는 Initialize*/
    virtual HRESULT     Initialize_Copytype(void* pArg); /*사본 객체가 생성될때 부르는 Initialize*/

    virtual void        Update_Priority(_float fTimeDelta);
    virtual void        Update(_float fTimeDelta);
    virtual void        Update_Late(_float fTimeDelta);
    virtual void        Update_Render(_float fTimeDelta);

    virtual HRESULT Render();


public:
    static CPostProcessCamera* Create(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pDeviceContext);
    virtual CGameObject* Clone(void* pArg);
    virtual void    Free() override;

};
NS_END

