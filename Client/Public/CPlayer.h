#pragma once
#include "CGameObject.h"
#include "VertexData.h"

NS_BEGIN(Client)

class CPlayer :
    public CGameObject
{
public:
    typedef struct tagPlayer_DESC : CGameObject::GAMEOBJECT_DESC
    {
        _wstring ImgPath;

    }PLAYER_DESC;


protected:
    CPlayer(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    CPlayer(const CPlayer& rhs);
    virtual ~CPlayer() = default;

public:
    virtual HRESULT     Initialize_Prototype(); /*원형 객체가 생성될때 부르는 Initialize*/
    virtual HRESULT     Initialize_Copytype(void* pArg); /*사본 객체가 생성될때 부르는 Initialize*/

    virtual void        Update_Priority(_float fTimeDelta);
    virtual void        Update(_float fTimeDelta);
    virtual void        Update_Late(_float fTimeDelta);
    virtual void        Update_Render(_float fTimeDelta) override;


    virtual HRESULT Render();

private:
    HRESULT		Initialize_Piepline();
    void        Move_Input(_float fTimeDelta);

private:
    void	CreateGeometry();
    void	CreateInputLayout();
    void	VertexShader();
    void	PixelShader();

    HRESULT		CreateRasterizerState();
    HRESULT		CreateSamplerState();
    HRESULT		CreateBlendState();

private:
    void		Set_IA();
    void		Set_VS();
    void		Set_RS();
    void		Set_PS();
    void		Set_OM();

public:
    static CPlayer* Create(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pDeviceContext);
    virtual CGameObject* Clone(void* pArg) override;
    virtual void Free() override;

protected:
    RenderPipelineResource<VertexTextureData, TransformData> m_Pipeline;

};

NS_END