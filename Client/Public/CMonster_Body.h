#pragma once
#include "CBody.h"
#include "CMonster.h"

NS_BEGIN(Client)
class CMonster_Body :
    public CBody
{
public:
    typedef struct tagMonsterBodyDesc : CBody::BODY_DESC
    {
        MONSTER_ACTION_CONTORL* pActionControl = nullptr;

    }MONSTER_BODY_DESC;

protected:
    CMonster_Body(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    CMonster_Body(const CMonster_Body& rhs);
    virtual ~CMonster_Body() = default;

public:
    virtual HRESULT     Initialize_Prototype(); /*원형 객체가 생성될때 부르는 Initialize*/
    virtual HRESULT     Initialize_Copytype(void* pArg); /*사본 객체가 생성될때 부르는 Initialize*/

    virtual void        Update_Priority(_float fTimeDelta);
    virtual void        Update(_float fTimeDelta);
    virtual void        Update_Late(_float fTimeDelta);
    virtual void        Update_Render(_float fTimeDelta);

    virtual HRESULT Render();
public:
    static CMonster_Body* Create(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pDeviceContext);
    virtual CGameObject* Clone(void* pArg) override;
    virtual void Free() override;

private:
            //피격효과 추가
    virtual HRESULT         Bind_ShaderResources() override;


public:
            //State에서 사용할 animKey들을 저장.(몬스터들은 종류가많아서.)
     void Register_AnimKey(CMonster::MONSTER_BASE_STATE first, const _wstring& second);
     _wstring Get_AnimKey(CMonster::MONSTER_BASE_STATE first);

public:
    UMap<CMonster::MONSTER_BASE_STATE, _wstring>        m_AnimKeys;


private:    
            //부모 monster로부터 참조받음
    MONSTER_ACTION_CONTORL* m_pActionControl = nullptr; 

};
NS_END

