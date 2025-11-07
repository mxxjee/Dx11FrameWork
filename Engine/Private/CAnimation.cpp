#include "CAnimation.h"
#include "CChannel.h"
#include "CModel.h"


CAnimation::CAnimation()
{
}

HRESULT CAnimation::Initialize(CModel* pModel, const char* filePath,_uint idx)
{
	ifstream	file(filePath);
	json jFile = json::parse(file);
	
	json AnimData = jFile["AnimDatas"][idx];

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
		CChannel* pChannel = CChannel::Create(pModel,jFile,animPath.c_str(), idx, i);
		if (nullptr == pChannel)
			return E_FAIL;

		m_Channels.push_back(pChannel);
	}

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


	/*재생바의 위치에 따라 뼈들의 상태를 갱신시킨다.*/
	for (auto& pChannel : m_Channels)
	{
		pChannel->Update_TransformationMatrix(Bones, m_fCurrentTrackPosition);
	}

	return false;
}

CAnimation* CAnimation::Create(CModel* pModel, const char* filePath, _uint idx)
{
	CAnimation* pInstance = new CAnimation();

	if (FAILED(pInstance->Initialize(pModel,filePath,idx)))
	{
		MSG_BOX("Failed to Created : CAnimation");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CAnimation::Free()
{
	__super::Free();

	for (auto& pChannel : m_Channels)
		Safe_Release(pChannel);

	m_Channels.clear();
}
