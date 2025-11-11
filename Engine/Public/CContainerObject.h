#pragma once
#include "CGameObject.h"
/*CPartObject를 담을 수 있는 클래스..
CPlayer는 이걸 상속받아 만든다.*/

/*파트오브젝트 여러개를 들고 있어주기 위한 기능을 수행*/

NS_BEGIN(Engine)
class ENGINE_DLL CContainerObject :
    public CGameObject
{
protected:
    CContainerObject(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    CContainerObject(const CContainerObject& rhs);
    virtual ~CContainerObject() = default;
    
public:
    virtual HRESULT     Initialize_Prototype(); /*원형 객체가 생성될때 부르는 Initialize*/
    virtual HRESULT     Initialize_Copytype(void* pArg); /*사본 객체가 생성될때 부르는 Initialize*/

    virtual void        Update_Priority(_float fTimeDelta);
    virtual void        Update(_float fTimeDelta);
    virtual void        Update_Late(_float fTimeDelta);
    virtual void        Update_Render(_float fTimeDelta);


    virtual HRESULT Render();

protected:
    UMap<_wstring, class CPartObject*> m_PartObjects;


protected:
    HRESULT         Add_PartObject(_uint iPrototypeLevelIdx, const _wstring& strPrototypeTag, const _wstring& strPartObjTag, void* pArg);

protected:
    class           CPartObject* Find_PartObject(const _wstring& strTag);

public:
    virtual CGameObject* Clone(void* pArg) = 0;
    virtual void Free() override;

};
NS_END

