#include "CTerrain_Highlight.h"
#include "CGameInstance.h"
#include "CVIBuffer_Triangle.h"
#include "CShader.h"


USING(MapTool)
CTerrain_Highlight::CTerrain_Highlight(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	:CMapObject{pDevice,pContext}
{
}
CTerrain_Highlight::CTerrain_Highlight(const CTerrain_Highlight& Prototype)
	: CMapObject(Prototype)
{
}
HRESULT CTerrain_Highlight::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CTerrain_Highlight::Initialize_Copytype(void* pArg)
{
	if (FAILED(__super::Initialize_Copytype(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Component(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Resources(pArg)))
		return E_FAIL;

	return S_OK;
}

void CTerrain_Highlight::Update_Priority(_float fTimeDelta)
{
	__super::Update_Priority(fTimeDelta);
}

void CTerrain_Highlight::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);
}

void CTerrain_Highlight::Update_Late(_float fTimeDelta)
{
	__super::Update_Late(fTimeDelta);
}

void CTerrain_Highlight::Update_Render(_float fTimeDelta)
{
	__super::Update_Render(fTimeDelta);
	m_pGameInstance->Add_RenderObject(m_eRenderGroup, this);
}

HRESULT CTerrain_Highlight::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	if (FAILED(m_pShader->Begin(m_passName)))
		return E_FAIL;

	if (FAILED(m_pTriangleBuffer->Bind_Resource()))
		return E_FAIL;

	if (FAILED(m_pTriangleBuffer->Render()))
		return E_FAIL;

	return S_OK;
}

CTerrain_Highlight* CTerrain_Highlight::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
	CTerrain_Highlight* pInstance = new CTerrain_Highlight(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create :CTerrain_Highlight ");
		Safe_Release(pInstance);

	}



	return pInstance;
}

CGameObject* CTerrain_Highlight::Clone(void* pArg)
{
	CTerrain_Highlight* pInstance = new CTerrain_Highlight(*this);
	if (FAILED(pInstance->Initialize_Copytype(pArg)))
	{
		MSG_BOX("Failed to Cloned :CTerrain_Highlight ");
		Safe_Release(pInstance);

	}
	return pInstance;
}



HRESULT CTerrain_Highlight::Ready_Component(void* pArg)
{
	//transformºÎÂø
	HIGHLIGHT_DESC* pDesc = static_cast<HIGHLIGHT_DESC*>(pArg);

	CVIBuffer_Triangle::TRIANGLEBUFFER_DESC* TriangleBufferDesc = static_cast<CVIBuffer_Triangle::TRIANGLEBUFFER_DESC*>(pDesc->TriangleBuffer);
	TriangleBufferDesc->pOwner = this;

	m_Triangle.v0 = TriangleBufferDesc->v0;
	m_Triangle.v1 = TriangleBufferDesc->v1;
	m_Triangle.v2 = TriangleBufferDesc->v2;

	//vibufferºÎÂø
	CComponent* pTriangleBuffer = dynamic_cast<CVIBuffer_Triangle*>(m_pGameInstance->Clone_Prototype
	(PROTOTYPE::COMPONENT, 0, PROTO_COMPONENT_NAME(L"VIBuffer_Triangle"), pDesc->TriangleBuffer));

	if (FAILED(__super::Add_Component(COMPONENT_TYPE::VIBUFFER_TRIANGLE, pTriangleBuffer, reinterpret_cast<CComponent**>(&m_pTriangleBuffer))))
		return E_FAIL;
	
	//transform parent¼³Á¤
	if (m_pTransformCom)
	{
		if(pDesc->pOwner)
			m_pTransformCom->Set_Parent(pDesc->pOwner->Get_Transform());
	}

	return S_OK;
}

HRESULT CTerrain_Highlight::Ready_Resources(void* pArg)
{
	CheckNullResult(pArg, E_FAIL);
	HIGHLIGHT_DESC* pHighlight_desc = static_cast<HIGHLIGHT_DESC*>(pArg);
	m_ShaderName = pHighlight_desc->ShaderName;
	m_passName = pHighlight_desc->passName;
	m_eRenderGroup = pHighlight_desc->eRenderGroup;


	m_pShader = m_pGameInstance->Find_Shader(pHighlight_desc->ShaderName);
	Safe_AddRef(m_pShader);



	return S_OK;
}

HRESULT CTerrain_Highlight::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShader, "g_WorldMatrix")))
		return E_FAIL;
}


void CTerrain_Highlight::Free()
{
	
	__super::Free();
	Safe_Release(m_pShader);
	Safe_Release(m_pTriangleBuffer);

}