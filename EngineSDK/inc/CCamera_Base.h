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
        CAMERA_TYPE eCameraType = CAMERA_TYPE::END;
        CAMERA_FLAG eCameraFlag = CAMERA_FLAG::NONE;


        _float3 vUp = { 0.f,1.f,0.1f };
        _float3 vPosition = { 0.f,0.f,-2.f };
        _float3 vAt = { 0.f,0.f,0.f };

        _float fFovy=90.f;
        _float fNear = 0.1f;
        _float fFar = 1000.f;

        _float fWidth = 1280.f;
        _float fHeight = 720.f;

        _float3 vOffset = {};


        _bool   m_bDynamic = true;

    }CAMERABASE_DESC;

protected:
    CCamera_Base(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    CCamera_Base(const CCamera_Base& rhs);
    virtual ~CCamera_Base() = default;


public:
    virtual HRESULT     Initialize_Prototype(); /*원형 객체가 생성될때 부르는 Initialize*/
    virtual HRESULT     Initialize_Copytype(void* pArg); /*사본 객체가 생성될때 부르는 Initialize*/
    virtual void        Update_Late(_float fTimeDelta);
private:
    HRESULT        Ready_Resource(void* pArg);
public:
    virtual void    Free() override;

public:
    //virtual void            Bind_ViewProjMatrix();
    //CCameraComponent*       Get_CameraComp() { return m_pCameraCom; }

 

protected:
    //pipeline에게 정보업데이트, 3인칭카메라는 따로 정의한다.
    virtual void Update_PipeLine();

public:
    virtual HRESULT     Create_RenderTagetview(bool bCreateRenderTarget);

    //현재 카메라가 소유하고있는 렌더타겟으로 swap하는 함수
    virtual HRESULT        Bind_RenderTarget();
    virtual HRESULT        UnBind_RenderTarget();
    virtual HRESULT         Clear_RenderTargetView(const _float4* pClearColor);


    //CShader* Get_Shader()       { return m_pMainShader; }
    //const string& Get_PassName() { return m_PassName; }

public:
    //이 카메라는 무슨무슨 렌더그룹을 렌더할건지,기본값 everything
    void        Set_RenderMask(_uint eGroup, bool bRender) { m_RenderMask[eGroup] = bRender; }
    void        Set_RenderAllRenderMask(bool bRender);
   
    const       vector<uint8_t>& Get_RenderMask() { return m_RenderMask; }
    uint8_t*   Get_RenderMaskValue(_uint i) { return &m_RenderMask[i]; }
   

    void        Set_Offset(_float3 Offset) { m_vOffset = Offset; }
    void        Set_PrevOffset(_float3 PreOffset) { m_vPreOffSet = PreOffset; }

    void       Set_Fovy(_float Fov) { m_fFovy = Fov; }
    void      Set_Near(float fNear) { m_fNearZ=fNear; }
    void       Set_Far(float fFar) { m_fFarZ=fFar; }

public:
                        //렌더그룹 렌더 전 설정할것들, RTV변경등.
    virtual void        PreRenderGroup(_uint iRenderGroup) {};

                            //렌더그룹 렌더 후 설정할것들. 다시돌려놓기 등.
    virtual void        PostRenderGroup(_uint iRenderGroup) {};

public:
    //카메라의 get함수들
    const CAMERA_TYPE& Get_CameraType() { return m_eCameraType; }
    const CAMERA_FLAG& Get_CameraFlag() { return m_eCameraFlag; }
    const RENDER_TARGET& Get_RenderTarget() const { return m_tRenderTarget; }
    
    const _float3& Get_Offset() { return m_vOffset; }
    const _float3& Get_InitOffset() { return m_vInitOffset; }
    const _float    Get_Fovy() { return m_fFovy; }
    const _float    Get_Near() { return m_fNearZ; }
    const _float    Get_Far() { return m_fFarZ; }
    const _float3& Get_PrevOffset() { return m_vPreOffSet; }
    _float4         Get_CameraPosition();

private:
    void            Make_Planes();

public:
    bool            IsInDistance(const _float3 vCenter);
    void            Set_Distance(_float fDistance) { m_fCulDistance = fDistance; }
public:
    virtual bool IsInFrustum(const _float3& MinBound, const _float3& MaxBound);

private:
    bool        IsOutSidePlane(_uint PlaneDir, const _float3& MinBound, const _float3& MaxBound);
protected:
    CAMERA_TYPE                     m_eCameraType = CAMERA_TYPE::END;
    CAMERA_FLAG                     m_eCameraFlag = CAMERA_FLAG::END;

    bool                            m_bUseNewRenderTarget = false;

    RENDER_TARGET                   m_tRenderTarget;        //모든 카메라는 렌더타겟을 갖는다.하지만 특수한경우에만 새로운 렌더타겟을 생성한다.

protected:
    _float4         m_Planes[ENUM_TO_UINT(PLANE::END)];
    _float3         m_PlaneNormal[ENUM_TO_UINT(PLANE::END)];

protected:
    vector<uint8_t>                    m_RenderMask;
    int                             m_RenderGroupMax = 0;

protected:
    _float			m_fFovy = {};

    _float          m_fWidth = {};
    _float          m_fHeight = {};

    _float			m_fNearZ{}, m_fFarZ{};
    _float3         m_vOffset;
    _float3         m_vInitOffset;
    _float3         m_vPreOffSet;


    bool            m_bPerspective = true;

    float           m_fCulDistance = 50.f;
};

NS_END