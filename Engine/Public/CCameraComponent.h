#pragma once
#include "CComponent.h"

NS_BEGIN(Engine)

class ENGINE_DLL CCameraComponent:
    public CComponent
{
public:
    typedef struct tagCameraDesc
    {
        _float3 vUp = { 0.f,1.f,0.1f };

        _float3 vOffset = {};
        class CGameObject* pTarget = nullptr;

        _float fNear = 0.1f;
        _float fFar = 1000.f;

        _bool   m_bDynamic = true;

    }CAMERACOMP_DESC;



protected:
    CCameraComponent(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    CCameraComponent(const CCameraComponent& Prototype);
    virtual ~CCameraComponent() = default;

public:
    virtual         HRESULT     Initialize_Prototype() override;
    virtual         HRESULT     Initialize_Copytype(void* pArg) override;


public:
    void        Set_Target(class CGameObject* pTarget) { m_pTarget = pTarget; }
    void        Set_Up(_float3 vUp) { m_vUp = vUp; }
public:
    const _float4x4& Get_ViewMatrix() { return m_matView; }
    const _float4x4& Get_ProjMatrix() { return m_matProj; }
    const _float3& Get_OffSet() { return m_vOffSet; }

    _float Get_Near() { return m_fNear; }
    _float Get_Far() { return m_fFar; }

    const _vector Get_Eye() { return vEye; }
    const _vector Get_At() { return vAt; }
    class CGameObject* Get_Target() { return m_pTarget; }

public:
    virtual void        Update_ViewMatrix(_float fTimeDelta);
    virtual void        Set_ProjectionMatrix() {};


public:
    _matrix         Get_MulViewProjMatrix();
protected:
    class       CGameObject* m_pTarget = { nullptr };
   

    _float4x4   m_matProj;        //투영행렬
    _float4x4   m_matView;          //뷰행렬

   _float3 m_vUp = { 0.f, 1.f, 0.f };

    _float3 m_vOffSet = {};

    _float m_fNear = {};
    _float m_fFar = {};

public:
    static CCameraComponent* Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    virtual     CComponent* Clone(void* pArg) override;
    virtual     void        Free() override;

protected:
    _vector     vEye;
    _vector     vAt;
    _bool       m_bDynamic = true;

};


NS_END

