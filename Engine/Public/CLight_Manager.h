#pragma once
#include "CBase.h"

NS_BEGIN(Engine)
class CLight_Manager :
    public CBase
{
private:
    CLight_Manager();
    virtual ~CLight_Manager() = default;


public:
    const       LIGHT_DESC* Get_LightDesc(_uint iIndex);
    
    HRESULT         Bind_Lights(class CShader* pShader);

public:
    HRESULT         Add_Light(const LIGHT_DESC& LightDesc);

public:
    static CLight_Manager* Create();
    virtual void Free() override;

public:
    list<class CLight*>     m_Lights;
};
NS_END

