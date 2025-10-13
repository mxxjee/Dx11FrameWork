#pragma once
#include "CGameObject.h"

namespace Engine
{
    class CShader;
    class CCameraComponent;

}

NS_BEGIN(Engine)


class ENGINE_DLL CCamera_Base :
    public CGameObject
{
public:
    typedef struct tagCamera_DESC : CGameObject::GAMEOBJECT_DESC
    {
        CAMERA_TYPE eCameraType= CAMERA_TYPE::END;
        CAMERA_FLAG eCameraFlag = CAMERA_FLAG::NONE;

    }CAMERABASE_DESC;

protected:
    CCamera_Base(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    CCamera_Base(const CCamera_Base& rhs);
    virtual ~CCamera_Base() = default;


public:
    virtual HRESULT     Initialize_Prototype(); /*원형 객체가 생성될때 부르는 Initialize*/
    virtual HRESULT     Initialize_Copytype(void* pArg); /*사본 객체가 생성될때 부르는 Initialize*/

public:
    virtual void    Free() override;

public:
    virtual void        Bind_ViewProjMatrix();
    CCameraComponent* Get_CameraComp() { return m_pCameraCom; }

protected:
    CCameraComponent* m_pCameraCom = { nullptr };
    

public:
    bool        HasRenderTarget() { return (m_eCameraFlag & CAMERA_FLAG::HAS_RENDERTARGET)==CAMERA_FLAG::HAS_RENDERTARGET; }
    
                    //현재 카메라가 소유하고있는 렌더타겟으로 swap하는 함수
    virtual HRESULT        Bind_RenderTarget();

                    //Bind_RenderTarget()이후 사용 , 카메라의 렌더타겟에서 원래 백버퍼의 렌더타겟으로 교체
    virtual HRESULT        UnBind_RenderTarget();

    virtual HRESULT         Clear_RenderTargetView(const _float4* pClearColor);

 
    
protected:
    CShader* m_pMainShader = { nullptr };
    ComPtr<ID3DX11EffectMatrixVariable>     m_GlobalViewProj;
    
    CAMERA_TYPE                     m_eCameraType = CAMERA_TYPE::END;
    CAMERA_FLAG                     m_eCameraFlag = CAMERA_FLAG::END;
};

NS_END