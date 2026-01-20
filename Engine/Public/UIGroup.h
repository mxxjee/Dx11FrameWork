#pragma once

#include "Engine_Define.h"

NS_BEGIN(Engine)
struct ENGINE_DLL UIGroup
{
public:
    _wstring Key;       //그룹이름
    UMap<size_t ,class CGameObject*>        Objects;        //그룹요소들
    bool        isActive = true;                //활성화 여부
    

public:
    void push_back(class CGameObject* pObj);
    void Release();
    bool    Is_Active() { return isActive; }
    class CGameObject* Find(const wstring& Key);
 
};

NS_END