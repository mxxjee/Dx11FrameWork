#pragma once
#include "CComponent.h"

NS_BEGIN(Engine)

class ENGINE_DLL CCollider_Base:
    public CComponent
{
public:
    typedef struct tagColliderDesc :CComponent::COMPONENT_DESC
    {
        _uint m_eColGroup = 0;
        void* m_BoundingDesc = nullptr;

    }COLLIDER_DESC;
protected:
    CCollider_Base(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    CCollider_Base(const CCollider_Base& Prototype);
    virtual ~CCollider_Base() = default;

#ifdef DEBUG

    virtual     HRESULT Render();

#endif // DEBUG

public:
    virtual         HRESULT     Initialize_Prototype();
    virtual         HRESULT     Initialize_Copytype(void* pArg);

public:
    virtual      HRESULT   Update_Collider(XMMATRIX    WorldMatrix)=0;
    virtual      bool   Intersects_Ray(_vector origin, _vector rayDir, _float& Dist,class CTransform* pTransform)=0;
    _uint       Get_ColGroup() { return m_eColGroup; }

                //콜라이더끼리충돌
    virtual     bool    Intersect(CCollider_Base* pOther);

    void        OnCollisionEnter(_uint iGroup,CCollider_Base* pOther);
    void        OnCollision(_uint iGroup,CCollider_Base* pOther);
    void        OnCollisionExit(_uint iGroup,CCollider_Base* pOther);

public:
    virtual _float3         Get_MaxBound(_vector vCenter);
    virtual _float3         Get_MinBound(_vector vCenter);
#ifdef _DEBUG
public:
    HRESULT             Render();
private:
    PrimitiveBatch<DirectX::VertexPositionColor>* m_pBatch = { nullptr };
    BasicEffect* m_pEffect = { nullptr };
    ID3D11InputLayout* m_pInputLayout = { nullptr };
#endif // _DEBUG

public:
    virtual CComponent* Clone(void* pArg) = 0;
    virtual void Free() override;

public:
    void        Reset_Collision() { m_isColl = false; }
    void        Set_Active(bool b) { m_bActive = b; }
    bool        Is_Active() { return m_bActive; }
    void        Set_Trigger(bool b) { m_bTrigger = b; }
public:
    COLLIDER_TYPE   Get_Type() { return m_eType; }
    class CBounding*      Get_Bounding() { return m_pBounding; }

protected:
    COLLIDER_TYPE            m_eType;      //콜라이더 타입,어느충돌?
    _matrix                  m_WolrdMatrix;      //오너의 월드매트릭스
    _bool                    m_bActive = true;   //활성화 여부
    _bool                    m_bDebugDraw = false;
    class                    CBounding* m_pBounding = { nullptr };     //생성할 바운딩박스 
    _bool                   m_isColl = { false };           //충돌 했는지 판단
    _uint                    m_eColGroup = 0;

protected:
    bool                    m_bTrigger = true;       //통과여부,false면 충돌시 밀어내기계산
};
NS_END

