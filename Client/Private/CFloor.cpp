#include "CFloor.h"
#include "CGameInstance.h"
#include "Client_Defines.h"


USING(Client)

CFloor::CFloor(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    :CQuad(pDevice, pContext)
{
}

CFloor::CFloor(const CFloor& rhs)
    : CQuad(rhs)
{
}

HRESULT CFloor::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CFloor::Initialize_Copytype(void* pArg)
{
    /*부모 컴포넌트 값세팅 */
    if (FAILED(__super::Initialize_Copytype(pArg)))
        return E_FAIL;

    if (FAILED(CreateRasterizerState()))
        return E_FAIL;

    return S_OK;
}

void CFloor::Update_Priority(_float fTimeDelta)
{
    __super::Update_Priority(fTimeDelta);
}

void CFloor::Update(_float fTimeDelta)
{
    __super::Update(fTimeDelta);
    CheckNull(m_pTransformCom);
}

void CFloor::Update_Late(_float fTimeDelta)
{
    __super::Update_Late(fTimeDelta);
}

void CFloor::Update_Render(_float fTimeDelta)
{
    __super::Update_Render(fTimeDelta);
  
}

HRESULT CFloor::Render()
{
    __super::Render();
    m_pContext->RSSetState(RasterizerState.Get());

    return S_OK;
}


HRESULT CFloor::CreateRasterizerState()
{
    D3D11_RASTERIZER_DESC desc = CD3D11_RASTERIZER_DESC(D3D11_DEFAULT);
    desc.CullMode = D3D11_CULL_NONE;

    m_pDevice->CreateRasterizerState(&desc, RasterizerState.GetAddressOf());

    return S_OK;
}

CFloor* CFloor::Create(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pDeviceContext)
{
    CFloor* pInstance = new CFloor(_pDevice, _pDeviceContext);
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Create :CFloor ");
        Safe_Release(pInstance);

    }



    return pInstance;
}

CGameObject* CFloor::Clone(void* pArg)
{
    CFloor* pInstance = new CFloor(*this);
    if (FAILED(pInstance->Initialize_Copytype(pArg)))
    {
        MSG_BOX("Failed to Cloned :CFloor ");
        Safe_Release(pInstance);

    }
    return pInstance;
}

void CFloor::Free()
{
    __super::Free();
}
