#pragma once
#include "CGameObject.h"
#include "Engine_Define.h"

struct UIGroup
{
    _wstring Key;       //그룹이름
    UMap<size_t, class CGameObject*>        Objects;        //그룹요소들
    bool        isActive = true;                //활성화 여부

public:
    void push_back(class CGameObject* pObj)
    {
        CheckNull(pObj);
        size_t Hash = hash<wstring>()(pObj->Get_Tag());
        Safe_AddRef(pObj);
        Objects.emplace(Hash, pObj);
    }

    class CGameObject* Find(const wstring& Key)
    {
        size_t Hash = hash<wstring>()(Key);
        auto iter = Objects.find(Hash);
        if (iter != Objects.end())
            return iter->second;

        return nullptr;
    }
};

