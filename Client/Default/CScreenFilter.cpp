#include "CScreenFilter.h"
#include "CGameInstance.h"
#include "CVIBuffer_Rect.h"

USING(Client)
CScreenFilter::CScreenFilter(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    :CGameObject(pDevice, pContext)
{
}

CScreenFilter::CScreenFilter(const CScreenFilter& rhs)
    :CGameObject(rhs)
{
}

HRESULT CScreenFilter::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;


    return S_OK;
}

HRESULT CScreenFilter::Initialize_Copytype(void* pArg)
{
    if (FAILED(__super::Initialize_Copytype(pArg)))
        return E_FAIL;

    /*³» ÄÄÆ÷³ÍÆ® °ª ¼¼ÆÃ*/
    if (FAILED(CScreenFilter::Ready_Components(pArg)))
        return E_FAIL;


    return S_OK;
}

void CScreenFilter::Update_Priority(_float fTimeDelta)
{
    __super::Update_Priority(fTimeDelta);

}

void CScreenFilter::Update(_float fTimeDelta)
{
    __super::Update(fTimeDelta);
}

void CScreenFilter::Update_Late(_float fTimeDelta)
{
    __super::Update_Late(fTimeDelta);
}

void CScreenFilter::Update_Render(_float fTimeDelta)
{
    __super::Update_Render(fTimeDelta);
}

HRESULT CScreenFilter::Render()
{
    //ÇöÀç ·»´õÅ¸°ÙÀ» °¡Á®¿È
    return S_OK;
}

HRESULT CScreenFilter::Ready_Components(void* pArg)
{
  /*  CComponent* pBuffer = dynamic_cast<CComponent*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::COMPONENT, 0, PROTO_COMPONENT_NAME(L"VIBuffer_Rect"), pArg));

    if (FAILED(Add_Component(COMPONENT_TYPE::VIBUFFER_RECT, pBuffer, (CComponent**)&m_pVIBufferCom)))
        return E_FAIL;

        */
    return S_OK;
}



void CScreenFilter::Free()
{
    __super::Free();
}
