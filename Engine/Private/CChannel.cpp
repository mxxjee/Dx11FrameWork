#include "CChannel.h"
#include "CBone.h"
#include "CModel.h"


CChannel::CChannel()
{
}

HRESULT CChannel::Initialize(CModel* pModel, json& Json, const char* AnimFilePath,int AnimIdx, int index)
{

	json AnimData = Json["AnimDatas"][AnimIdx];
	json ChannelData = AnimData["Channels"][index];

	m_iNumKeyFrames = ChannelData["NumKeyFrames"].get<int>();
	string Name = ChannelData["BoneName"];

	m_iBoneIndex = pModel->Get_BoneIndex(Name.c_str());

	
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
	
	/*다시재생했을떄..*/
	if (0.0f == fCurrentTrackPosition)
		m_iCurrentKeyFrameIndex = 0;

	/* fCurrentTrackPosition시간에 맞는 현재 뼈의 상태를 만든다.*/
	_float4x4	TransformationMatrix = {};

	/*가장 마지막 키프레임키를 가져와 애니메이션 끝 도달을 확인.*/
	KEYFRAME		LastKeyFrame = m_KeyFrames.back();

	/*조합할 SRT*/
	_float3 vScale, vTranslation;
	_float4 vRotation;
	
	//애니메이션 클립끝남,마지막 상태유지
	if (fCurrentTrackPosition >= LastKeyFrame.fTrackPosition)
	{
		vScale = LastKeyFrame.vScale;
		vRotation = LastKeyFrame.vRotation;
		vTranslation = LastKeyFrame.vTranslation;

	}


	//선형보간 하기
	else
	{
		_vector vLeftScale{}, vRightScale{};
		_vector vLeftRotation{}, vRightRotation{};
		_vector vLeftTranslation{}, vRightTranslation{};



		if (fCurrentTrackPosition >= m_KeyFrames[(INT64)m_iCurrentKeyFrameIndex + 1].fTrackPosition)
			++m_iCurrentKeyFrameIndex;


		vLeftScale = XMLoadFloat3(&m_KeyFrames[m_iCurrentKeyFrameIndex].vScale);
		vRightScale = XMLoadFloat3(&m_KeyFrames[(INT64)m_iCurrentKeyFrameIndex+1].vScale);

		vLeftRotation = XMLoadFloat4(&m_KeyFrames[m_iCurrentKeyFrameIndex].vRotation);
		vRightRotation = XMLoadFloat4(&m_KeyFrames[(INT64)m_iCurrentKeyFrameIndex + 1].vRotation);


		vLeftTranslation = XMLoadFloat3(&m_KeyFrames[m_iCurrentKeyFrameIndex].vTranslation);
		vRightTranslation = XMLoadFloat3(&m_KeyFrames[(INT64)m_iCurrentKeyFrameIndex + 1].vTranslation);


		_float fRatio = (fCurrentTrackPosition - m_KeyFrames[m_iCurrentKeyFrameIndex].fTrackPosition) / (m_KeyFrames[m_iCurrentKeyFrameIndex + 1].fTrackPosition - m_KeyFrames[m_iCurrentKeyFrameIndex].fTrackPosition);

		XMStoreFloat3(&vScale,XMVectorLerp(vLeftScale, vRightScale, fRatio));
		XMStoreFloat4(&vRotation, XMQuaternionSlerp(vLeftRotation, vRightRotation, fRatio));
		XMStoreFloat3(&vTranslation, XMVectorLerp(vLeftTranslation, vRightTranslation, fRatio));


	}

	XMStoreFloat4x4(&TransformationMatrix,
		XMMatrixScaling(vScale.x, vScale.y, vScale.z) * XMMatrixRotationQuaternion(XMVectorSet(vRotation.x, vRotation.y, vRotation.z, vRotation.w)) * XMMatrixTranslation(vTranslation.x, vTranslation.y, vTranslation.z));



	Bones[m_iBoneIndex]->Set_TransformationMatrix(TransformationMatrix);
}

CChannel* CChannel::Create(class CModel* pModel, json& Json, const char* AnimFilePath, int AnimIdx, int index)
{
	CChannel* pInstance = new CChannel();

	if (FAILED(pInstance->Initialize(pModel,Json, AnimFilePath, AnimIdx, index)))
	{
		MSG_BOX("Failed to Created : CChannel");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CChannel::Free()
{
	__super::Free();

	m_KeyFrames.clear();
}
