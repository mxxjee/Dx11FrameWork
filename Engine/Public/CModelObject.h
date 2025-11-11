#pragma once
#include "CContainerObject.h"


NS_BEGIN(Engine)

//Partojbect:Body를 기본적으로 소유하는 오브젝트

class ENGINE_DLL CModelObject :
    public CContainerObject
{
public:
    /*모델들이 가지는 기본상태값들.. 이거 상속받아서 추가로 세팅*/
    enum MODEL_STATE : UINT32
    {
        NONE,
        IDLE = 1 << 0,
        RUN = 1 << 1,
        ATTACK = 1 << 2,
    };
public:
    typedef struct tagModelObjectDesc : CGameObject::GAMEOBJECT_DESC
    {
        void* BodyDesc = nullptr;//BodyDesc정보
       

    }MODELOBJECT_DESC;

protected:
    CModelObject(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    CModelObject(const CModelObject& rhs);
    virtual ~CModelObject() = default;
    

public:
    virtual HRESULT     Initialize_Prototype(); /*원형 객체가 생성될때 부르는 Initialize*/
    virtual HRESULT     Initialize_Copytype(void* pArg); /*사본 객체가 생성될때 부르는 Initialize*/

    virtual void        Update_Priority(_float fTimeDelta);
    virtual void        Update(_float fTimeDelta);
    virtual void        Update_Late(_float fTimeDelta);
    virtual void        Update_Render(_float fTimeDelta);


    virtual HRESULT Render();

private:
    HRESULT                     Bind_ShaderResources();

public:
    static CModelObject* Create(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pDeviceContext);
    virtual CGameObject* Clone(void* pArg) override;
    virtual void Free() override;
   
public:
    HRESULT     Ready_Components(void* pArg);     
    HRESULT     Ready_PartObjects(void* pArg);


protected:
    _uint                   m_iState = {};


};
NS_END

