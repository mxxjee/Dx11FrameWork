#pragma once
#include "CBase.h"
#include "UIGroup.h"

/*UI 들의 생명주기는 관리하지않음. -> 이는 objectmanager에서 관리
UI들의 그룹화 + 이벤트를 등록하고, 이벤트를 broadcast해주는 역할*/

NS_BEGIN(Engine)



class CUI_Manager :
    public CBase
{
private:
    CUI_Manager();
    virtual ~CUI_Manager() = default;



public:
    //새로운  UIGroup 구조체를 만들어서 등록
    HRESULT     Register_UIGroup(const UIGroup& Group,const _wstring& Key=L"");


    //이미 매니저에 존재하는 그룹에 추가로 등록
    HRESULT     AddUIToGroup(const _wstring& Key, CGameObject* pGameObject);

    //이벤트 등록.. 
    HRESULT     RegisterEvent(const _wstring& Key, function<void(void*)> _function);
    HRESULT     UnRegisterEvent(const _wstring& Key);


    //이벤트 뿌리기 , pData=전달하고싶은 데이터들
    HRESULT     BroadCastEvent(const _wstring& Key, void* pData);
    
    //그룹단위로 setactive/false처리
    HRESULT        SetActiveGroup(const _wstring& Key, bool bActive);


    UIGroup*            Get_UIGroup(const _wstring Key);
    function<void(void*)> Get_EventFunction(const _wstring& Key);

public:
    static CUI_Manager* Create();
    virtual void        Free() override;

private:
    UMap<size_t, UIGroup*>     m_UIMap;
    UMap<size_t, function<void(void*)>> m_EventMap;


};

NS_END

