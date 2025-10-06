#pragma once
#include "CCameraComponent.h"

NS_BEGIN(Engine)
class ENGINE_DLL CPerspectiveCameraComponent final :
    public CCameraComponent
{
public:
    typedef struct tagPerspective_CameraDesc : public CCameraComponent::CAMERA_DESC
    {
        float fFov = XMConvertToRadians(90.f);
        float Aspect = {};


    }PERSPECTIVE_DESC;

protected:
    CPerspectiveCameraComponent(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    CPerspectiveCameraComponent(const CCameraComponent& Prototype);
    virtual ~CPerspectiveCameraComponent() = default;

public:
    HRESULT     Initialize_Prototype() override;
    HRESULT     Initialize_Copytype(void* pArg) override;

public:
    void        Update_ViewMatrix(_float fTimeDelta) override;
    void        Update_ProjectionMatrix() override;
public:
    static CPerspectiveCameraComponent* Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    virtual     CComponent* Clone(void* pArg) override;
    virtual     void        Free() override;

private:
    _float      m_fFov = {};
    _float      m_fAspect = {};
};
NS_END

