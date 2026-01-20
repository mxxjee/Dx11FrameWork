#pragma once
#include "CBase.h"
#include "Client_Defines.h"

NS_BEGIN(Client)

class CItem_Manager :
    public CBase
{
    DECLARE_SINGLETON(CItem_Manager)
private:
    explicit CItem_Manager() {};
    virtual ~CItem_Manager() = default;


public:
            //아이템 정보 세팅
    HRESULT         Initialize();
    void            Free();


public:
    ITMINFO* Get_ItemInfo(ItemType eType);

private:
    vector<ITMINFO*>        m_ItmInfos;
};

NS_END