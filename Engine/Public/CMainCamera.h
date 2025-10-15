#pragma once
#include "CCamera_Base.h"

namespace Engine
{
    class CPerspectiveCameraComponent;
    class CShader;
}

NS_BEGIN(Engine)
class ENGINE_DLL CMainCamera :
    public CCamera_Base
{
private:
    CMainCamera(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    CMainCamera(const CMainCamera& rhs);
    virtual ~CMainCamera() = default;

public:
    virtual HRESULT     Initialize_Prototype(); /*원형 객체가 생성될때 부르는 Initialize*/
    virtual HRESULT     Initialize_Copytype(void* pArg); /*사본 객체가 생성될때 부르는 Initialize*/

    virtual void        Update_Priority(_float fTimeDelta);
    virtual void        Update(_float fTimeDelta);
    virtual void        Update_Late(_float fTimeDelta);
    virtual void        Update_Render(_float fTimeDelta);

    virtual HRESULT Render();

public:
    //bInit = 초기화용, 첫프레임시 튀는걸 방지한다(바로스냅). 자연스럽게 타겟변경시에는 Set_Target()사용
    void        Set_Target(CGameObject* pTarget,bool bInit=false);
    void        Follow_Target(_float fTimeDelta);
 
public:
    static CMainCamera* Create(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pDeviceContext);
    virtual CGameObject* Clone(void* pArg);
    virtual void    Free() override;


};

NS_END

