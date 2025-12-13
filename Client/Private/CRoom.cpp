#include "CRoom.h"
#include "CStaticBody.h"

USING(Client)
CRoom::CRoom(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    :CStaticModelObject(pDevice,pContext)
{
}

CRoom::CRoom(const CRoom& rhs)
    : CStaticModelObject(rhs)
{
}

HRESULT CRoom::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CRoom::Initialize_Copytype(void* pArg)
{
    if (FAILED(__super::Initialize_Copytype(pArg)))
        return E_FAIL;

    CRoom::MODELOBJECT_DESC* desc = static_cast<MODELOBJECT_DESC*>(pArg);
    CStaticBody::BODY_DESC* BodyDesc = static_cast<CStaticBody::BODY_DESC*>(desc->BodyDesc);
    BodyDesc->pParentMatrix= m_pTransformCom->Get_WorldMatrixPtr();
    BodyDesc->ObjTag = desc->ObjTag + L"_body";

    if(FAILED(__super::Ready_PartObjects(pArg)))
        return E_FAIL;

    return S_OK;
}

void CRoom::Update_Priority(_float fTimeDelta)
{
    __super::Update_Priority(fTimeDelta);
}

void CRoom::Update(_float fTimeDelta)
{
    __super::Update(fTimeDelta);
}

void CRoom::Update_Late(_float fTimeDelta)
{
    __super::Update_Late(fTimeDelta);
}

void CRoom::Update_Render(_float fTimeDelta)
{
    __super::Update_Render(fTimeDelta);
}

HRESULT CRoom::Render()
{
    
    return S_OK;
}

CRoom* CRoom::Create(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pDeviceContext)
{
    CRoom* pInstance = new CRoom(_pDevice, _pDeviceContext);
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Create :CRoom ");
        Safe_Release(pInstance);

    }



    return pInstance;
}

CGameObject* CRoom::Clone(void* pArg)
{
    CRoom* pInstance = new CRoom(*this);
    if (FAILED(pInstance->Initialize_Copytype(pArg)))
    {
        MSG_BOX("Failed to Cloned :CRoom ");
        Safe_Release(pInstance);

    }
    return pInstance;
}

void CRoom::Free()
{
    __super::Free();
}
