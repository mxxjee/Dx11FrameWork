#pragma once
#include "CPartObject.h"
namespace Engine
{
    class CCollider_Base;
}

NS_BEGIN(Client)
class CIInteractable;
class CInteraction_TriggerBox :
    public CPartObject
{
public:
    typedef struct tagInteractionTriggerBoxDesc : public CPartObject::PARTOBJECT_DESC
    {
        _float3 vCenter = {};//부모로 부터얼마나떨어졌는지
        _float3 vExtents = {};


    }INTERACTION_TRIGGER_DESC;


protected:
    CInteraction_TriggerBox(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    CInteraction_TriggerBox(const CInteraction_TriggerBox& Prototype);
    virtual ~CInteraction_TriggerBox() = default;

public:
    virtual HRESULT     Initialize_Prototype(); /*원형 객체가 생성될때 부르는 Initialize*/
    virtual HRESULT     Initialize_Copytype(void* pArg); /*사본 객체가 생성될때 부르는 Initialize*/

    virtual void        Update_Priority(_float fTimeDelta);
    virtual void        Update(_float fTimeDelta);
    virtual void        Update_Late(_float fTimeDelta);
    virtual void        Update_Render(_float fTimeDelta);

    virtual HRESULT Render();
    
public:
    void        Register_Colliders(_uint iLevelIdx);

private:
    HRESULT         Ready_Component(void* pArg);

public:
    virtual void    Set_Active(bool _b);
    bool            Is_Collision() { return Is_PlayerCollision(); }
    bool            Is_PlayerCollision() const { return !m_PlayerOverlaps.empty(); }
    bool            Is_MonsterCollision() const { return !m_MonsterOverlaps.empty(); }
    void            Set_Size(_float3 vSize);
    virtual     void    PushOut(_float3 vOutPush) {};

protected:
    vector<CCollider_Base*> m_pCollider = { nullptr };

protected:
    const _float4x4* m_pSocketMatrix = { nullptr };

public:
    virtual     void    OnCollisionEnter(_uint iGroup, CCollider_Base* pOther);
    virtual     void    OnCollisionStay(_uint iGroup, CCollider_Base* pOther);
    virtual     void    OnCollisionExit(_uint iGroup, CCollider_Base* pOther);

public:
    static CInteraction_TriggerBox* Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    virtual CGameObject* Clone(void* pArg) override;
    virtual void Free() override;

public:
    CGameObject*    Get_Other() { return Get_PlayerOther(); }
    CGameObject*    Get_PlayerOther() const;
    CGameObject*    Get_MonsterOther() const;

private:
    void            Add_Overlap(std::vector<CCollider_Base*>& Overlaps, CCollider_Base* pOther);
    void            Remove_Overlap(std::vector<CCollider_Base*>& Overlaps, CCollider_Base* pOther);
    void            Clear_Overlaps();

    std::vector<CCollider_Base*> m_PlayerOverlaps;
    std::vector<CCollider_Base*> m_MonsterOverlaps;


};

NS_END
