#pragma once
#include "CBase.h"

/*임시, 이렇게 구조체를 사용할거싱ㅁ*/
class CTransform
{
public:
    typedef struct tagTransformDesc
    {
        _float3 m_vLocalPosition = {};
        _float3 m_vLocalScale = {};
        _float3 m_vLocalRotation = {};
        
        _float  m_fSpeed = {};

    }TRANSFORM_DESC;
};
NS_BEGIN(Engine)
class ENGINE_DLL CGameObject :
    public CBase
{
public:
    typedef struct tagGameObjectDesc : public CTransform::TRANSFORM_DESC
    {
        _wstring ObjTag = L"";

    }GAMEOBJECT_DESC;

protected:
    CGameObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CGameObject(const CGameObject* rhs);
    virtual ~CGameObject() = default;



public:
    virtual HRESULT     Initialize_Prototype(); /*원형 객체가 생성될때 부르는 Initialize*/
    virtual HRESULT     Initialize_Copytype(void *pArg); /*사본 객체가 생성될때 부르는 Initialize*/
    
    virtual void        Update_Priority(_float fTimeDelta);
    virtual void        Update(_float fTimeDelta);
    virtual void        Update_Late(_float fTimeDelta);


    virtual HRESULT Render();
    

protected:
    ComPtr<ID3D11Device> m_pDevice = { nullptr };
    ComPtr<ID3D11DeviceContext> m_pContext = { nullptr };

protected:
    _wstring                tag = L"";
    

public:
    virtual CGameObject* Clone(void* pArg)=0;
    virtual void    Free() override;

};
NS_END

