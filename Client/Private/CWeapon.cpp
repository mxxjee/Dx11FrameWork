#include "CWeapon.h"
#include "CCollider_Base.h"
#include "CGameInstance.h"
#include "Client_Defines.h"

USING(Client)
CWeapon::CWeapon(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    :CPartObject(pDevice,pContext)
{
}

CWeapon::CWeapon(const CWeapon& Prototype)
    :CPartObject(Prototype)
{
}

HRESULT CWeapon::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CWeapon::Initialize_Copytype(void* pArg)
{
    WEAPON_DESC* pDesc = static_cast<WEAPON_DESC*>(pArg);

    m_pSocketMatrix = pDesc->pSocketMatrix;

    if (FAILED(__super::Initialize_Copytype(pArg)))
        return E_FAIL;





    return S_OK;
}

void CWeapon::Update_Priority(_float fTimeDelta)
{
    __super::Update_Priority(fTimeDelta);
}

void CWeapon::Update(_float fTimeDelta)
{
    _matrix SocketMatrix = XMLoadFloat4x4(m_pSocketMatrix);
    for (size_t i = 0; i < 3; ++i)
        SocketMatrix.r[i] = XMVector3Normalize(SocketMatrix.r[i]);


    //따라가려는 소켓매트릭스 x 원래 parent매트릿그
    _matrix ParentMatrix = SocketMatrix * XMLoadFloat4x4(m_pParentMatrix);


    __super::SetUp_CombinedWorldMatrix(ParentMatrix);
   
}

void CWeapon::Update_Late(_float fTimeDelta)
{
    for(auto& i:m_pCollider)
        i->Update_Collider(XMLoadFloat4x4(&m_CombinedWorldMatrix));


}

void CWeapon::Update_Render(_float fTimeDelta)
{
    m_pGameInstance->Add_RenderObject(ENUM_TO_UINT(RENDERGROUP::NONALPHA), this);

}

HRESULT CWeapon::Render()
{
#ifdef _DEBUG
    if (m_bDrawDebug)
    {
        for (auto& i : m_pCollider)
            i->Render();
    }

#endif
    return S_OK;
}



CWeapon* CWeapon::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
    CWeapon* pInstance = new CWeapon(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CWeapon");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CWeapon::Clone(void* pArg)
{
    CWeapon* pInstance = new CWeapon(*this);

    if (FAILED(pInstance->Initialize_Copytype(pArg)))
    {
        MSG_BOX("Failed to Cloned : CWeapon");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CWeapon::Free()
{
    for (auto& i : m_pCollider)
        Safe_Release(i);

    __super::Free();
  
}
