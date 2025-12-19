#include "CModel_Bed.h"
#include "CAnimBody.h"

USING(Client)

CModel_Bed::CModel_Bed(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    :CAnimModelObject(pDevice, pContext)
{
}

CModel_Bed::CModel_Bed(const CModel_Bed& rhs)
    : CAnimModelObject(rhs)
{
}

HRESULT CModel_Bed::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CModel_Bed::Initialize_Copytype(void* pArg)
{
    if (FAILED(__super::Initialize_Copytype(pArg)))
        return E_FAIL;

    CModel_Bed::MODELOBJECT_DESC* desc = static_cast<MODELOBJECT_DESC*>(pArg);
    CAnimBody::ANIMBODY_DESC BodyDesc = {};

    BodyDesc.eRenderGroup = ENUM_TO_UINT(RENDERGROUP::ALPHA);
    BodyDesc.modelName = L"Bed";
    BodyDesc.m_iLevelID = m_iSceneID;
    BodyDesc.pOwner = this;
    BodyDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
    BodyDesc.ObjTag = desc->ObjTag + L"_body";

    desc->BodyDesc = &BodyDesc;
    if (FAILED(__super::Ready_PartObjects(pArg)))
        return E_FAIL;

    return S_OK;
}

void CModel_Bed::Update_Priority(_float fTimeDelta)
{
    __super::Update_Priority(fTimeDelta);
}

void CModel_Bed::Update(_float fTimeDelta)
{
    __super::Update(fTimeDelta);
}

void CModel_Bed::Update_Late(_float fTimeDelta)
{
    __super::Update_Late(fTimeDelta);
}

void CModel_Bed::Update_Render(_float fTimeDelta)
{
    __super::Update_Render(fTimeDelta);
}

HRESULT CModel_Bed::Render()
{

    return S_OK;
}

CModel_Bed* CModel_Bed::Create(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pDeviceContext)
{
    CModel_Bed* pInstance = new CModel_Bed(_pDevice, _pDeviceContext);
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Create :CModel_Bed ");
        Safe_Release(pInstance);

    }



    return pInstance;
}

CGameObject* CModel_Bed::Clone(void* pArg)
{
    CModel_Bed* pInstance = new CModel_Bed(*this);
    if (FAILED(pInstance->Initialize_Copytype(pArg)))
    {
        MSG_BOX("Failed to Cloned :CModel_Bed ");
        Safe_Release(pInstance);

    }
    return pInstance;
}

void CModel_Bed::Free()
{
    __super::Free();
}
