#pragma once
#include "CNPC.h"

namespace Engine
{
    class CShader;
};

NS_BEGIN(Client)
class CNPC_Kid_Blue :
    public CNPC
{
protected:
    CNPC_Kid_Blue(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    CNPC_Kid_Blue(const CNPC_Kid_Blue& rhs);
    virtual ~CNPC_Kid_Blue() = default;

public:
    HRESULT     Initialize_Prototype(void* pArg); /*NPC는 clone으로 생성하지않는다.*/

    virtual void        Update_Priority(_float fTimeDelta);
    virtual void        Update(_float fTimeDelta);
    virtual void        Update_Late(_float fTimeDelta);
    virtual void        Update_Render(_float fTimeDelta) override;


    virtual HRESULT Render();

public:
    void            Check_Interaction();

public:
    virtual void    Enter_Interaction();
    virtual void    Exit_Interaction();

public:
    static CNPC_Kid_Blue* Create(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pDeviceContex, void* pArg);
    virtual void Free() override;

private:
    bool        m_bCanChangeAnim= false;
    bool        m_bEnd = false;

    float       m_fAlpha = 1.f;

    CShader* pModelShader = nullptr;
};

NS_END