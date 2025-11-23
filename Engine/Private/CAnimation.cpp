#include "CAnimation.h"
#include "CChannel.h"
#include "CModel.h"
#include "CBone.h"
#include "CBody.h"

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
		Reset_Animtion();//애니메이션 재생바를 0으로 위치시키고, 키프레임을 모두 초기화한다.
		

		if (!m_bLoop)
		{
			return true;		//애니메이션 끝남 알림

		}
		//Reset_Animtion();//애니메이션 재생바를 0으로 위치시키고, 키프레임을 모두 초기화한다.
		
	}

	//전이상태가 아니라면..특정 뼈들만 상태갱신
	
	_uint	iIndex = {};

	/*재생바의 위치에 따라 뼈들의 상태를 갱신시킨다.*/
	for (auto& pChannel : m_Channels)
	{
		if (m_bEnableRootMotion)
			Update_RootMotion(Bones, pChannel);

		pChannel->Update_TransformationMatrix(Bones, m_fCurrentTrackPosition, &m_CurrentKeyFrameIndices[iIndex]);
		iIndex += 1;
	}

	return false;

}


void CAnimation::Reset_Animtion()
{
	m_fCurrentTrackPosition = 0.f;//루프일경우 다시 맨처음부터 실행
	for (auto& TargetKeyFrame : m_CurrentKeyFrameIndices)
		TargetKeyFrame = 0;

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

_matrix CAnimation::Get_CurrentKeyFrameBonSRT(int BoneIdx)
{
	CChannel* pChannel = Get_Channel_BoneIdx(BoneIdx);
	if (pChannel)
		return pChannel->Get_CurrentKeyFrameBoneSRT(&m_CurrentKeyFrameIndices[Get_ChannelIdx(BoneIdx)]);

	return _matrix();
}

int CAnimation::Get_ChannelIdx(int BoneNum)
{
	int Num = 0;

	for (auto Channel : m_Channels)
	{
		if (Channel->Get_BoneIndex_ByChannel() == BoneNum)
			break;

		else
			++Num;
	}

	return Num;
}

void CAnimation::Update_RootMotion(const vector<class CBone*>& Bones, CChannel* pChannel)
{
	//루트본이아니라면, 실행X
	
	CheckFalse(pChannel->Get_BoneIndex_ByChannel() ==1);
	RootDelta = _float3(0.f, 0.f, 0.f);

	//현재 프레임의 루트본 위치
	_uint CurrentIdx=0;
	CurrentIdx = pChannel->Get_CurrentKeyFrameIndex(m_fCurrentTrackPosition, &CurrentIdx);
	_matrix RootBone_CurMatrix = pChannel->Get_CurrentKeyFrameBoneSRT(&CurrentIdx);
	_vector vScale, vRotation, vTranslation;
	

	//SRT분리
	XMMatrixDecompose(&vScale, &vRotation, &vTranslation, RootBone_CurMatrix);

	CheckTrue(CurrentIdx == 0);
	KEYFRAME* keyFrame = (pChannel->Get_KeyFrame(CurrentIdx));
	//이전프레임 루트본위치
	CurrentIdx -= 1;

	_matrix RootBone_PreMatrix = pChannel->Get_CurrentKeyFrameBoneSRT(&CurrentIdx);
	_vector vPreScale, vPreRotation, vPreTranslation;
	//SRT분리
	XMMatrixDecompose(&vPreScale, &vPreRotation, &vPreTranslation, RootBone_PreMatrix);

	/// //////////////////////////////////////////////////////////////
	_float3 vCurPos, vPrePos;
	XMStoreFloat3(&vCurPos, vTranslation);
	XMStoreFloat3(&vPrePos, vPreTranslation);

	

	keyFrame->vTranslation.y = 0.f;
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
