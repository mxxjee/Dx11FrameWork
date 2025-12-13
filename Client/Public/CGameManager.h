#pragma once
#include "CBase.h"

namespace Engine
{
    class CGameObject;
    class CLayer;
    class CGameInstance;
    class CBase;
}


NS_BEGIN(Client)
class CPlayer;
class CGameManager :
    public CBase
{
    DECLARE_SINGLETON(CGameManager)

private:
    explicit CGameManager() {};
    virtual ~CGameManager() = default;

public:
    void        Set_MainPlayer(CGameObject* pObj);
    void        Set_MainPlayer(CBase* pObj);

    CPlayer* Get_MainPlayer() { return m_pMainPlayer; }

    void        Set_LastPosition(_float4 vPosition);
    void        Set_LastPosition(_vector vPosition);

    _float4     Get_LastPosition_By_Float4();
    _vector     Get_LastPosition_By_Vector();

    virtual void Free() override;

private:
    CPlayer* m_pMainPlayer=nullptr;
    _float4  vLastPosition;
};
NS_END
