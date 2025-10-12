#pragma once
#include "CBase.h"
#include "Engine_Enum.h"

NS_BEGIN(Engine)

class CCamera_Base;
class CGameObject;

class CCamera_Manager :
    public CBase
{
private:
    HRESULT         Initialize();
public:
    //카메라 등록 및 제거..
    void        RegisterCamera(CAMERA_TYPE eType, CGameObject* pObj);
    void        UnRegisterCamera(CAMERA_TYPE eType);
    void        Set_MainCamera(CAMERA_TYPE eType);

    //뷰,투영행렬가져오기
    const _float4x4& GetViewMatrix(CAMERA_TYPE eType) const;
    const _float4x4& GetProjMatrix(CAMERA_TYPE eType) const;
    const _matrix GetMulViewProjMatrix(CAMERA_TYPE eType) const;
    void    Bind_ViewProjMatrix(CAMERA_TYPE eType);

    //카메라 가져오기
    CCamera_Base*   Find_Camera(CAMERA_TYPE eType);
    CCamera_Base* Get_MainCamera() { return m_pMainCamera; }

    //메인카메라의 뷰,투영행렬 관련
    const _float4x4& Get_Main_ViewMatrix();
    const _float4x4& Get_Main_ProjMatrix();
    _matrix Get_Main_MulViewProjMatrix();
    void    Bind_Main_ViewProjMatrix() const;
public:
    void        Update_Cameras(_float fTimeDelta);
    void        LateUpdate_Cameras(_float fTimeDelta);
public:
    static CCamera_Manager* Create();
    virtual void        Free() override;

private:
    array<CCamera_Base*, ENUM_TO_UINT(CAMERA_TYPE::END)> m_Cameras;
    CCamera_Base*        m_pMainCamera = nullptr; //현재 활성중인 원근투영카메라

private:
    _float4x4       g_Identityfloat4x4;
    _matrix         g_IdentityMatrix;
};
NS_END

