#pragma once
#include "CComponent.h"
/*중력은 점프할떄, 공중에있을때 enable한다.
*/
NS_BEGIN(Engine)
class ENGINE_DLL CGravity :
    public CComponent
{

private:
    CGravity(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    CGravity(const CGravity& Prototype);
    virtual ~CGravity() = default;

public:
    virtual         HRESULT     Initialize_Prototype();
    virtual         HRESULT     Initialize_Copytype(void* pArg);


public:
    void        Update(_float fTimeDelta);      // 매프레임마다 중력을 적용한 deltaY계산
    float       GetFallDistance(_float fTimeDelta)  const { return m_VelocityY *fTimeDelta; }        //이번 프레임에 떨어진 거리(yDelta)

    bool        IsFalling() { return m_VelocityY < 0.f && !m_bOnGround; }
    void        Set_EnableGravity(bool b) { m_bEnableGravity = b; }
    void        ResetVelocity() { m_VelocityY = 0.f; }
    
public:
    float       GetVelocityY() { return m_VelocityY; }
    bool        IsJumping() { return m_bIsJumping; }
    void        SetJumping(bool b) { m_bIsJumping = b; }
    void        SetOnGround(bool b) { m_bOnGround = b; }
    bool        IsOnGround() { return m_bOnGround; }

public:
    void        Jump(float fPower)
    {
        m_VelocityY = fPower;
        m_bEnableGravity = true;
        m_bOnGround = false;
        m_bIsJumping = true;

    }

    void        Land()
    {
        m_VelocityY = 0;
        m_bEnableGravity = false;
        m_bOnGround = true;
        m_bIsJumping = false;
    }

public:
    static CGravity* Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    virtual CComponent* Clone(void* pArg);
    virtual void Free() override;

private:
    bool        m_bEnableGravity = false;    //중력 사용 여부(공중일떄 동작한다)
    bool        m_bIsJumping = false;       //현재 점프하고있나요?
    bool        m_bOnGround = false;        //착지 여부

    ///////////////////////////////////////////////
    float           m_Gravity = -98.0f;
    float           MaxFallSpeed = -5.f;     //최대 낙하 속도(MAX)
    float           m_VelocityY = 0.f;      //떨어지는 속도

};
NS_END
