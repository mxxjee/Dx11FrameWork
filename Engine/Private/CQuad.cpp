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
    if (FAILED(CQuad::Ready_Components(pArg)))
        return E_FAIL;
 

    QUAD_DESC* pQuad_Desc = static_cast<QUAD_DESC*>(pArg);
    m_eRenderGroup = pQuad_Desc->eRenderGroup;
    m_pTexShader = m_pGameInstance->Find_Shader(pQuad_Desc->ShaderName);
    Safe_AddRef(m_pTexShader);

    if (pQuad_Desc->ImgPath != L"")
    {
        m_pTexture = new CTexture(m_pDevice);
        if (FAILED(m_pTexture->LoadImg(pQuad_Desc->ImgPath)))
            return E_FAIL;

    }
   


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

    CShader* pRenderShader = m_pGameInstance->Get_RenderShader();
    string  pRenderPass = m_pGameInstance->Get_RenderPassName();

    CheckNullResult(pRenderShader, E_FAIL);

    //렌더 할때 copydata로 GPU에게 데이터전송
    pRenderShader->SetMatrix("g_WorldMatrix", m_pTransformCom->Get_World((TransformScope::WORLD)));
    pRenderShader->SetResource("texture0", m_pTexture->GetComPtr());
    pRenderShader->SetFloat("g_Brightness", 1.f);


    pRenderShader->Begin(pRenderPass); //VS-PS

 
    m_pVIBufferCom->Bind_Resource();   //IA단계


    //Set_BlendState();           //OM단계
    m_pVIBufferCom->Render();      //OM단계
    return S_OK;
}

HRESULT CQuad::Ready_Components(void* pArg)
{
    CComponent* pBuffer = dynamic_cast<CComponent*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::COMPONENT, 0, PROTO_COMPONENT_NAME(L"VIBuffer_Rect"), pArg));
    
    if (FAILED(Add_Component(COMPONENT_TYPE::VIBUFFER_RECT, pBuffer, (CComponent**)&m_pVIBufferCom)))
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

void CQuad::Free()
{
    __super::Free();
    Safe_Release(m_pTexShader);
    Safe_Release(m_pTexture);
    Safe_Release(m_pVIBufferCom);
}
