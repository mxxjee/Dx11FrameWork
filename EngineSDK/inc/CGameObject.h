#pragma once
#include "CBase.h"
#include "CTransform.h"

NS_BEGIN(Engine)

class ENGINE_DLL CGameObject :
    public CBase
{
public:
    typedef struct tagGameObjectDesc : CComponent::tagComponentDesc
    {
        _wstring ObjTag;

    }GAMEOBJECT_DESC;

protected:
    CGameObject(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    CGameObject(const CGameObject& rhs);
    virtual ~CGameObject() = default;



public:
    virtual HRESULT     Initialize_Prototype(); /*원형 객체가 생성될때 부르는 Initialize*/
    virtual HRESULT     Initialize_Copytype(void *pArg); /*사본 객체가 생성될때 부르는 Initialize*/
    
    virtual void        Update_Priority(_float fTimeDelta);
    virtual void        Update(_float fTimeDelta);
    virtual void        Update_Late(_float fTimeDelta);
    virtual void        Update_Render(_float fTimeDelta);


    virtual HRESULT Render();
    
public:
    const _wstring& Get_Tag() { return tag; }

protected:
    ComPtr<ID3D11Device> m_pDevice = { nullptr };
    ComPtr<ID3D11DeviceContext> m_pContext = { nullptr };
    class CGameInstance* m_pGameInstance = { nullptr };
    CTransform* m_pTransformCom = { nullptr };

protected:
    _wstring                tag = L"";
    map<const _wstring, class CComponent*>      m_Components;

public:
    virtual CGameObject* Clone(void* pArg)=0;
    virtual void    Free() override;




protected:
    TransformData m_transformData;
};
NS_END

