#include "CPartObject.h"

CPartObject::CPartObject(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    :CGameObject(pDevice,pContext)
{
}

CPartObject::CPartObject(const CPartObject& Prototype)
    :CGameObject(Prototype)
{
}

HRESULT CPartObject::Initialize_Prototype()
{
    
    return S_OK;
}

HRESULT CPartObject::Initialize_Copytype(void* pArg)
{
    PARTOBJECT_DESC* pDesc = static_cast<PARTOBJECT_DESC*>(pArg);
    m_pParentMatrix = pDesc->pParentMatrix;
    m_pOwner = pDesc->pOwner;
        
    if(FAILED(__super::Initialize_Copytype(pArg)))
        return E_FAIL;

    return S_OK;
}

void CPartObject::Update_Priority(_float fTimeDelta)
{
}

void CPartObject::Update(_float fTimeDelta)
{
}

void CPartObject::Update_Late(_float fTimeDelta)
{
}

void CPartObject::Update_Render(_float fTimeDelta)
{
}

HRESULT CPartObject::Render()
{
    return S_OK;
}

void CPartObject::SetUp_CombinedWorldMatrix(_fmatrix ParentMatrix)
{
    //자신의 월드행렬 * 부모월드행렬
    XMStoreFloat4x4(&m_CombinedWorldMatrix,
        XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr()) * ParentMatrix);

}


void CPartObject::Free()
{
    __super::Free();
}
