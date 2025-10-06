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

    }CAMERA_DESC;



protected:
    CCameraComponent(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    CCameraComponent(const CCameraComponent& Prototype);
    virtual ~CCameraComponent() = default;

public:
    virtual         HRESULT     Initialize_Prototype() override;
    virtual         HRESULT     Initialize_Copytype(void* pArg) override;


public:
    void        Set_Target(class CGameObject* pTarget) { m_pTarget = pTarget; }
    

    const _float4x4& Get_ViewMatrix() { return m_matView; }
    const _float4x4& Get_ProjMatrix() { return m_matProj; }
    const _float3& Get_OffSet() { return m_vOffSet; }

public:
    virtual void        Update_ViewMatrix(_float fTimeDelta);
    virtual void        Update_ProjectionMatrix() {};

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


};


NS_END

