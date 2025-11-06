#include "CAnimation.h"
#include "CChannel.h"

CAnimation::CAnimation()
{
}

HRESULT CAnimation::Initialize(const char* filePath,_uint idx)
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
		CChannel* pChannel = CChannel::Create(filePath,animPath.c_str(), idx, i);
		if (nullptr == pChannel)
			return E_FAIL;

		m_Channels.push_back(pChannel);
	}

    return S_OK;
}

void CAnimation::Update_TransformationMatrices(const vector<class CBone*>& Bones, _float fTimeDelta)
{
	m_fCurrentTrackPosition += fTimeDelta * m_fTickPerSecond;


	for (auto& pChannel : m_Channels)
	{

	}
}

CAnimation* CAnimation::Create(const char* filePath, _uint idx)
{
	CAnimation* pInstance = new CAnimation();

	if (FAILED(pInstance->Initialize(filePath,idx)))
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
