#pragma once
#include "CContainerObject.h"


NS_BEGIN(Engine)

//Partojbect:Body를 기본적으로 소유하는 오브젝트
class CBody;

class ENGINE_DLL CModelObject :
    public CContainerObject
{

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
    virtual HRESULT     Ready_Components(void* pArg);     
    virtual HRESULT     Ready_PartObjects(void* pArg);
    virtual CBody* Get_Body() { return m_pBody;}

protected:
    CBody*           m_pBody = nullptr;





};
NS_END

