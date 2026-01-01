
#include "CNPC.h"
NS_BEGIN(Client)


class CNPC_Fairy :
    public CNPC
{
protected:
    CNPC_Fairy(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    CNPC_Fairy(const CNPC_Fairy& rhs);
    virtual ~CNPC_Fairy() = default;

public:
    HRESULT     Initialize_Prototype(void* pArg); /*NPC는 clone으로 생성하지않는다.*/

    virtual void        Update_Priority(_float fTimeDelta);
    virtual void        Update(_float fTimeDelta);
    virtual void        Update_Late(_float fTimeDelta);
    virtual void        Update_Render(_float fTimeDelta) override;


    virtual HRESULT Render();

public:
    virtual void    Exit_Interaction();

private:
    void        Reigster_AnimNotify();

public:
    static CNPC_Fairy* Create(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pDeviceContex, void* pArg);
    virtual void Free() override;
    bool        m_bEnd = false;


};
NS_END