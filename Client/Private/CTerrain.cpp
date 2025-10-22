#include "CTerrain.h"
#include "CGameInstance.h"

#include "CVIBuffer_Terrain.h"
#include "CTexture.h"
#include "CShader.h"

#include "Client_Defines.h"


USING(Client)
CTerrain::CTerrain(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    :CTerrain_Base{pDevice,pContext}
{
}

CTerrain::CTerrain(const CTerrain& Prototype)
    : CTerrain_Base(Prototype)
{
}

HRESULT CTerrain::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;


    return S_OK;
}

HRESULT CTerrain::Initialize_Copytype(void* pArg)
{
    if (FAILED(__super::Initialize_Copytype(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components(pArg)))
        return E_FAIL;


    return S_OK;
}

void CTerrain::Update_Priority(_float fTimeDelta)
{
    __super::Update_Priority(fTimeDelta);
}

void CTerrain::Update(_float fTimeDelta)
{
    __super::Update(fTimeDelta);
}

void CTerrain::Update_Late(_float fTimeDelta)
{
    __super::Update_Late(fTimeDelta);
}

void CTerrain::Update_Render(_float fTimeDelta)
{
    __super::Update_Render(fTimeDelta); 
  
}

HRESULT CTerrain::Render()
{
    if (FAILED(__super::Render()))
        return E_FAIL;


    return S_OK;
}

HRESULT CTerrain::Ready_Components(void* pArg)
{

    CComponent* pBuffer_Terrain = dynamic_cast<CVIBuffer_Terrain*>(m_pGameInstance->Clone_Prototype
    (PROTOTYPE::COMPONENT, 0, PROTO_COMPONENT_NAME(L"VIBuffer_Terrain"), pArg));


    if(FAILED(__super::Add_Component(COMPONENT_TYPE::VIBUFFER_TERRAIN,pBuffer_Terrain,(CComponent**)&m_pVIBufferCom)))
        return E_FAIL;

    return S_OK;
}





CTerrain* CTerrain::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
    CTerrain* pInstance = new CTerrain(pDevice, pContext);
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Create :CTerrain ");
        Safe_Release(pInstance);

    }



    return pInstance;
}

CGameObject* CTerrain::Clone(void* pArg)
{
    CTerrain* pInstance = new CTerrain(*this);
    if (FAILED(pInstance->Initialize_Copytype(pArg)))
    {
        MSG_BOX("Failed to Cloned :CTerrain ");
        Safe_Release(pInstance);

    }
    return pInstance;
}

void CTerrain::Free()
{
    __super::Free();
 
}
