#include "CAnimation.h"
#include "CChannel.h"
#include "CModel.h"


CAnimation::CAnimation()
{
}

CAnimation::CAnimation(const CAnimation& Prototype)
	:m_iNumChannels{Prototype.m_iNumChannels},
	m_Channels{Prototype.m_Channels},
	m_fTickPerSecond{Prototype.m_fTickPerSecond},
	m_fDuration{Prototype.m_fDuration},
	m_fCurrentTrackPosition{Prototype.m_fCurrentTrackPosition},
	m_bLoop{Prototype.m_bLoop},
	m_CurrentKeyFrameIndices{Prototype.m_CurrentKeyFrameIndices}

{
	strcpy_s(m_szName, MAX_PATH, Prototype.m_szName);

	for (auto& Channel : m_Channels)
		Safe_AddRef(Channel);
}

HRESULT CAnimation::Initialize(CModel* pModel, json& Json, const char* filePath,_uint idx)
{
	
	
	json AnimData = Json["AnimDatas"][idx];

	string AnimName = AnimData["Name"];

	strcpy_s(m_szName, MAX_PATH, AnimName.c_str());
	m_iNumChannels = AnimData["NumChannels"];
	m_fTickPerSecond = AnimData["TickPerSecond"].get<float>();
	m_fDuration = AnimData["Duration"].get<float>();
	m_fCurrentTrackPosition = 0.f;

	fs::path Path = filePath;
	string BasePath = Path.parent_path().string();
	string animPath = BasePath + "\\" + m_szName + ".anim";


	for (size_t i = 0; i < m_iNumChannels; ++i)
	{
		//채널이 키프레임을 만들떄 필요한 .anim파일을 넘겨준다.
		CChannel* pChannel = CChannel::Create(pModel, Json,animPath.c_str(), idx, i);
		if (nullptr == pChannel)
			return E_FAIL;

		m_Channels.push_back(pChannel);
	}

	m_CurrentKeyFrameIndices.resize(m_iNumChannels);

    return S_OK;
}

bool CAnimation::Update_TransformationMatrices(const vector<class CBone*>& Bones, _float fTimeDelta)
{

	m_fCurrentTrackPosition += fTimeDelta * m_fTickPerSecond;


	if (m_fCurrentTrackPosition >= m_fDuration)
	{
		if (!m_bLoop)
			return true;		//애니메이션 끝남 알림

		m_fCurrentTrackPosition = 0.f;//루프일경우 다시 맨처음부터 실행
	}

	//전이상태가 아니라면..특정 뼈들만 상태갱신
	
	_uint	iIndex = {};

	/*재생바의 위치에 따라 뼈들의 상태를 갱신시킨다.*/
	for (auto& pChannel : m_Channels)
	{
		pChannel->Update_TransformationMatrix(Bones, m_fCurrentTrackPosition, &m_CurrentKeyFrameIndices[iIndex++]);
	}

	return false;

}

bool CAnimation::Update_BlendAnim(CModel* pModel, const vector<class CBone*>& Bones, _float TranslationTime, int PreAnimIndex)
{

	////전이상태일떄 수행하는 함수..
	//
	////이전애니메이션의 키프레임값가져오기
	////이전애니메이션의 위치와 새로실행할 애니메이션의 첫프레임과 보간
	//CAnimation* pPreAnim = pModel->Get_Animation(PreAnimIndex);
	//float pPrevAnimPosition = pPreAnim->Get_CurrentTrackPoistion();

	///*모든 본을 검사하며 전이가필요한 것들은 전이를 수행하도록하게 하고, 아닌것들은 */
	//for (auto& pBon : Bones)
	//{

	//}

	//for (auto& pChannel : m_Channels)
	//{
	//	pChannel->UpdateTransformMatrix_BlendAnim(Bones,pPreAnim, TranslationTime, pPrevAnimPosition);
	//}
	return false; 
}

CChannel* CAnimation::Get_Channel_BoneIdx(int BoneNum)
{
	int Num = 0;

	for (auto Channel : m_Channels)
	{
		if (Channel->Get_BoneIndex_ByChannel() == BoneNum)
			return Channel;
	}
	return nullptr;
}

CAnimation* CAnimation::Create(CModel* pModel, json& Json, const char* filePath, _uint idx)
{
	CAnimation* pInstance = new CAnimation();

	if (FAILED(pInstance->Initialize(pModel, Json,filePath,idx)))
	{
		MSG_BOX("Failed to Created : CAnimation");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CAnimation* CAnimation::Clone()
{
	return new CAnimation(*this);
}

void CAnimation::Free()
{
	__super::Free();

	for (auto& pChannel : m_Channels)
		Safe_Release(pChannel);

	m_Channels.clear();
}
