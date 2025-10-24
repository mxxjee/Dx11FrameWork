#pragma once
#include "CBase.h"

NS_BEGIN(Engine)
class CLight :
    public CBase
{
private:
    CLight();
    virtual ~CLight() = default;

public:
    const LIGHT_DESC* Get_LightDesc() const { return &m_LightDesc; }
    bool            IsActive()              { return m_bActive; }
public:
    HRESULT         Initialize(const LIGHT_DESC& LightDesc);

private:
    LIGHT_DESC      m_LightDesc{};

public:
    static CLight* Create(const LIGHT_DESC& LightDesc);
    virtual         void    Free() override;

private:
    bool            m_bActive = true;
};
NS_END

