#include "CChannel.h"
#include "CBone.h"

CChannel::CChannel()
{
}

HRESULT CChannel::Initialize(const char* filePath, const char* AnimFilePath,int AnimIdx, int index)
{
	ifstream	file(filePath);
	json jFile = json::parse(file);

	json AnimData = jFile["AnimDatas"][AnimIdx];
	json ChannelData = AnimData["Channels"][index];

	m_iNumKeyFrames = ChannelData["NumKeyFrames"].get<int>();
	m_iBoneIndex = ChannelData["BoneIndex"].get<int>();
	
	int BinaryOffSet = ChannelData["BinaryOffset"].get<int>();
	int BinarySize = ChannelData["BinarySize"].get<int>();
	

	m_KeyFrames.resize(m_iNumKeyFrames);

	ifstream Animfile(AnimFilePath, std::ios::binary);
	if (Animfile.is_open())
	{
		Animfile.seekg(BinaryOffSet);

		for (size_t i = 0; i < m_iNumKeyFrames; ++i)
		{
			KEYFRAME			KeyFrame{};
			Animfile.read((char*)&KeyFrame.vScale, sizeof(_float3));
			Animfile.read((char*)&KeyFrame.vRotation, sizeof(_float4));
			Animfile.read((char*)&KeyFrame.vTranslation, sizeof(_float3));
			Animfile.read((char*)&KeyFrame.fTrackPosition, sizeof(_float));

			m_KeyFrames[i] = KeyFrame;
		}
	}

	
    return S_OK;
}

void CChannel::Update_TransformationMatrix(const vector<class CBone*>& Bones, _float fCurrentTrackPosition)
{
	_float4x4	TransformationMatrix = {};
	


	Bones[m_iBoneIndex]->Set_TransformationMatrix(TransformationMatrix);
}

CChannel* CChannel::Create(const char* filePath, const char* AnimFilePath, int AnimIdx, int index)
{
	CChannel* pInstance = new CChannel();

	if (FAILED(pInstance->Initialize(filePath, AnimFilePath, AnimIdx, index)))
	{
		MSG_BOX("Failed to Created : CChannel");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CChannel::Free()
{
}
