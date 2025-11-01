#pragma once
#include "CBase.h"
#include "Engine_Enum.h"

NS_BEGIN(Engine)

class CCamera_Base;
class CGameObject;
class CShader;
class CConstantBuffer;

class CCamera_Manager :
    public CBase
{
private:
    CCamera_Manager(ComPtr<ID3D11Device>  _pDevice, ComPtr<ID3D11DeviceContext>  _pContext);
    virtual ~CCamera_Manager() = default;
private:
    HRESULT         Initialize();
public:
    //카메라 등록 및 제거..
    void        RegisterCamera(CAMERA_TYPE eType, CGameObject* pObj);
    void        UnRegisterCamera(CAMERA_TYPE eType);
    void        Set_MainCamera(CAMERA_TYPE eType);

    //뷰,투영행렬가져오기
  /*  const _float4x4& GetViewMatrix(CAMERA_TYPE eType) const;
    const _float4x4& GetProjMatrix(CAMERA_TYPE eType) const;
    const _matrix GetMulViewProjMatrix(CAMERA_TYPE eType) const;
    void    Bind_ViewProjMatrix(CAMERA_TYPE eType);*/
    

    //메인카메라가져오기
    CCamera_Base* Get_MainCamera()  { return m_pMainCamera; }

    //카메라 가져오기
    CCamera_Base*   Find_Camera(CAMERA_TYPE eType);
    
    //메인카메라의 뷰,투영행렬 관련
   // _matrix Get_Main_MulViewProjMatrix();
    //void    Bind_Main_ViewProjMatrix() const;


    ////현재 Render함수에서 호출되고있는 카메라의 뷰,투영가져오기
    //const _float4x4& Get_RenderCamera_ViewMatrix() const;
    //const _float4x4& Get_RenderCamera_GetProjMatrix() const;
    //const _matrix  Get_RenderCamera_GetMulViewProjMatrix() const;
public:
    void        Update_Cameras(_float fTimeDelta);
    void        LateUpdate_Cameras(_float fTimeDelta);
    void        Render_Cameras();

public:
    CShader* Get_RenderShader() { return m_pRenderShader; }
    const string& Get_RenderPassName() { return m_pRenderPassName; }
    CCamera_Base* Get_RenderCamera() { return m_pRenderCamera; }

public:
    static CCamera_Manager* Create(ComPtr<ID3D11Device>  _pDevice, ComPtr<ID3D11DeviceContext>  _pContext);
    virtual void        Free() override;

private:
    array<CCamera_Base*, ENUM_TO_UINT(CAMERA_TYPE::END)> m_Cameras;
    CCamera_Base*        m_pMainCamera = nullptr; //현재 활성중인 원근투영카메라
    CCamera_Base*       m_pRenderCamera = nullptr;

private:
    _float4x4       g_Identityfloat4x4;
    _matrix         g_IdentityMatrix;

private:
    CShader* m_pRenderShader = nullptr;
    string  m_pRenderPassName = "";

private:
    ComPtr<ID3D11Device>            m_pDevice;
    ComPtr<ID3D11DeviceContext>     m_pContext;

};
NS_END

