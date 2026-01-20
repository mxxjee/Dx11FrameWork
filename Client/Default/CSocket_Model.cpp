#include "CSocket_Model.h"
#include "CModel.h"
#include "CShader.h"
#include "CMeshComponent.h"
#include "CGameInstance.h"
#include "Client_Defines.h"


USING(Client)
CSocket_Model::CSocket_Model(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    :CPartObject(pDevice,pContext)
{
}

CSocket_Model::CSocket_Model(const CSocket_Model& Prototype)
    : CPartObject(Prototype)
{
}

HRESULT CSocket_Model::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;


    return S_OK;
}

HRESULT CSocket_Model::Initialize_Copytype(void* pArg)
{
    if (FAILED(__super::Initialize_Copytype(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Component(pArg)))
        return E_FAIL;

 
    return S_OK;
}

void CSocket_Model::Update_Priority(_float fTimeDelta)
{
    __super::Update_Priority(fTimeDelta);
}

void CSocket_Model::Update(_float fTimeDelta)
{
  
    CheckNull(m_pSocketMatrix);
    _matrix SocketMatrix = XMLoadFloat4x4(m_pSocketMatrix);
    for (size_t i = 0; i < 3; ++i)
        SocketMatrix.r[i] = XMVector3Normalize(SocketMatrix.r[i]);


    //따라가려는 소켓매트릭스 x 원래 parent매트릿그
    _matrix ParentMatrix = SocketMatrix * XMLoadFloat4x4(m_pParentMatrix);


    __super::SetUp_CombinedWorldMatrix(ParentMatrix);
}

void CSocket_Model::Update_Late(_float fTimeDelta)
{
    __super::Update_Late(fTimeDelta);
}

void CSocket_Model::Update_Render(_float fTimeDelta)
{
    m_pGameInstance->Add_RenderObject(ENUM_TO_UINT(RENDERGROUP::NONALPHA), this);
}


HRESULT CSocket_Model::Render()
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
            Mesh.second->Bind_ShaderResource(m_pShader, "g_NoramlTexture", aiTextureType::aiTextureType_NORMALS);
            Mesh.second->Bind_ShaderResource(m_pShader, "g_SpecularTexture", aiTextureType::aiTextureType_SPECULAR);
            Mesh.second->Bind_ShaderResource(m_pShader, "g_AmbientTexture", aiTextureType::aiTextureType_AMBIENT);

            if (FAILED(m_pShader->Begin(Mesh.second->Get_PassName())))
                return E_FAIL;

            if (FAILED(m_pModel->Render(Mesh.second)))
                return E_FAIL;


        }



    }



    return S_OK;
}

HRESULT CSocket_Model::Ready_Component(void* pArg)
{
    SOKET_MODEL_DESC* pDesc = static_cast<SOKET_MODEL_DESC*>(pArg);
    m_pSocketMatrix = pDesc->pSocketMatrix;


    CModel::tagModelDesc pModelDesc;
    pModelDesc.pOwner = this;
    m_pModel = m_pGameInstance->Clone_Model(pDesc->ModelName, &pModelDesc);
    if (!m_pModel)
        return E_FAIL;
    
    else
    {
        m_pShader = m_pModel->Get_Shader();
        Safe_AddRef(m_pShader);
    }
    return S_OK;
}

HRESULT CSocket_Model::Bind_ShaderResources()
{
    /*Combined Matrix를 직접던진다.*/
    if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", m_CombinedWorldMatrix)))
        return E_FAIL;


    return S_OK;
}

CSocket_Model* CSocket_Model::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
    CSocket_Model* pInstance = new CSocket_Model(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CSocket_Model");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CSocket_Model::Clone(void* pArg)
{
    CSocket_Model* pInstance = new CSocket_Model(*this);

    if (FAILED(pInstance->Initialize_Copytype(pArg)))
    {
        MSG_BOX("Failed to Cloned : CSocket_Model");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CSocket_Model::Free()
{
    __super::Free();

    Safe_Release(m_pModel);
    Safe_Release(m_pShader);
}
