#pragma once
#include "CModelObject.h"
#include "VertexData.h"

/*움직임과 상태제어만 한다.
상태에 따른 애니메이션처리는 Body객체가 한다.*/
namespace Engine
{
    class CBody;
    class CInput_Manager;
    class CNavigation;
}
NS_BEGIN(Client)


class CPlayer :
    public CModelObject
{
private:
    typedef struct tagPlayerInput
    {
        bool m_bisAttack = false;
        bool m_bisMove = false;
        bool m_bisShield = false;

    }PLAYER_INPUT;
protected:
    CPlayer(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    CPlayer(const CPlayer& rhs);
    virtual ~CPlayer() = default;

public:
    virtual HRESULT     Initialize_Prototype(); /*원형 객체가 생성될때 부르는 Initialize*/
    virtual HRESULT     Initialize_Copytype(void* pArg); /*사본 객체가 생성될때 부르는 Initialize*/

    virtual void        Update_Priority(_float fTimeDelta);
    virtual void        Update(_float fTimeDelta);
    virtual void        Update_Late(_float fTimeDelta);
    virtual void        Update_Render(_float fTimeDelta) override;


    virtual HRESULT Render();

private:
    void        Update_Input(_float fTimeDelta);

    bool        Event_Input(_float  fTimeDelta);

    bool        Move_Input(_float fTimeDelta);


    virtual void        Motion_Change();
    virtual void        State_Change();
   
public:
    static CPlayer* Create(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pDeviceContext);
    virtual CGameObject* Clone(void* pArg) override;
    virtual void Free() override;

public:
    virtual HRESULT     Ready_Components(void* pArg);
    virtual HRESULT     Ready_PartObjects(void* pArg);
    void                Reserve_Animation_To_Body(_wstring AnimKey, bool bNextAnimLoop);


private:
    CNavigation* m_pNavigationCom = { nullptr };
    const PLAYER_INPUT* Get_Input() { return &m_Input; }
private:
    bool bPressed = false;
    bool m_bMove = true;
    bool m_bActionInput = false;

    CInput_Manager*         m_pInputManager = nullptr;
    int                     iHp = 5;

private:
    PLAYER_INPUT        m_Input;

};

NS_END