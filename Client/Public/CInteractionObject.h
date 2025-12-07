#pragma once
#include "CContainerObject.h"

namespace Engine
{
    class CCollider_Base;
    class CBody;

}

NS_BEGIN(Client)
class CInteractionObject :
    public CContainerObject
{
public:
    typedef struct Interaction_DESC : CGameObject::GAMEOBJECT_DESC
    {
        _uint                   eInteractionType = 0;
        _uint                   eRenderGroup = 0;
        _uint                   eCollisionGroup = 0;

        void*                   pColliderComp = nullptr;
        _wstring                ModelName;
        void*                   BodyDesc = nullptr;
        bool                    bAnimated = false;      //애님을 사용하는 메쉬인지

    }Interaction_DESC;


protected:
    CInteractionObject(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    CInteractionObject(const CInteractionObject& rhs);
    virtual ~CInteractionObject() = default;

public:
    HRESULT     Initialize_Prototype(); /*원형 객체가 생성될때 부르는 Initialize*/
    virtual HRESULT     Initialize_Copytype(void* pArg); /*사본 객체가 생성될때 부르는 Initialize*/

    void        Update_Priority(_float fTimeDelta);
    void        Update(_float fTimeDelta);
    void        Update_Late(_float fTimeDelta);
    void        Update_Render(_float fTimeDelta);


    virtual HRESULT Render();

public:
    static CInteractionObject* Create(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pDeviceContext);
    // CGameObject을(를) 통해 상속됨
    virtual CGameObject* Clone(void* pArg) override;
    virtual     void    Free() override;

public:
    virtual HRESULT     Ready_Components(void* pArg);
    virtual HRESULT     Ready_PartObjects(void* pArg);


private:
    _uint                   m_eInteractionType = 0; //인터렉션타입
    CCollider_Base*         m_pCollider = nullptr; //    콜라이더 정보
    CBody*                  m_pBody = nullptr;
    _uint                   m_eRenderGroup = 0;


private:
    _uint       m_iState = 0;

};
NS_END

