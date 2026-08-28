#include "CIInteractable.h"
#include "CInteraction_Manager.h"

USING(Client)

// Common Candidate and lifetime policy shared by every interactable owner.
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
