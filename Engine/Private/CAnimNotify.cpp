#include "CAnimNotify.h"
#include "CGameInstance.h"
CAnimNotify::CAnimNotify()
    :m_pGameInstance(CGameInstance::GetInstance())
{
    Safe_AddRef(m_pGameInstance);
}

HRESULT CAnimNotify::Initialize(void* pArg)
{
    AnimNotify_DESC* pDesc = static_cast<AnimNotify_DESC*>(pArg);
    m_KeyFrame = pDesc->iKeyFrame;
    m_Event = pDesc->GameEvent;

    return S_OK;
}

void CAnimNotify::NotifyBegin()
{
    m_pGameInstance->Emit(m_Event);
}


CAnimNotify* CAnimNotify::Create(void* pArg)
{
    CAnimNotify* pInstance = new CAnimNotify();
    if (FAILED(pInstance->Initialize(pArg)))
    {
        Safe_Release(pInstance);
        MSG_BOX("Failed to Create:AnimNotify");
    }
    return pInstance;
}

void CAnimNotify::Free()
{
    Safe_Release(m_pGameInstance);
}
