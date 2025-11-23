#pragma once
#include "CModelObject.h"
#include "CIInteractable.h"

NS_BEGIN(Client)
class CPlayer;

class CNPC :
    public CModelObject, public CIInteractable
{
public:
    enum class NPC_STATE
    {
        NONE,WAIT,TALK,END
    };

    typedef struct tagNPC_Desc :CModelObject::MODELOBJECT_DESC
    {
        string ScriptName="";      //파싱할 스크립트이름
        wstring ModelName = L"";
        float TalkRange=10.f;            //플레이어가 이 거리 안으로 들어오면 상호작용가능
        int NPC_ID=0; 
    }NPC_DESC;

protected:
    CNPC(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    CNPC(const CNPC& rhs);
    virtual ~CNPC() = default;

public:
    HRESULT     Initialize_Prototype(void *pArg); /*NPC는 clone으로 생성하지않는다.*/

    virtual void        Update_Priority(_float fTimeDelta);
    virtual void        Update(_float fTimeDelta);
    virtual void        Update_Late(_float fTimeDelta);
    virtual void        Update_Render(_float fTimeDelta) override;


    virtual HRESULT Render();

public:

    ///////////범위감지용//////////
    virtual bool            IsInteratable();      //상호작용조건
    virtual void        Enter_InteractRange();
    virtual void        Stay_InteractRange(_float fTimeDelta);       //상호작용가능한 범위에잇을떄 계속호출
    virtual void        Exit_InteractRange();      //ㅓ범위나갔을때 호출


    /// ///실제 인터렉션 할때 호출되는함수

    virtual void    Enter_Interaction();
    virtual void    Stay_Interaction(_float fTimeDelta);
    virtual void    Exit_Interaction();

    virtual _int	Get_Interaction_Priority() { return InteractionType::NPC; }//우선순위 젤높음
private:
    HRESULT         Ready_Components(void* pArg);
    virtual HRESULT     Ready_PartObjects(void* pArg);
    HRESULT         Ready_Resource(void* pArg);

public:
    static CNPC* Create(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pDeviceContex, void* pArg) {};
    virtual CGameObject* Clone(void* pArg) override;
    virtual void Free() override;

protected:
    NPC_DESC        m_NpcDesc;
    CPlayer*        m_pPlayer = nullptr;


protected:
    bool        m_bInteractable = false;        //상호작용가능여부
    bool        m_bTalking = false;
    float       m_fTime = 0.f;//임시로 Exit조건 시간으로두기

};

NS_END