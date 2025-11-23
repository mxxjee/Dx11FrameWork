#include "CBody.h"
#include "CModel.h"
#include "CMeshComponent.h"
#include "CShader.h"
#include "CGameInstance.h"
#include "CAnimation.h"





CBody::CBody(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    :CPartObject(pDevice,pContext)
{
}

CBody::CBody(const CBody& rhs)
    :CPartObject(rhs)
{
}

HRESULT CBody::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CBody::Initialize_Copytype(void* pArg)
{
    BODY_DESC* pDesc = static_cast<BODY_DESC*>(pArg);
    m_pParentState = pDesc->pParentState;


    if (FAILED(__super::Initialize_Copytype(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Resource(pArg)))
        return E_FAIL;

    return S_OK;
}

void CBody::Update_Priority(_float fTimeDelta)
{
    __super::SetUp_CombinedWorldMatrix(XMLoadFloat4x4(m_pParentMatrix));
}

void CBody::Update(_float fTimeDelta)
{
   
}

void CBody::Update_Late(_float fTimeDelta)
{
    if (m_pModel && m_pModel->Get_NumAnim() > 0)
        m_pModel->Play_Animation(fTimeDelta);
}

void CBody::Update_Render(_float fTimeDelta)
{
    m_pGameInstance->Add_RenderObject(m_eRenderGroup, this);
}

HRESULT CBody::Render()
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

            if (FAILED(m_pModel->Bind_Bones(m_pShader, "g_BoneMatrices", Mesh.second)))
                return E_FAIL;

            if (FAILED(m_pShader->Begin(Mesh.second->Get_PassName())))
                return E_FAIL;

            if (FAILED(m_pModel->Render(Mesh.second)))
                return E_FAIL;


        }



    }


    return S_OK;
}

void CBody::Set_VisibleMesh(const wstring& MeshName, bool bVisible)
{
    if (m_pModel)
        m_pModel->Set_VisibleMesh(MeshName, bVisible);
}

_float3 CBody::Get_RootDelta()
{
    _float3         Result = _float3();

    if (m_pModel)
        Result = m_pModel->Get_RootDelta();

    return Result;
}

bool CBody::Get_IsAnimFinished()
{

	CheckNullResult(m_pModel, false);
    CheckTrueResult(m_pModel->Get_NumAnim() == 0, false);


	return m_pModel->Get_IsAnimFinished();

}

HRESULT CBody::Ready_Components(void* pArg)
{
    CheckNullResult(pArg, E_FAIL);
    BODY_DESC* pBodyDesc = static_cast<BODY_DESC*>(pArg);
    if (pBodyDesc)
    {
        CModel::MODEL_DSC pModelDesc;
        pModelDesc.pOwner = this;
        m_pModel = m_pGameInstance->Clone_Model(pBodyDesc->modelName, &pModelDesc);

        if (!m_pModel)
            return E_FAIL;
    }

    return S_OK;
}

HRESULT CBody::Ready_Resource(void* pArg)
{
    CBody::BODY_DESC* pBodyDesc = static_cast<CBody::BODY_DESC*>(pArg);
    m_eRenderGroup = pBodyDesc->eRenderGroup;
    m_pParentState = pBodyDesc->pParentState;

    if (m_pModel)
    {
        m_pShader = m_pModel->Get_Shader();
        Safe_AddRef(m_pShader);

    }

    return S_OK;
}

HRESULT CBody::Bind_ShaderResources()
{
    /*Combined Matrix를 직접던진다.*/
    if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", m_CombinedWorldMatrix)))
        return E_FAIL;


    return S_OK;
}

CBody* CBody::Create(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pDeviceContext)
{
    CBody* pInstance = new CBody(_pDevice, _pDeviceContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CBody");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CBody::Clone(void* pArg)
{
    CBody* pInstance = new CBody(*this);

    if (FAILED(pInstance->Initialize_Copytype(pArg)))
    {
        MSG_BOX("Failed to Cloned : CBody");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CBody::Free()
{
    __super::Free();


    Safe_Release(m_pModel);
    Safe_Release(m_pShader);
}

void CBody::Reserve_Animation(_wstring AnimKey, bool bLoop)
{
    m_NextAnimKey = AnimKey;
    m_NextAnimLoop = bLoop;

}

