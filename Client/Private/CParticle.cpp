#include "CParticle.h"
#include "CVIBuffer_Particle_Point.h"
#include "CEffectData_Manager.h"
#include "CShader.h"
#include "CTexture.h"
#include "CEffectPoolManager.h"

USING(Client)

CParticle::CParticle(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	:CEffect(pDevice,pContext)
{
}

CParticle::CParticle(const CParticle& rhs)
	:CEffect(rhs)
{
}

HRESULT CParticle::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CParticle::Initialize_Copytype(void* pArg)
{
	PARTICLE_DESC* pDesc = static_cast<PARTICLE_DESC*>(pArg);

	if (FAILED(__super::Initialize_Copytype(pArg )))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	EffectData* pData = m_pEffectData_Manager->Find_Data(m_DataName);
	if (pData)
	{

		m_pParticleData = static_cast<ParticleEffetData*>(pData);

		m_pTransformCom->Rotation(_float3(m_pParticleData->InitRotation.x, m_pParticleData->InitRotation.y, m_pParticleData->InitRotation.z));
		m_pTransformCom->Set_Scale(m_pParticleData->InitScale);

		_vector vPos = m_pTransformCom->Get_State(STATE::POSITION);
		m_pTransformCom->Set_State(STATE::POSITION, vPos + XMVectorSetW(XMLoadFloat4(&m_pParticleData->InitOffSet), 0.f));



	}
	else
	{
		m_pParticleData = new ParticleEffetData();
		m_bNew = true;
	}

	if (FAILED(Ready_Resource()))
		return E_FAIL;

	m_pDataRef = m_pParticleData;
	CurrentScale = m_pDataRef->InitScale;
	CurrentMove = m_pDataRef->InitOffSet;
	m_PassName = pDesc->passName;

	Make_LocalMatrix();
	
	//버퍼 풀 초기화
	CVIBuffer_Particle::PARTICLE_DESC Desc;
	ZeroMemory(&Desc, sizeof(CVIBuffer_Particle::PARTICLE_DESC));

	Desc.iNumInstance = m_pParticleData->iNumInstance;
	Desc.vRange = m_pParticleData->vRange;
	Desc.vCenter = m_pParticleData->vCenter;
	Desc.vRange = m_pParticleData->vRange;
	Desc.vSizeRange_Start = m_pParticleData->vSizeRange_Start;
	Desc.vSizeRange_End = m_pParticleData->vSizeRange_End;
	Desc.vSpeedRange = m_pParticleData->vSpeedRange;
	Desc.vLifeTimeRange = m_pParticleData->vLifeTimeRange;

	if (m_pVIBufferCom)
		m_pVIBufferCom->Reset_Particle(Desc);

	//모든 이펙트 입자 초기화
	Reset_All_Particles();

	return S_OK;
}

void CParticle::Update(_float fTimeDelta)
{
	if (m_bStop)
		return;

	__super::Update(fTimeDelta);

	_bool bIsAnyAlive = false;

	vector<VTXPARTICLE> vInstanceData;
	vInstanceData.reserve(m_ParticlePool.size());

	for (auto& particle : m_ParticlePool)
	{
		//리스폰체크
		if (!particle.bAlive)
		{
			if (m_pParticleData->m_bLoop)
				Reset_Single_Particle(particle);
			else
				continue;

		}

		//살아있는상태라면,  계산해주기
		if (particle.bAlive)
		{
			//라이프타임계산
			particle.fAge += fTimeDelta;
			if (particle.fAge >= particle.fLifeTime)
			{
				particle.bAlive = false;
				continue;
			}

			bIsAnyAlive = true;

			_float vRatio = particle.fAge / particle.fLifeTime;
			
			_vector vPos = XMLoadFloat3(&particle.vPos);
			_vector vDir = XMLoadFloat3(&particle.vDir);
			_vector vGravity = XMVectorSet(0.f, m_pParticleData->fGravity, 0.f, 0.f);

			vPos += (vDir * particle.fSpeed * fTimeDelta) + (vGravity * fTimeDelta);
			XMStoreFloat3(&particle.vPos, vPos);


			particle.fRotation += particle.fRotationSpeed * fTimeDelta;

			_float2 vCurSize;
			XMStoreFloat2(&vCurSize, XMVectorLerp(XMLoadFloat2(&particle.vSizeStart), XMLoadFloat2(&particle.vSizeEnd), vRatio));

			_float4 vCurColor;
			XMStoreFloat4(&vCurColor, XMVectorLerp(XMLoadFloat4(&m_pParticleData->vColor), XMLoadFloat4(&m_pParticleData->vColor_End), vRatio));


			/*스프라이트 UV계산*/
			_float4 vUV_Info = _float4(0.f, 0.f, 1.f, 1.f);
			if (m_pParticleData->bIsSpriteAnim)
			{
				_uint iTotalFrames = (_uint)(m_pParticleData->vSpriteCount.x * m_pParticleData->vSpriteCount.y);
				_float fAnimTime = vRatio * m_pParticleData->vSpriteSpeed;
				_uint iCurFrame = (_uint)(fAnimTime * iTotalFrames) % iTotalFrames;

				_uint iCol = iCurFrame % (_uint)m_pParticleData->vSpriteCount.x;
				_uint iRow = iCurFrame / (_uint)m_pParticleData->vSpriteCount.x;

				vUV_Info.z = 1.f / m_pParticleData->vSpriteCount.x; // Scale U
				vUV_Info.w = 1.f / m_pParticleData->vSpriteCount.y; // Scale V
				vUV_Info.x = iCol * vUV_Info.z; // Offset U
				vUV_Info.y = iRow * vUV_Info.w; // Offset V
			}


			// 빌보드 회전은 GS에서 처리, 여기선 Z축 회전만 적용
			_matrix LocalMatrix = XMMatrixScaling(vCurSize.x, vCurSize.y, 1.f) * XMMatrixRotationZ(particle.fRotation) * XMMatrixTranslation(particle.vPos.x, particle.vPos.y, particle.vPos.z);
		
			
			//인스턴스데이터
			VTXPARTICLE tInstance;
			XMStoreFloat4(&tInstance.vRight, LocalMatrix.r[0]);
			XMStoreFloat4(&tInstance.vUp, LocalMatrix.r[1]);
			XMStoreFloat4(&tInstance.vLook, LocalMatrix.r[2]);
			XMStoreFloat4(&tInstance.vTranslation, LocalMatrix.r[3]);
			tInstance.vColor = vCurColor;
			tInstance.vUV_Info = vUV_Info;
			tInstance.vLifeTime = _float2(particle.fAge, particle.fLifeTime);

			vInstanceData.push_back(tInstance);

		}
	}

	//버퍼에갱신
	if (m_pVIBufferCom)
		m_pVIBufferCom->Update_Buffer(vInstanceData);

	if (!m_pParticleData->m_bLoop && !bIsAnyAlive)
	{
		// 이펙트 객체를 비활성화하고 풀로 반환
		Set_Active(false);
		if (m_pEffectPool_Manager)
			m_pEffectPool_Manager->Request_Return(this);
	}
}

void CParticle::Update_Late(_float fTimeDelta)
{
	__super::Update_Late(fTimeDelta);

}

void CParticle::Update_Render(_float fTimeDelta)
{
	__super::Update_Render(fTimeDelta);


}

HRESULT CParticle::Render()
{
	__super::Render();

	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	if (FAILED(m_pShader->Begin(m_PassName)))
		return E_FAIL;


	if (FAILED(m_pVIBufferCom->Bind_Resource()))
		return E_FAIL;          //IA단계     //IA단계

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;


	return S_OK;
}

HRESULT CParticle::Ready_Components()
{
	CComponent::COMPONENT_DESC Desc;
	Desc.pOwner = this;

	CComponent* pVIBuffer_Particle = dynamic_cast<CVIBuffer_Particle_Point*>(m_pGameInstance->Clone_Prototype(
		PROTOTYPE::COMPONENT, 0, PROTO_COMPONENT_NAME(L"VIBuffer_Particle_Point"), &Desc)
		);

	if (FAILED(Add_Component(
		COMPONENT_TYPE::PARTICLE,
		pVIBuffer_Particle,
		reinterpret_cast<CComponent**>(&m_pVIBufferCom)
	)))
		return E_FAIL;






	return S_OK;
}

HRESULT CParticle::Ready_Resource()
{
	Reset_Textures();

	return S_OK;
}


void CParticle::Reset_All_Particles()
{
	m_ParticlePool.clear();
	m_ParticlePool.resize(m_pParticleData->iNumInstance);

	vector<VTXPARTICLE> vInstanceData;
	vInstanceData.reserve(m_ParticlePool.size());

	_float fInterval = 0.f;
	if (m_pParticleData->iNumInstance > 1)
		fInterval = m_pParticleData->vLifeTimeRange.y / (_float)m_pParticleData->iNumInstance;

	CGameInstance* pInst = m_pGameInstance;

	//전체리셋(폭발형이면 모두 Alive=true, 지속형이면 Alive=false 후 순차생성)
	for (auto& particle : m_ParticlePool)
	{
		// 1. 기본값 리셋 (위치=Center, 나이=0)
		Reset_Single_Particle(particle);
		particle.bAlive = true;

		if (m_pParticleData->m_bLoop)
		{
			// 수명의 0% ~ 90% 사이 랜덤 시간만큼 이미 지났다고 가정
			_float fSafeLifeTime = particle.fLifeTime * 0.9f;
			_float fRandomAge = pInst->Random(0.f, fSafeLifeTime);

			// 나이 적용
			particle.fAge = fRandomAge;

			// 그 시간만큼 물리 이동 시뮬레이션
			_vector vPos = XMLoadFloat3(&particle.vPos);
			_vector vDir = XMLoadFloat3(&particle.vDir);
			_vector vGravity = XMVectorSet(0.f, m_pParticleData->fGravity, 0.f, 0.f);

			// 현재위치 += (방향 * 속도 * 시간) + (중력 * 시간)
			vPos += (vDir * particle.fSpeed * fRandomAge) + (vGravity * fRandomAge);

			// 회전도 미리 적용
			particle.fRotation += particle.fRotationSpeed * fRandomAge;

			XMStoreFloat3(&particle.vPos, vPos);
		}

		// 초기 데이터 강제 생성 (첫 프레임 렌더링용)


		// 진행률 계산
		_float fLifeRatio = (particle.fLifeTime > 0.f) ? (particle.fAge / particle.fLifeTime) : 0.f;

		// 크기 보간
		_float2 vCurSize;
		XMStoreFloat2(&vCurSize, XMVectorLerp(XMLoadFloat2(&particle.vSizeStart), XMLoadFloat2(&particle.vSizeEnd), fLifeRatio));

		// 색상 보간
		_float4 vCurColor;
		XMStoreFloat4(&vCurColor, XMVectorLerp(XMLoadFloat4(&m_pParticleData->vColor), XMLoadFloat4(&m_pParticleData->vColor_End), fLifeRatio));

		// UV 계산 (기본값) - 필요하면 Update와 똑같이 복사해서 넣으세요
		_float4 vUV_Info = _float4(0.f, 0.f, 1.f, 1.f);
		if (m_pParticleData->bIsSpriteAnim)
		{
			_uint iTotalFrames = (_uint)(m_pParticleData->vSpriteCount.x * m_pParticleData->vSpriteCount.y);
			_float fAnimTime = fLifeRatio * m_pParticleData->vSpriteSpeed;
			_uint iCurFrame = (_uint)(fAnimTime * iTotalFrames) % iTotalFrames;

			_uint iCol = iCurFrame % (_uint)m_pParticleData->vSpriteCount.x;
			_uint iRow = iCurFrame / (_uint)m_pParticleData->vSpriteCount.x;

			vUV_Info.z = 1.f / m_pParticleData->vSpriteCount.x;
			vUV_Info.w = 1.f / m_pParticleData->vSpriteCount.y;
			vUV_Info.x = iCol * vUV_Info.z;
			vUV_Info.y = iRow * vUV_Info.w;
		}

		// 행렬 계산 (순수 로컬)
		_matrix LocalMatrix = XMMatrixScaling(vCurSize.x, vCurSize.y, 1.f) * XMMatrixRotationZ(particle.fRotation) * XMMatrixTranslation(particle.vPos.x, particle.vPos.y, particle.vPos.z);

		// 인스턴스 데이터 생성 및 삽입
		VTXPARTICLE tInstance;
		XMStoreFloat4(&tInstance.vRight, LocalMatrix.r[0]);
		XMStoreFloat4(&tInstance.vUp, LocalMatrix.r[1]);
		XMStoreFloat4(&tInstance.vLook, LocalMatrix.r[2]);
		XMStoreFloat4(&tInstance.vTranslation, LocalMatrix.r[3]);
		tInstance.vColor = vCurColor;
		tInstance.vUV_Info = vUV_Info;
		tInstance.vLifeTime = _float2(particle.fAge, particle.fLifeTime);

		// [핵심] 여기서 데이터를 넣어줘야 Update_Buffer가 의미가 있습니다.
		vInstanceData.push_back(tInstance);
	}

	// 3. 강제 업로드 (이제 데이터가 들어있으므로 정상 작동)
	if (m_pVIBufferCom)
		m_pVIBufferCom->Update_Buffer(vInstanceData);
}

void CParticle::Reset_Single_Particle(PARTICLE_INFO& tParticle)
{
	tParticle.bAlive = true;
	tParticle.fAge = 0.f;

	CGameInstance* pInst = m_pGameInstance;

	// 랜덤 위치 (InitOffset을 Center로 사용하여 랜덤 범위 적용)
	_float fRX = pInst->Random(m_pParticleData->vRange.x * -0.5f, m_pParticleData->vRange.x * 0.5f);
	_float fRY = pInst->Random(m_pParticleData->vRange.y * -0.5f, m_pParticleData->vRange.y * 0.5f);
	_float fRZ = pInst->Random(m_pParticleData->vRange.z * -0.5f, m_pParticleData->vRange.z * 0.5f);

	tParticle.vPos = _float3(m_pParticleData->InitOffSet.x + fRX,
		m_pParticleData->InitOffSet.y + fRY,
		m_pParticleData->InitOffSet.z + fRZ);

	// 속도, 수명
	tParticle.fSpeed = pInst->Random(m_pParticleData->vSpeedRange.x, m_pParticleData->vSpeedRange.y);
	tParticle.fLifeTime = pInst->Random(m_pParticleData->vLifeTimeRange.x, m_pParticleData->vLifeTimeRange.y);

	// 방향
	if (m_pParticleData->bUseRandomDir)
	{
		_float fDirX = pInst->Random(-1.f, 1.f);
		_float fDirY = pInst->Random(-1.f, 1.f);
		_float fDirZ = pInst->Random(-1.f, 1.f);
		XMStoreFloat3(&tParticle.vDir, XMVector3Normalize(XMVectorSet(fDirX, fDirY, fDirZ, 0.f)));
	}
	else
	{
		tParticle.vDir = m_pParticleData->vMoveDir;
	}

	// 회전
	tParticle.fRotation = 0.f;
	tParticle.fRotationSpeed = pInst->Random(m_pParticleData->vRotationSpeedRange.x, m_pParticleData->vRotationSpeedRange.y);

	// 크기 (데이터 원본값 사용)
	tParticle.vSizeStart = m_pParticleData->vSizeRange_Start;
	tParticle.vSizeEnd = m_pParticleData->vSizeRange_End;
}

void CParticle::Render_TextureList()
{
	const vector<CTexture*> Textures=m_pEffectData_Manager->Get_Textures();
	const vector<wstring> TextureKeyList = m_pEffectData_Manager->Get_TextureKeyList();

	for (_uint i = 0; i <ENUM_TO_UINT(EFFECT_TEXTYPE::END); ++i)
	{
		//내가 선택한 텍스처를표시한다.
		ImGui::PushID((int)i); // ID 충돌 방지
		if (m_Textures[i])
		{
			ID3D11ShaderResourceView* pSRV = m_Textures[i]->Get_SRV(0).Get();
			if (pSRV)
			{
				if (ImGui::ImageButton("##TextureBtn", (ImTextureID)pSRV, ImVec2(64, 64)))
				{
					ImGui::OpenPopup("TextureSelectPopup");
				}
			}
			
			else
			{
				// 이미지가 없으면 텍스트 버튼
				if (ImGui::Button("No Image", ImVec2(64, 64)))
					ImGui::OpenPopup("TextureSelectPopup");
			}

			ImGui::SameLine();
			string strTexKey = (m_pParticleData->TexKey[i] == L"") ? "NONE" : WStringToUTF8(m_pParticleData->TexKey[i]);
			ImGui::Text("Slot %d\n%s", i, strTexKey.c_str());


			//텍스처선택팝업창
			if (ImGui::BeginPopup("TextureSelectPopup"))
			{
				ImGui::Text("Select Texture");
				ImGui::Separator();

				// 목록을 그리드로 보여주기 (한 줄에 4개씩)
				int columns = 4;
				float thumbnailSize = 50.0f;

				for (size_t k = 0; k < Textures.size(); ++k)
				{
					
					string keyName = WStringToUTF8(TextureKeyList[k]);
					ID3D11ShaderResourceView* pPopupSRV = Textures[k]->Get_SRV(0).Get();

					// SRV가 있어야 그림
					if (pPopupSRV)
					{
						ImGui::PushID((int)k);
						// 클릭 시 데이터 변경
						if (ImGui::ImageButton("##TexSelectBtn",(ImTextureID)pPopupSRV, ImVec2(thumbnailSize, thumbnailSize)))
						{
							
							m_pParticleData->TexKey[i] = TextureKeyList[k]; // 키값 변경
							Reset_Textures();
							ImGui::CloseCurrentPopup(); // 팝업 닫기
						}
						ImGui::PopID(); // 팝업 내부 Pop
						// 마우스 올리면 이름 툴팁
						if (ImGui::IsItemHovered())
							ImGui::SetTooltip("%s", keyName.c_str());

						// 그리드 정렬 (마지막 아이템이 아니면 SameLine)
						if ((k + 1) % columns != 0)
							ImGui::SameLine();
					}
				}
				ImGui::EndPopup();
			}

			
		}

		ImGui::PopID();
	}
}

CParticle* CParticle::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
	CParticle* pInstance = new CParticle(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CParticle");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CParticle::Clone(void* pArg)
{
	CParticle* pInstance = new CParticle(*this);
	if (FAILED(pInstance->Initialize_Copytype(pArg)))
	{
		MSG_BOX("Failed to Created : CParticle");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CParticle::Free()
{
	__super::Free();
	
	if (m_bNew)
		Safe_Delete(m_pParticleData);

	for (auto& pTex : m_Textures)
		Safe_Release(pTex);
	

	Safe_Release(m_pVIBufferCom);
}

void CParticle::Play()
{	
	if (!m_bActive)
		Set_Active(true);

	m_bStop = false;

	// 다시 처음부터 재생
	Reset_All_Particles();
}

void CParticle::Stop()
{
	__super::Stop();
	m_bStop = true;
	// 필요시 풀 비우기
}

#ifdef _DEBUG
void CParticle::Render_DebugImgui()
{
	const char* items[] = { "MESH", "PARTICLE" };
	int current_item = (int)m_pDataRef->eType; // 현재 내 타입

	if (ImGui::Combo("Effect Type", &current_item, items, IM_ARRAYSIZE(items)))
	{
		EFFECT_DESC_TYPE eNewType = (EFFECT_DESC_TYPE)current_item;
		m_pDataRef->eType = eNewType;
		m_pEffectData_Manager->Update_Data(m_DataName, m_pDataRef);

	}


	if (ImGui::DragFloat("LifeTime", (float*)&m_pDataRef->fLifeTime))
	{
		m_pEffectData_Manager->Update_Data(m_DataName, m_pDataRef);

	}

	if (ImGui::ColorEdit4("Color", (float*)&m_pDataRef->vColor))
	{
		m_bNeedToReset=true;
	
	}

	if (ImGui::DragFloat("Itensity", (float*)&m_pDataRef->fIntensity))
	{
		m_pEffectData_Manager->Update_Data(m_DataName, m_pDataRef);

	}

	if (ImGui::DragFloat4("InitOffSet(Center)", (float*)&m_pDataRef->InitOffSet))
	{
		m_bNeedToReset = true;
		Make_LocalMatrix();
		//Update_Matrix();
	}

	if (ImGui::Checkbox("Loop", (bool*)&m_pDataRef->m_bLoop))
	{
		m_pEffectData_Manager->Update_Data(m_DataName, m_pDataRef);

	}

	//개수설정
	if (ImGui::DragInt("Num Instance", (_int*)&m_pParticleData->iNumInstance, 1, 1, 5000))
		m_bNeedToReset = true;

	//스폰범위설정
	if (ImGui::DragFloat3("Range", (float*)&m_pParticleData->vRange, 0.1f))
		m_bNeedToReset = true;

	//크기설정
	if (ImGui::DragFloat2("Size Start", (float*)&m_pParticleData->vSizeRange_Start, 0.01f))
		m_bNeedToReset = true;
	if (ImGui::DragFloat2("Size End", (float*)&m_pParticleData->vSizeRange_End, 0.01f))
		m_bNeedToReset = true;

	//속도,방향
	if (ImGui::DragFloat2("Speed Range", (float*)&m_pParticleData->vSpeedRange, 0.1f))
		m_bNeedToReset = true;
	if (ImGui::Checkbox("Random Dir", &m_pParticleData->bUseRandomDir))
		m_bNeedToReset = true;

	if (ImGui::DragFloat2("LifeTime Range", (float*)&m_pParticleData->vLifeTimeRange, 0.1f))
		m_bNeedToReset = true;

	if (!m_pParticleData->bUseRandomDir)
		if (ImGui::DragFloat3("Fixed Dir", (float*)&m_pParticleData->vMoveDir, 0.01f))
			m_bNeedToReset = true;

	//텍스처목록
	Render_TextureList();

	if (ImGui::Button("Play"))
		Play();


	if (ImGui::Button("Stop"))
		Stop();

	if (ImGui::Button("Apply & Save"))
	{
		// 데이터 저장
		m_pEffectData_Manager->Update_Data(m_DataName, m_pParticleData);
		m_pEffectData_Manager->Save_To_Json(m_DataName, m_pDataRef);
		m_bNeedToReset = true; // 강제 리셋
	}


	if (m_bNeedToReset)
	{
		// 1. 매니저 업데이트 (안전장치)
		m_pEffectData_Manager->Update_Data(m_DataName, m_pParticleData);

		// 2. 엔진 구조체 매핑
		CVIBuffer_Particle::tagVIBuffer_Particle_Desc Desc;
		ZeroMemory(&Desc, sizeof(CVIBuffer_Particle::tagVIBuffer_Particle_Desc));
		Desc.iNumInstance = m_pParticleData->iNumInstance;
		Desc.vRange = m_pParticleData->vRange;
		Desc.vCenter = _float3(m_pParticleData->InitOffSet.x, m_pParticleData->InitOffSet.y, m_pParticleData->InitOffSet.z);
		Desc.vSizeRange_Start = m_pParticleData->vSizeRange_Start;
		Desc.vSizeRange_End = m_pParticleData->vSizeRange_End;
		Desc.vSpeedRange = m_pParticleData->vSpeedRange;
		Desc.vLifeTimeRange = m_pParticleData->vLifeTimeRange;

		// 3. 버퍼 재생성
		m_pVIBufferCom->Reset_Particle(Desc);

		// 4. 풀 리셋
		Reset_All_Particles();
		m_bNeedToReset = false;
	}

}
#endif

HRESULT CParticle::Bind_ShaderResources()
{
	if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", CombinedMatrix)))
		return E_FAIL;

	_float4 vTime = _float4(m_fProgress, 0.f, 0.f, 0.f);
	m_pGameInstance->CopyData_Buffer("LoadingBuffer", &vTime, sizeof(_float4));

	if(m_Textures[ENUM_TO_UINT(EFFECT_TEXTYPE::DIFFUSE)])
		if (FAILED(m_Textures[ENUM_TO_UINT(EFFECT_TEXTYPE::DIFFUSE)]->Bind_ShaderResource(m_pShader, "g_DiffuseTexture", 0)))
			return E_FAIL;


	return S_OK;
}

void CParticle::Reset_Textures()
{
	if (m_pParticleData->TexKey[ENUM_TO_UINT(EFFECT_TEXTYPE::DIFFUSE)] != L"")
	{
		CTexture* pDiffuseTex = m_pGameInstance->Find_Texture(m_pParticleData->TexKey[ENUM_TO_UINT(EFFECT_TEXTYPE::DIFFUSE)]);
		if (pDiffuseTex)
			m_Textures[ENUM_TO_UINT(EFFECT_TEXTYPE::DIFFUSE)] = pDiffuseTex;

	}

	if (m_pParticleData->TexKey[ENUM_TO_UINT(EFFECT_TEXTYPE::NOISE)] != L"")
	{
		CTexture* pNoiseTex = m_pGameInstance->Find_Texture(m_pParticleData->TexKey[ENUM_TO_UINT(EFFECT_TEXTYPE::NOISE)]);
		if (pNoiseTex)
			m_Textures[ENUM_TO_UINT(EFFECT_TEXTYPE::NOISE)] = pNoiseTex;

	}
	
	if (m_pParticleData->TexKey[ENUM_TO_UINT(EFFECT_TEXTYPE::ALPHAMASK)] != L"")
	{
		CTexture* pAlphaMaskTex = m_pGameInstance->Find_Texture(m_pParticleData->TexKey[ENUM_TO_UINT(EFFECT_TEXTYPE::ALPHAMASK)]);
		if (pAlphaMaskTex)
			m_Textures[ENUM_TO_UINT(EFFECT_TEXTYPE::ALPHAMASK)] = pAlphaMaskTex;
	}
	


}
