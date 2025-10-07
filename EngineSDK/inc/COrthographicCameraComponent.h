#pragma once
#include "CCameraComponent.h"

NS_BEGIN(Engine)

class ENGINE_DLL COrthographicCameraComponent :
    public CCameraComponent
{
public:
    typedef struct tagOrthographic_CameraDesc : public CCameraComponent::CAMERA_DESC
    {
        _float ViewHeight = {};
        _float ViewWdith = {};

    }ORTHOGRAPHIC_DESC;
private:
    COrthographicCameraComponent(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    COrthographicCameraComponent(const COrthographicCameraComponent& Prototype);
    virtual ~COrthographicCameraComponent() = default;

public:
    HRESULT     Initialize_Prototype() override;
    HRESULT     Initialize_Copytype(void* pArg) override;

public:
    void        Update_ViewMatrix(_float fTimeDelta) override;
    void        Update_ProjectionMatrix() override;
public:
    static COrthographicCameraComponent* Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    virtual     CComponent* Clone(void* pArg) override;
    virtual     void        Free() override;

private:
    _float      m_vViewHeight = {};
    _float      m_vViewWidth = {};


};

NS_END