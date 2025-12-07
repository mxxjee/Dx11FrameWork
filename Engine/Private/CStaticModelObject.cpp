#include "CStaticModelObject.h"
#include "CModel.h"
#include "CBody.h"
#include "CStaticBody.h"

CStaticModelObject::CStaticModelObject(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    :CModelObject(pDevice,pContext)
{
}

CStaticModelObject::CStaticModelObject(const CStaticModelObject& rhs)
    :CModelObject(rhs)
{
}

HRESULT CStaticModelObject::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CStaticModelObject::Initialize_Copytype(void* pArg)
{
    if (FAILED(__super::Initialize_Copytype(pArg)))
        return E_FAIL;

    return S_OK;
}

void CStaticModelObject::Update_Priority(_float fTimeDelta)
{
    __super::Update_Priority(fTimeDelta);
}

void CStaticModelObject::Update(_float fTimeDelta)
{
    __super::Update(fTimeDelta);
}


void CStaticModelObject::Update_Late(_float fTimeDelta)
{
    __super::Update_Late(fTimeDelta);
}

void CStaticModelObject::Update_Render(_float fTimeDelta)
{
    __super::Update_Render(fTimeDelta);
}


HRESULT CStaticModelObject::Render()
{
    return S_OK;
}

CStaticModelObject* CStaticModelObject::Create(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pDeviceContext)
{
    CStaticModelObject* pInstance = new CStaticModelObject(_pDevice, _pDeviceContext);
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Create :CStaticModelObject ");
        Safe_Release(pInstance);

    }



    return pInstance;
}

CGameObject* CStaticModelObject::Clone(void* pArg)
{
    CStaticModelObject* pInstance = new CStaticModelObject(*this);
    if (FAILED(pInstance->Initialize_Copytype(pArg)))
    {
        MSG_BOX("Failed to Cloned :CStaticModelObject ");
        Safe_Release(pInstance);

    }
    return pInstance;
}

void CStaticModelObject::Free()
{
    __super::Free();
}

CBody* CStaticModelObject::Get_Body()
{
    return m_pStaticBody;
}
