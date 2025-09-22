#pragma once
#include "CBase.h"

NS_BEGIN(Engine)

class CPrototype_Manager :
    public CBase
{

private:
    CPrototype_Manager();
    virtual ~CPrototype_Manager() = default;

public:
    HRESULT         Initialize(_uint  iNumLevels);
    HRESULT         Add_Prototype(_uint m_iNumLevel, const _wstring& strPrototypeTag, CBase* _base);
    CBase*          Clone_Prototype(PROTOTYPE eType,_uint iNumLevel, const _wstring& strPrototypeTag, void* pArg = nullptr);

private:
    CBase* Find_Prototype(_uint m_iNumLevel, const _wstring tag);

public:
    static CPrototype_Manager*      Create(_uint iNumLevels);
    virtual void        Free();
private:
    _uint            m_iNumLevels = {};
    vector<unordered_map<_wstring, CBase*>> m_Prototypes;

};

NS_END