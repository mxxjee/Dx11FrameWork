#pragma once
#include "CModelObject.h"

NS_BEGIN(Client)
class CPlayer;

class CNPC :
    public CModelObject
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
    bool            CanInteractable();      //거리를통해 상호작용가능한지
    virtual void    EnterInteractRange() {};
    virtual void    OnInteractRange(_float fTimeDelta) { };       //상호작용가능한 범위에잇을떄 계속호출
    virtual void    ExitInteractRange() {};      //ㅓ범위나갔을때 호출


    /// <summary>
    /// ///실제 인터렉션 할때 호출되는함수
    /// </summary>
    virtual void    Start_Interaction() {};
    virtual void    On_Interaction(_float fTimeDelta){};
    virtual void    Exit_Interaction(){};

private:
    HRESULT         Ready_Components(void* pArg);
    virtual HRESULT     Ready_PartObjects(void* pArg);
    HRESULT         Ready_Resource(void* pArg);

private:
    void        Update_InteractionRange(_float fTimeDelta);      //거리에따라서 여부 업데이트
    void        Update_Interaction(_float fTimeDelta);               //실제로 말하고있는중인지.
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
};

NS_END