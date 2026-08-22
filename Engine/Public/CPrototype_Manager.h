#pragma once
#include "CBase.h"

//컴포넌트 혹은 오브젝트의 원형들을 레벨별로 구분해서 보관

NS_BEGIN(Engine)
class CPrototype_Manager final :
    public CBase
{

private:
    CPrototype_Manager();
    virtual ~CPrototype_Manager() = default;

public:
    HRESULT        Initialize(_uint iNumLevels);
    HRESULT        Add_Prototype(_uint iLevelIndex, const _wstring& strProtoTag, CBase* pPrototype);
    CBase*          Clone_Prototype(PROTOTYPE ePrototypeID, _uint iLevelIndex, const _wstring& strPrototag, void* pArg);      
    void            Clear(_uint iLevel);

private:
    vector<unordered_map<_wstring, CBase*>> m_Prototypes;
    _uint           m_iNumLevels = {};


private:
    CBase* Find_Prototype(_uint iLevelIndex, const _wstring& strProtoTag);

public:
    static CPrototype_Manager* Create(_uint iNumLevels);
    virtual void Free() override;
};

NS_END

