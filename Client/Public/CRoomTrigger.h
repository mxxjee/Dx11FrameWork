#pragma once
#include "CTrigger_Box.h"

NS_BEGIN(Client)
class CRoomTrigger :
    public CTrigger_Box
{
public:
    struct RoomTriggerDesc :public CTrigger_Box::TriggerBoxDesc
    {
        string m_nextKey="";
    };
protected:
    CRoomTrigger(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    CRoomTrigger(const CRoomTrigger& rhs);
    virtual ~CRoomTrigger() = default;

public:
    virtual HRESULT     Initialize_Prototype(); /*원형 객체가 생성될때 부르는 Initialize*/
    virtual HRESULT     Initialize_Copytype(void* pArg); /*사본 객체가 생성될때 부르는 Initialize*/


public:
    virtual     void    OnCollisionEnter(_uint iGroup, CCollider_Base* pOther);
    virtual     void    OnCollisionStay(_uint iGroup, CCollider_Base* pOther);
    virtual     void    OnCollisionExit(_uint iGroup, CCollider_Base* pOther);

private:
    string      m_NextRoomKey = "";

    

};

NS_END
