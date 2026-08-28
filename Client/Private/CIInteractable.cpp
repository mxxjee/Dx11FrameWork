#include "CIInteractable.h"
#include "CInteraction_Manager.h"

USING(Client)

// 모든 상호작용 객체가 공유하는 후보 등록 및 생명주기 정책이다.
void CIInteractable::OnInteractionRangeEnter()
{
	RequestAddCandidate();
}

void CIInteractable::OnInteractionRangeExit()
{
	RequestRemoveCandidate();
}

void CIInteractable::RequestAddCandidate()
{
	CInteraction_Manager::GetInstance()->RequestAddCandidate(this);
}

void CIInteractable::RequestRemoveCandidate()
{
	CInteraction_Manager::GetInstance()->RequestRemoveCandidate(this);
}

void CIInteractable::ReleaseInteractionRegistration()
{
	CInteraction_Manager::GetInstance()->PurgeInteractable(this);
}
