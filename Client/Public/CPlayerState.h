#pragma once
#include "CBase.h"

NS_BEGIN(Client)
class CPlayerState:
    public CBase
{
protected:
    CPlayerState();
    virtual ~CPlayerState() = default;


public:
    virtual void Free();

};

NS_END

