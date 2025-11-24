#include "CMapModel.h"
#include "CShader.h"
#include "CModel.h"
#include "CMeshComponent.h"
#include "CModelObject.h"
#include "CGameInstance.h"
#include "CMeshColliderComponent.h"




CMapModel::CMapModel(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    :CMapObject(pDevice,pContext)
{
}

CMapModel::CMapModel(const CMapModel& rhs)
    : CMapObject(rhs)
{
}

HRESULT CMapModel::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CMapModel::Initialize_Copytype(void* pArg)
{
    /*부모 컴포넌트 값세팅 */
    if (FAILED(__super::Initialize_Copytype(pArg)))
        return E_FAIL;
         
  
    /*내 컴포넌트 값 세팅*/
    if (FAILED(Ready_Resources(pArg)))
        return E_FAIL;

   /* m_pShader = m_pGameInstance->Find_Shader(L"Shader_MapTool");*/

    return S_OK;
}

void CMapModel::Update_Priority(_float fTimeDelta)
{
    __super::Update_Priority(fTimeDelta);
}

void CMapModel::Update(_float fTimeDelta)
{
    __super::Update(fTimeDelta);


}

void CMapModel::Update_Late(_float fTimeDelta)
{
    __super::Update_Late(fTimeDelta);
}

void CMapModel::Update_Render(_float fTimeDelta)
{
    __super::Update_Render(fTimeDelta);
}

HRESULT CMapModel::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    for (auto& Mesh : m_pModel->Get_Meshs())
    {
        /*모든 메쉬를 순회하면서 바인드한다.
           각 메쉬들의 위치와 소유한 메테리얼의 이미지 바인딩.
           이후 메쉬를 그리는 작업*/

        if (Mesh.second)
        {
            Mesh.second->Bind_ShaderResource(m_pShader, "g_DiffuseTexture", aiTextureType::aiTextureType_DIFFUSE);
            Mesh.second->Bind_ShaderResource(m_pShader, "g_SpecularTexture", aiTextureType::aiTextureType_SPECULAR);
            Mesh.second->Bind_ShaderResource(m_pShader, "g_AmbientTexture", aiTextureType::aiTextureType_AMBIENT);

            if (FAILED(m_pShader->Begin(m_passName)))
                return E_FAIL;

            if (FAILED(m_pModel->Render(Mesh.second)))
                return E_FAIL;


        }



    }


    return S_OK;
}

HRESULT CMapModel::Ready_Component(void* pArg)
{
    CheckNullResult(pArg, E_FAIL);
    MAPMODEL_DESC* pModelDesc = static_cast<MAPMODEL_DESC*>(pArg);
    if (pModelDesc)
    {
        CModel::MODEL_DSC* ppModelDesc = static_cast<CModel::MODEL_DSC*>(pModelDesc->modelDesc);
        ppModelDesc->pOwner = this;
        m_pModel = m_pGameInstance->Clone_Model(pModelDesc->modelName, ppModelDesc);

        if (!m_pModel)
            return E_FAIL;
    }

    /*메쉬콜라이더컴포넌트(픽킹검사)*/
    CMeshColliderComponent::COLLIDER_MESH ColliderDesc;
    ColliderDesc.pModel = m_pModel;
    ColliderDesc.pOwner = this;
    ColliderDesc.vScaleOffSet = _float3(10.f, 10.f, 10.f);

    CComponent* pMeshCollider = dynamic_cast<CMeshColliderComponent*>(m_pGameInstance->Clone_Prototype(
        PROTOTYPE::COMPONENT,
        0,
        PROTO_COMPONENT_NAME(L"MeshCollider"),
        &ColliderDesc));

    if (FAILED(Add_Component(
        COMPONENT_TYPE::MESH_COLLIDER,
        pMeshCollider,
        reinterpret_cast<CComponent**>(&pColliderComp)
    )))
        return E_FAIL;


    return S_OK;
}

HRESULT CMapModel::Ready_Resources(void* pArg)
{
    CheckNullResult(pArg, E_FAIL);
    MAPMODEL_DESC* pModel_dsc = static_cast<MAPMODEL_DESC*>(pArg);


    if (m_pModel)
    {
        if (m_pShader)
            Safe_Release(m_pShader);

        m_pShader = m_pGameInstance->Find_Shader(L"Shader_MapTool");
        Safe_AddRef(m_pShader);

    }



    return S_OK;
}

HRESULT CMapModel::Bind_ShaderResources()
{
    if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShader, "g_WorldMatrix")))
        return E_FAIL;


    return S_OK;
}

CMapModel* CMapModel::Create(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pDeviceContext)
{
    CMapModel* pInstance = new CMapModel(_pDevice, _pDeviceContext);
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Create :CMapModel ");
        Safe_Release(pInstance);

    }



    return pInstance;
}

CGameObject* CMapModel::Clone(void* pArg)
{
    CMapModel* pInstance = new CMapModel(*this);
    if (FAILED(pInstance->Initialize_Copytype(pArg)))
    {
        MSG_BOX("Failed to Cloned :CMapModel ");
        Safe_Release(pInstance);

    }
    return pInstance;
}

void CMapModel::Free()
{
    __super::Free();
    Safe_Release(m_pModel);
}
