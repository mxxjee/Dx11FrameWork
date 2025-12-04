#pragma once
#include "CBody.h"
#include "Client_Defines.h"

NS_BEGIN(Client)
class CPlayer_Body :
    public CBody
{
public:
    typedef struct tagCPlayer_BodyDesc : CBody::BODY_DESC
    {
        float* pDamgeRender = nullptr;

    }PLAYER_BODY_DESC;

protected:
    CPlayer_Body(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    CPlayer_Body(const CPlayer_Body& rhs);
    virtual ~CPlayer_Body() = default;

public:
    virtual HRESULT     Initialize_Prototype(); /*원형 객체가 생성될때 부르는 Initialize*/
    virtual HRESULT     Initialize_Copytype(void* pArg); /*사본 객체가 생성될때 부르는 Initialize*/

    virtual void        Update_Priority(_float fTimeDelta);
    virtual void        Update(_float fTimeDelta);
    virtual void        Update_Late(_float fTimeDelta);
    virtual void        Update_Render(_float fTimeDelta);

    virtual HRESULT Render();

private :
    virtual HRESULT         Bind_ShaderResources() override;

private:
    void        Motion_Change();
    HRESULT        Ready_Animation_Speed();
    HRESULT        Ready_Animation_Notify();
    HRESULT        Ready_Animation_Listner();       // 이벤트리스너 등록

public:
    static CPlayer_Body* Create(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pDeviceContext);
    virtual CGameObject* Clone(void* pArg) override;
    virtual void Free() override;


private:
    float*        m_pDamageRender = nullptr;

};
NS_END

