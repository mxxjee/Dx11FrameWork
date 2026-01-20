#include "UIGroup.h"
#include "CGameObject.h"

void UIGroup::push_back(CGameObject* pObj)
{
    CheckNull(pObj);
    size_t Hash = hash<wstring>()(pObj->Get_Tag());
    Objects.emplace(Hash, pObj);
}

CGameObject* UIGroup::Find(const wstring& Key)
{
    size_t Hash = hash<wstring>()(Key);
    auto iter = Objects.find(Hash);
    if (iter != Objects.end())
        return iter->second;

    return nullptr;
}

void UIGroup::Release()
{
    for (auto& pair : Objects)
    {
        Safe_Release(pair.second);
    }
    
    Objects.clear();
}