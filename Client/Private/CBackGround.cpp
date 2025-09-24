#include "CBackGround.h"

USING(Client)

CBackGround::CBackGround(ComPtr<ID3D11Device>& pDevice, ComPtr<ID3D11DeviceContext>& pContext)
    :CUI(pDevice,pContext)
{
}

CBackGround::CBackGround(const CBackGround& rhs)
    :CUI(rhs)
{
}

HRESULT CBackGround::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CBackGround::Initialize_Copytype(void* pArg)
{
    /*°ª¼¼ÆÃ */
    if (FAILED(__super::Initialize_Copytype(pArg)))
        return E_FAIL;

    return S_OK;
}

void CBackGround::Update_Priority(_float fTimeDelta)
{
    __super::Update_Priority(fTimeDelta);
}

void CBackGround::Update(_float fTimeDelta)
{
    __super::Update(fTimeDelta);
}

void CBackGround::Update_Late(_float fTimeDelta)
{
    __super::Update_Late(fTimeDelta);
}

HRESULT CBackGround::Render()
{
    
    return S_OK;
}

CBackGround* CBackGround::Create(ComPtr<ID3D11Device>& pDevice, ComPtr<ID3D11DeviceContext>& pContext)
{
    CBackGround* pInstance = new CBackGround(pDevice, pContext);
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Create :CBackGround ");
        Safe_Release(pInstance);

    }



    return pInstance;
}

CGameObject* CBackGround::Clone(void* pArg)
{
    CBackGround* pInstance = new CBackGround(*this);
    if (FAILED(pInstance->Initialize_Copytype(pArg)))
    {
        MSG_BOX("Failed to Cloned :CBackGround ");
        Safe_Release(pInstance);

    }
    return pInstance;
}

void CBackGround::Free()
{
    __super::Free();

}
