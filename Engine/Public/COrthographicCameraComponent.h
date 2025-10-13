#pragma once
#include "CCameraComponent.h"

NS_BEGIN(Engine)

class ENGINE_DLL COrthographicCameraComponent :
    public CCameraComponent
{
public:
    typedef struct tagOrthographic_CameraDesc : public CCameraComponent::CAMERACOMP_DESC
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
    void        Set_ProjectionMatrix() override;
public:
    virtual  void            Set_CameraValue(float _fNear, float _fFar,float _vViewHeight=0.f, float _vViewWidth=0.f);

public:
    static COrthographicCameraComponent* Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    virtual     CComponent* Clone(void* pArg) override;
    virtual     void        Free() override;

private:
    _float      m_vViewHeight = {};
    _float      m_vViewWidth = {};


};

NS_END