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
        bool                m_bCreateNewRenderTarget = false;
        CAMERA_TYPE eCameraType= CAMERA_TYPE::END;
        CAMERA_FLAG eCameraFlag = CAMERA_FLAG::NONE;

        CShader* pMainShader = nullptr;
        string  PassName = "";


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
    virtual void            Bind_ViewProjMatrix();
    CCameraComponent*       Get_CameraComp() { return m_pCameraCom; }
    
    const CAMERA_TYPE&      Get_CameraType() { return m_eCameraType; }
    const CAMERA_FLAG&      Get_CameraFlag() { return m_eCameraFlag; }

    const RENDER_TARGET&    Get_RenderTarget() const { return m_tRenderTarget; }

protected:
    CCameraComponent* m_pCameraCom = { nullptr };
    

public:
    virtual HRESULT     Create_RenderTagetview(bool bCreateRenderTarget);
                    
    //현재 카메라가 소유하고있는 렌더타겟으로 swap하는 함수
    virtual HRESULT        Bind_RenderTarget();
    virtual HRESULT        UnBind_RenderTarget();
    virtual HRESULT         Clear_RenderTargetView(const _float4* pClearColor);

 
    CShader* Get_Shader()       { return m_pMainShader; }
    const string& Get_PassName() { return m_PassName; }

public:
                //이 카메라는 무슨무슨 렌더그룹을 렌더할건지,기본값 everything
    void        Set_RenderMask(_uint eGroup, bool bRender) { m_RenderMask[eGroup] = bRender; }
    void        Set_RenderAllRenderMask(bool bRender);
    const       vector<bool>& Get_RenderMask() { return m_RenderMask; }


protected:
    CShader* m_pMainShader = { nullptr };       //카메라가 오브젝트 렌더 시 사용할 쉐이더.
    string  m_PassName = "";                    //이용할 패스.

    ComPtr<ID3DX11EffectMatrixVariable>     m_GlobalViewProj;
    
    CAMERA_TYPE                     m_eCameraType = CAMERA_TYPE::END;
    CAMERA_FLAG                     m_eCameraFlag = CAMERA_FLAG::END;

    bool                            m_bUseNewRenderTarget = false;
    
    RENDER_TARGET                   m_tRenderTarget;        //모든 카메라는 렌더타겟을 갖는다.하지만 특수한경우에만 새로운 렌더타겟을 생성한다.


protected:
    vector<bool>                    m_RenderMask;
    int                             m_RenderGroupMax = 0;
};

NS_END