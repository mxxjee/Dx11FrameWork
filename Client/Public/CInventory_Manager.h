#pragma once
#include "CBase.h"



NS_BEGIN(client)
class CInventory_Manager :
    public CBase
{
    DECLARE_SINGLETON(CInventory_Manager);
private:
    explicit CInventory_Manager();
    virtual ~CInventory_Manager();


public:
    bool        Is_Get_Feather() { return m_bFeather; }
    void        Set_Feather(bool b) { m_bFeather = b; }


public:
    void            Free() override;

private:
    bool        m_bFeather = false;
};
NS_END

