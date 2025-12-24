#include "CGameManager.h"
#include "CGameObject.h"
#include "CGameObject.h"
#include "CBase.h"
#include "CPlayer.h"

IMPLEMENT_SINGLETON(CGameManager)

USING(Client)
void CGameManager::Set_MainPlayer(CGameObject* pObj)
{
	CheckNull(pObj);
	CPlayer* pPlayer = dynamic_cast<CPlayer*>(pObj);
	CheckNull(pPlayer);

	m_pMainPlayer = pPlayer;
	Safe_AddRef(pPlayer);

}

void CGameManager::Set_MainPlayer(CBase* pObj)
{
	CheckNull(pObj);
	CPlayer* pPlayer = dynamic_cast<CPlayer*>(pObj);
	CheckNull(pPlayer);

	m_pMainPlayer = pPlayer;
	Safe_AddRef(pPlayer);

}

void CGameManager::Set_LastPosition(_float4 vPosition)
{
	vLastPosition = vPosition;
}

void CGameManager::Set_LastPosition(_vector vPosition)
{
	return XMStoreFloat4(&vLastPosition, vPosition);
}

_float4 CGameManager::Get_LastPosition_By_Float4()
{
	return vLastPosition;
}

_vector CGameManager::Get_LastPosition_By_Vector()
{
	return XMLoadFloat4(&vLastPosition);
}

_float4 CGameManager::Get_DefaultPositio_By_Float4()
{
	return vDefulatPosition;
}

_vector CGameManager::Get_DefaultPositioBy_Vector()
{
	return XMLoadFloat4(&vDefulatPosition);
}

void CGameManager::Free()
{
	__super::Free();
	Safe_Release(m_pMainPlayer);
}

void CGameManager::Set_DefaultPosition(_vector vPosition)
{
	return XMStoreFloat4(&vLastPosition, vPosition);
}

void CGameManager::Set_DefaultPosition(_float4 vPosition)
{
	vLastPosition = vPosition;
}
