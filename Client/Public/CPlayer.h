#pragma once
#include "CModelObject.h"
#include "VertexData.h"

/*움직임과 상태제어만 한다.
상태에 따른 애니메이션처리는 Body객체가 한다.*/
namespace Engine
{
    class CInput_Manager;
}
NS_BEGIN(Client)


class CPlayer :
    public CModelObject
{
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

    void        Event_Input(_float  fTimeDelta);

    void        Move_Input(_float fTimeDelta);
   
public:
    static CPlayer* Create(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pDeviceContext);
    virtual CGameObject* Clone(void* pArg) override;
    virtual void Free() override;

public:
    virtual HRESULT     Ready_PartObjects(void* pArg);


private:
    bool bPressed = false;
    bool m_bMove = true;

    CInput_Manager*         m_pInputManager = nullptr;
    int                     iHp = 5;

};

NS_END