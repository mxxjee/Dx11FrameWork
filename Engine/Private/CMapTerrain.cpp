#include "CMapTerrain.h"
#include "CModel.h"
#include "CShader.h"
#include "CMeshComponent.h"
#include "CGameInstance.h"
#include "CGameInstance.h"



CMapTerrain::CMapTerrain(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    :CTerrain_Base(pDevice,pContext)
{
}

CMapTerrain::CMapTerrain(const CMapTerrain& rhs)
    :CTerrain_Base(rhs)
{
}

HRESULT CMapTerrain::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;

    

    return S_OK;
}

HRESULT CMapTerrain::Initialize_Copytype(void* pArg)
{
    /*부모 컴포넌트 값세팅 */
    if (FAILED(CGameObject::Initialize_Copytype(pArg)))
        return E_FAIL;

    /*내 컴포넌트 값 세팅*/
    if (FAILED(Ready_Components(pArg)))
        return E_FAIL;

    /*내 컴포넌트 값 세팅*/
    if (FAILED(Ready_Resources(pArg)))
        return E_FAIL;


    m_eObjType = MapObjType::TERRAIN;



    return S_OK;
}

void CMapTerrain::Update_Priority(_float fTimeDelta)
{
    __super::Update_Priority(fTimeDelta);
}

void CMapTerrain::Update(_float fTimeDelta)
{
    __super::Update(fTimeDelta);
}

void CMapTerrain::Update_Late(_float fTimeDelta)
{
    __super::Update_Late(fTimeDelta);
}

void CMapTerrain::Update_Render(_float fTimeDelta)
{
    __super::Update_Render(fTimeDelta);
}

HRESULT CMapTerrain::Render()
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

            if (FAILED(m_pShader->Begin(Mesh.second->Get_PassName())))
                return E_FAIL;

            if (FAILED(m_pModel->Render(Mesh.second)))
                return E_FAIL;


        }



    }


    return S_OK;
}

HRESULT CMapTerrain::Ready_Components(void* pArg)
{
    CheckNullResult(pArg, E_FAIL);
    MAPTERRAIN_DESC* pTerrainModelDesc = static_cast<MAPTERRAIN_DESC*>(pArg);
    if (pTerrainModelDesc)
    {
        CModel::MODEL_DSC* ppModelDesc = static_cast<CModel::MODEL_DSC*>(pTerrainModelDesc->modelDesc);
        ppModelDesc->pOwner = this;
        m_pModel = m_pGameInstance->Clone_Model(pTerrainModelDesc->modelName, ppModelDesc);

        if (!m_pModel)
            return E_FAIL;
    }

    return S_OK;
}

HRESULT CMapTerrain::Ready_Resources(void* pArg)
{

    CheckNullResult(pArg, E_FAIL);
    MAPTERRAIN_DESC* pTerrainModelDesc = static_cast<MAPTERRAIN_DESC*>(pArg);



	if (m_pShader)
		Safe_Release(m_pShader);

    m_pShader = m_pGameInstance->Find_Shader(L"Shader_MapTool");
	Safe_AddRef(m_pShader);




    return S_OK;
}

HRESULT CMapTerrain::Bind_ShaderResources()
{
    if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShader, "g_WorldMatrix")))
        return E_FAIL;


    return S_OK;
}

CMapTerrain* CMapTerrain::Create(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pDeviceContext)
{
    CMapTerrain* pInstance = new CMapTerrain(_pDevice, _pDeviceContext);
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Create :CMapTerrain ");
        Safe_Release(pInstance);

    }



    return pInstance;
}

CGameObject* CMapTerrain::Clone(void* pArg)
{
    CMapTerrain* pInstance = new CMapTerrain(*this);
    if (FAILED(pInstance->Initialize_Copytype(pArg)))
    {
        MSG_BOX("Failed to Cloned :CMapModel ");
        Safe_Release(pInstance);

    }
    return pInstance;
}

void CMapTerrain::Free()
{
    __super::Free();
    Safe_Release(m_pModel);
}

void CMapTerrain::OnSeletected(bool bSelected)
{
    m_bSelected = bSelected;
}

void CMapTerrain::Save_To_Json()
{
}
