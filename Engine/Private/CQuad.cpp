#include "CQuad.h"
#include "CGameInstance.h"

#include "CTexture.h"
#include "CShader.h"
#include "CVIBuffer_Rect.h"


CQuad::CQuad(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	:CGameObject(pDevice,pContext)
{
}

CQuad::CQuad(const CQuad& rhs)
	:CGameObject(rhs)
{
}

HRESULT CQuad::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CQuad::Initialize_Copytype(void* pArg)
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

void CQuad::Update_Priority(_float fTimeDelta)
{
    __super::Update_Priority(fTimeDelta);
}

void CQuad::Update(_float fTimeDelta)
{
    __super::Update(fTimeDelta);
    CheckNull(m_pTransformCom);

   

}

void CQuad::Update_Late(_float fTimeDelta)
{
    __super::Update_Late(fTimeDelta);
}

void CQuad::Update_Render(_float fTimeDelta)
{
    __super::Update_Render(fTimeDelta);
    m_pGameInstance->Add_RenderObject(m_eRenderGroup, this);

}

HRESULT CQuad::Render()
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

HRESULT CQuad::Ready_Components(void* pArg)
{

    CComponent* pBuffer = dynamic_cast<CComponent*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::COMPONENT, 0, PROTO_COMPONENT_NAME(L"VIBuffer_Rect"), pArg));
    if (FAILED(__super::Add_Component(COMPONENT_TYPE::VIBUFFER_RECT, pBuffer, (CComponent**)&m_pVIBufferCom)))
        return E_FAIL;

   
    return S_OK;
}








CQuad* CQuad::Create(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pDeviceContext)
{
    CQuad* pInstance = new CQuad(_pDevice, _pDeviceContext);
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Create :CQuad ");
        Safe_Release(pInstance);

    }
    return pInstance;
}

CGameObject* CQuad::Clone(void* pArg)
{
    CQuad* pInstance = new CQuad(*this);
    if (FAILED(pInstance->Initialize_Copytype(pArg)))
    {
        MSG_BOX("Failed to Cloned :CQuad ");
        Safe_Release(pInstance);

    }
    return pInstance;
}

HRESULT CQuad::Ready_Resources(void* pArg)
{
    CheckNullResult(pArg, E_FAIL);
    QUAD_DESC* pQuad_Desc = static_cast<QUAD_DESC*>(pArg);

    m_eRenderGroup = pQuad_Desc->eRenderGroup;
    m_ShaderName = pQuad_Desc->ShaderName;
    m_passName = pQuad_Desc->passName;

   
    m_pShader = m_pGameInstance->Find_Shader(pQuad_Desc->ShaderName);
    Safe_AddRef(m_pShader);

    if (pQuad_Desc->TextureKey != L"")
    {
        m_pTexture = m_pGameInstance->Find_Texture(pQuad_Desc->TextureKey);
        if(m_pTexture)
            Safe_AddRef(m_pTexture);
    }
        
    return S_OK;
}

void CQuad::Free()
{
    __super::Free();
    Safe_Release(m_pShader);
    Safe_Release(m_pTexture);
    Safe_Release(m_pVIBufferCom);
}

HRESULT CQuad::Bind_ShaderResources()
{
    if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShader, "g_WorldMatrix")))
        return E_FAIL;

    if (FAILED(m_pTexture->Bind_ShaderResource(m_pShader, "texture0", 0)))
        return E_FAIL;

    if (FAILED(m_pShader->Bind_Float("g_Alpha",m_vARGB.x)))
        return E_FAIL;


    return S_OK;

}

void CQuad::Set_Texture(const _wstring& NewTexKey)
{
    CTexture* pNewTex = m_pGameInstance->Find_Texture(NewTexKey);
    if (pNewTex)
    {
        Safe_Release(m_pTexture);
        m_pTexture = pNewTex;
        Safe_AddRef(m_pTexture);
    }
}
