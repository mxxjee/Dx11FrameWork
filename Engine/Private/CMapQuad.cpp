#include "CMapQuad.h"

#include "CShader.h"
#include "CGameInstance.h"

#include "CTexture.h"
#include "CVIBuffer_Rect.h"

CMapQuad::CMapQuad(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	:CMapObject(pDevice, pContext)
{
}

CMapQuad::CMapQuad(const CMapQuad& rhs)
	: CMapObject(rhs)
{
}

HRESULT CMapQuad::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CMapQuad::Initialize_Copytype(void* pArg)
{
	/*부모 컴포넌트 값세팅 */
	if (FAILED(__super::Initialize_Copytype(pArg)))
		return E_FAIL;

	/*내 컴포넌트 값 세팅*/
	if (FAILED(Ready_Components(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Resources(pArg)))

		return E_FAIL;
	return S_OK;
}

void CMapQuad::Update_Priority(_float fTimeDelta)
{
	__super::Update_Priority(fTimeDelta);
}

void CMapQuad::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);


}

void CMapQuad::Update_Late(_float fTimeDelta)
{
	__super::Update_Late(fTimeDelta);
}

void CMapQuad::Update_Render(_float fTimeDelta)
{
	__super::Update_Render(fTimeDelta);
}

HRESULT CMapQuad::Render()
{
	__super::Render();

	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	if (FAILED(m_pShader->Begin(m_passName)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Resource()))
		return E_FAIL;          //IA단계

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CMapQuad::Ready_Components(void* pArg)
{

	CComponent* pBuffer = dynamic_cast<CComponent*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::COMPONENT, 0, PROTO_COMPONENT_NAME(L"VIBuffer_Rect"), pArg));
	if (FAILED(__super::Add_Component(COMPONENT_TYPE::VIBUFFER_RECT, pBuffer, (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	pBuffer->Set_Owner(this);

	return S_OK;
}

HRESULT CMapQuad::Ready_Resources(void* pArg)
{
	CheckNullResult(pArg, E_FAIL);
	MAPQUAD_DESC* pQuad_Desc = static_cast<MAPQUAD_DESC*>(pArg);


	if (pQuad_Desc->TextureKey != L"")
	{
		m_pTexture = m_pGameInstance->Find_Texture(pQuad_Desc->TextureKey);
		if (m_pTexture)
			Safe_AddRef(m_pTexture);
	}

	return S_OK;
}

HRESULT CMapQuad::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShader, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pTexture->Bind_ShaderResource(m_pShader, "texture0", 0)))
		return E_FAIL;


	return S_OK;
}

CMapQuad* CMapQuad::Create(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pDeviceContext)
{
	CMapQuad* pInstance = new CMapQuad(_pDevice, _pDeviceContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create :CMapQuad ");
		Safe_Release(pInstance);

	}
	return pInstance;
}

CGameObject* CMapQuad::Clone(void* pArg)
{
	CMapQuad* pInstance = new CMapQuad(*this);
	if (FAILED(pInstance->Initialize_Copytype(pArg)))
	{
		MSG_BOX("Failed to Cloned :CMapQuad ");
		Safe_Release(pInstance);

	}
	return pInstance;
}

void CMapQuad::Free()
{
	__super::Free();
	Safe_Release(m_pTexture);
	Safe_Release(m_pVIBufferCom);
}
