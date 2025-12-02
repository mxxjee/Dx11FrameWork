#pragma once
#include "CBase.h"
#include "CTransform.h"

NS_BEGIN(Engine)
class CCollider_Base;

class ENGINE_DLL CGameObject :
    public CBase
{
public:
    typedef struct tagGameObjectDesc 
    {
        _wstring ObjTag;
        class CGameObject* pTarget = nullptr;
        void* TransformDesc = nullptr;
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
    HRESULT     Ready_Components(void* pArg);
    HRESULT     Ready_Resource(void* pArg);

public:
    HRESULT     Add_Component(COMPONENT_TYPE eType, CComponent* pComp, CComponent** pOut);

public:
    virtual void    Set_Active(bool _b) { m_bActive = _b; }
    void    Set_Target(CGameObject* pObj) { m_pTarget = pObj; }
    void    Set_Tag(const _wstring& Tag) {tag = Tag;}


public:
    const _wstring& Get_Tag() { return tag; }
    CComponent* Get_Component(COMPONENT_TYPE eType);
    CTransform* Get_Transform() const { return m_pTransformCom; }

    bool            Is_Active() { return m_bActive; }

public:
    CGameObject*    Get_Target() { return m_pTarget; }
   
#ifdef _DEBUG
public:
    virtual void            Render_Transform_Imgui();
#endif


public:
    virtual     void    OnCollisionEnter(_uint iGroup, CCollider_Base* pOther) {};
    virtual     void    OnCollisionStay(_uint iGroup, CCollider_Base* pOther) {};
    virtual     void    OnCollisionExit(_uint iGroup, CCollider_Base* pOther){};

protected:
    ComPtr<ID3D11Device> m_pDevice = { nullptr };
    ComPtr<ID3D11DeviceContext> m_pContext = { nullptr };
    class CGameInstance* m_pGameInstance = { nullptr };
    CTransform* m_pTransformCom = { nullptr };


protected:
    _wstring                tag = L"";
    map<COMPONENT_TYPE, class CComponent*>      m_Components;
    bool                                        m_bActive=true;
    CGameObject*                                m_pTarget = { nullptr };

public:
    virtual CGameObject* Clone(void* pArg)=0;
    virtual void    Free() override;


};
NS_END

