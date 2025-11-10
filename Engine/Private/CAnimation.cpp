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
	//재생바가 있을때, 현재 재생바의 위치, 처음 재생시 m_fCurrentTrackPosition은 0부터 들어가지 않고 바로 timedelta를 곱한 값이 들어감
	m_fCurrentTrackPosition += fTimeDelta * m_fTickPerSecond;

	if (m_fCurrentTrackPosition >= m_fDuration)
	{
		if (!m_bLoop)
			return true;		//애니메이션 끝남 알림

		m_fCurrentTrackPosition = 0.f;//루프일경우 다시 맨처음부터 실행
	}


	_uint	iIndex = {};
	/*재생바의 위치에 따라 뼈들의 상태를 갱신시킨다.*/
	for (auto& pChannel : m_Channels)
	{
		pChannel->Update_TransformationMatrix(Bones, m_fCurrentTrackPosition, &m_CurrentKeyFrameIndices[iIndex++]);
	}

	return false;
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
