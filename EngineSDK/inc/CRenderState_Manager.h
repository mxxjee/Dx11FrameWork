#pragma once
#include "CBase.h"

NS_BEGIN(Engine)
class CRenderState_Manager :
    public CBase
{
private:
    CRenderState_Manager(ComPtr<ID3D11Device>  _pDevice, ComPtr<ID3D11DeviceContext>  _pContext);
    virtual ~CRenderState_Manager() = default;

private:
     HRESULT         Initialize();

public:
    //렌더스테이트 등록
    HRESULT        Register_RenderStates(_uint iRenderGroup, const RenderStates& States);
    const RenderStates& Get_RenderStates(_uint iRenderGroup);

    //셰이더마다 바인딩
    HRESULT             Bind_SamplerState(class CShader* pShader, _uint iRenderGroup);

public:
    static CRenderState_Manager* Create(ComPtr<ID3D11Device>  _pDevice, ComPtr<ID3D11DeviceContext>  _pContext);
    virtual void    Free() override;

private:
    ComPtr<ID3D11Device>                m_pDevice;
    ComPtr<ID3D11DeviceContext>         m_pContext;
    UMap<_uint, RenderStates>            m_RenderStates;

    RenderStates                           m_DefaultRenerState;

};

NS_END