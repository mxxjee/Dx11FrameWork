#pragma once
#include "CContainerObject.h"


NS_BEGIN(Engine)

//Partojbect:Body를 기본적으로 소유하는 오브젝트
class CBody;

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

protected:
    virtual void Set_State(STATE_OP Flag,_uint State);       //스테이트갱신, cur/pre갱신
    virtual void Motion_Change();       //pre!=cur (애니메이션 진입함수)
    virtual void State_Change();        //애니메이션 끝남에 따라 상태처리
private:
    HRESULT                     Bind_ShaderResources();

public:
    static CModelObject* Create(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pDeviceContext);
    virtual CGameObject* Clone(void* pArg) override;
    virtual void Free() override;
   
public:
    HRESULT     Ready_Components(void* pArg);     
    HRESULT     Ready_PartObjects(void* pArg);

public:
    /*Imgui 디버그용..현재 상태와 애니메이션출력*/
    virtual void        Render_CurrentState_Animation();
    string Convert_String_To_Enum(_uint eState);

protected:
    CBody*           m_pBody = nullptr;
protected:
    _uint                   m_iState = {};
    _uint                   m_iPreState{};




};
NS_END

