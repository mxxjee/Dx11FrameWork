#include "CInteraction_BigRock.h"
#include "CBody.h"
USING(Client)

CInteraction_BigRock::CInteraction_BigRock(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    :CInteractionObject(pDevice, pContext)
{
}

CInteraction_BigRock::CInteraction_BigRock(const CInteraction_BigRock& rhs)
    : CInteractionObject(rhs)
{
}

HRESULT CInteraction_BigRock::Initialize_Copytype(void* pArg)
{
    if (FAILED(__super::Initialize_Copytype(pArg)))
        return E_FAIL;

    return S_OK;
}

HRESULT CInteraction_BigRock::Ready_PartObjects(void* pArg)
{

    CheckNullResult(pArg, E_FAIL);

    Interaction_DESC* pDesc = static_cast<Interaction_DESC*>(pArg);

    CBody::BODY_DESC pBodyDesc;
    pBodyDesc.eRenderGroup = m_eRenderGroup;
    pBodyDesc.pOwner = this;
    pBodyDesc.modelName = pDesc->ModelName;
    pBodyDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
    pBodyDesc.ObjTag = pDesc->ObjTag + L"_body";

    if (FAILED(__super::Add_PartObject(0, PROTO_OBJ_NAME(L"StaticBody"), L"Part_Body", &pBodyDesc)))
        return E_FAIL;
    m_pBody = dynamic_cast<CBody*>(Find_PartObject(L"Part_Body"));
    if (m_pBody)
        Safe_AddRef(m_pBody);

    return S_OK;
}

void CInteraction_BigRock::Update(_float fTimeDelta)
{
    __super::Update(fTimeDelta);
}


void CInteraction_BigRock::Update_Render(_float fTimeDelta)
{
    __super::Update_Render(fTimeDelta);
}

HRESULT CInteraction_BigRock::Render()
{
    __super::Render();

    return S_OK;
}


bool CInteraction_BigRock::IsInteratable()
{
    return false;
}

void CInteraction_BigRock::Enter_InteractRange()
{
}

void CInteraction_BigRock::Exit_InteractRange()
{
}

void CInteraction_BigRock::Enter_Interaction()
{
}

void CInteraction_BigRock::Stay_Interaction(_float fTimeDelta)
{
}

void CInteraction_BigRock::Exit_Interaction()
{
}

CInteraction_BigRock* CInteraction_BigRock::Create(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pDeviceContext)
{
    CInteraction_BigRock* pInstance = new CInteraction_BigRock(_pDevice, _pDeviceContext);
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Create :CInteraction_BigRock ");
        Safe_Release(pInstance);

    }



    return pInstance;
}

CGameObject* CInteraction_BigRock::Clone(void* pArg)
{
    CInteraction_BigRock* pInstance = new CInteraction_BigRock(*this);
    if (FAILED(pInstance->Initialize_Copytype(pArg)))
    {
        MSG_BOX("Failed to Clone :CInteraction_BigRock ");
        Safe_Release(pInstance);

    }



    return pInstance;
}

void CInteraction_BigRock::Free()
{
    __super::Free();
}
