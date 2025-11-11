#pragma once
#include "CModelObject.h"

NS_BEGIN(Client)
class CMonster :
    public CModelObject
{
public:
    typedef struct MonsterDesc : public CModelObject::MODELOBJECT_DESC
    {
        int MaxHp;
        int iAttack;

    }MONSTER_DESC;
protected:
    CMonster(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    CMonster(const CMonster& rhs);
    virtual ~CMonster() = default;

public:
    virtual HRESULT     Initialize_Prototype(); /*원형 객체가 생성될때 부르는 Initialize*/
    virtual HRESULT     Initialize_Copytype(void* pArg); /*사본 객체가 생성될때 부르는 Initialize*/

    virtual void        Update_Priority(_float fTimeDelta);
    virtual void        Update(_float fTimeDelta);
    virtual void        Update_Late(_float fTimeDelta);
    virtual void        Update_Render(_float fTimeDelta) override;


    virtual HRESULT Render();
private:
    HRESULT     Ready_Resource(void* pArg);

private:
    virtual   HRESULT     Ready_PartObjects(void* pArg);

public:
    static CMonster* Create(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pDeviceContext);
    virtual CGameObject* Clone(void* pArg) override;
    virtual void Free() override;


public:
    int             iHp;
    int             iMaxHp;
    int             iAttack = 10;

    int                     m_iAnimIdx = 0;

};

NS_END