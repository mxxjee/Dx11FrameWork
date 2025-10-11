#pragma once
#include "CGameObject.h"

namespace Engine
{
    class CShader;
}

NS_BEGIN(Engine)


class ENGINE_DLL CCamera_Base :
    public CGameObject
{
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
    virtual void        Bind_ViewProjMatrix()=0;

protected:
    CShader* m_pMainShader = { nullptr };
};

NS_END