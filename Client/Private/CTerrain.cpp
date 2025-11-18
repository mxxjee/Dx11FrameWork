#include "CTerrain.h"
#include "CGameInstance.h"

#include "CBody.h"

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
    if (FAILED(CGameObject::Initialize_Copytype(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components(pArg)))
        return E_FAIL;

    if (FAILED(Ready_PartObjects(pArg)))
        return E_FAIL;


    return S_OK;
}

void CTerrain::Update_Priority(_float fTimeDelta)
{
    __super::Update_Priority(fTimeDelta);

    m_pBody->Update_Priority(fTimeDelta);
}

void CTerrain::Update(_float fTimeDelta)
{
    __super::Update(fTimeDelta);
    m_pBody->Update(fTimeDelta);
}

void CTerrain::Update_Late(_float fTimeDelta)
{
    __super::Update_Late(fTimeDelta);
    m_pBody->Update_Late(fTimeDelta);
}

void CTerrain::Update_Render(_float fTimeDelta)
{
    __super::Update_Render(fTimeDelta); 
    m_pBody->Update_Render(fTimeDelta);
}

HRESULT CTerrain::Render()
{

   

    return S_OK;
}

HRESULT CTerrain::Ready_Components(void* pArg)
{


    return S_OK;
}

HRESULT CTerrain::Ready_PartObjects(void* pArg)
{
    CheckNullResult(pArg, E_FAIL);
    TERRAINOBJECT_DESC* pTerrainDesc = static_cast<TERRAINOBJECT_DESC*>(pArg);
    if (pTerrainDesc)
    {
        CBody::BODY_DESC* pBodyDesc = static_cast<CBody::BODY_DESC*>(pTerrainDesc->BodyDesc);
        pBodyDesc->pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
        pBodyDesc->ObjTag = tag + L"_Body";


        m_pBody = dynamic_cast<CBody*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_TO_UINT(LEVEL_ID::STATIC), PROTO_OBJ_NAME(L"Body"), pBodyDesc));

    }
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
    Safe_Release(m_pBody);
 
}
