#pragma once
#include "Engine_Define.h"
#include "CGameObject.h"
#include "CGameInstance.h"


namespace MathUtils
{
	//UI오브젝트를 화면에 띄우기 위해 transform에게 worldpos를 전달할때, 변환용(UI)
	//이떄 vScreenPos는 좌상단 (0,0)을 기준으로 한 스크린좌표를 넣는다.
	static _float4 ScreenToWorld_UI(const _vector& vScreenPos, FLOAT fViewPortWidth, FLOAT fViewPortHeight)
	{
		_float4 Result;
		Result.x = XMVectorGetX(vScreenPos) - fViewPortWidth * 0.5f;
		Result.y = ((-1) * XMVectorGetY(vScreenPos)) + fViewPortHeight * 0.5f;
		Result.z = XMVectorGetZ(vScreenPos);
		Result.w = 1;


		return Result;


	}

	//월드좌표를 스크린좌표로 변환
	static _vector WorldToScreen(const _vector& vWorldPos,
		const _float4x4& view, const _float4x4& proj, int screenWidth, int screenHeight)
	{
		//View,Proj 행렬을 연산용 데이터로 바꾸기.
		_matrix matView = XMLoadFloat4x4(&view);
		_matrix matProj = XMLoadFloat4x4(&proj);

		_matrix matViewProj = XMMatrixMultiply(matView, matProj); // view * proj
		_vector NewWorldPos = XMVector3TransformCoord(vWorldPos, matViewProj);


		_float4 screenPos;
		screenPos.x = XMVectorGetX(NewWorldPos) + (screenWidth * 0.5f);
		screenPos.y = XMVectorGetY(NewWorldPos) + (screenHeight * 0.5f);
		screenPos.z = XMVectorGetZ(NewWorldPos);


		return XMLoadFloat4(&screenPos);


	}

	//현재 활성화된 view,proj행렬넣기
	//UI Obj 좌표 -> World (view : Identity, proj : 직교투영)
	//마우스클릭 -> (View : camera view/ proj:원근투영)

	static _vector ScreenToWorld(const _vector& vScreenPos,
		const _float4x4& view, const _float4x4& proj, int screenWidth, int screenHeight)
	{
		// 1. Screen → NDC
		float ndcX = (XMVectorGetX(vScreenPos) / screenWidth) * 2.0f - 1.0f;
		float ndcY = 1.0f - (XMVectorGetY(vScreenPos) / screenHeight) * 2.0f;
		float ndcZ = XMVectorGetZ(vScreenPos);
		_vector ndcPos = XMVectorSet(ndcX, ndcY, ndcZ, 1.0f);

		// 2. NDC → View (투영 역행렬)
		_matrix projInv = XMMatrixInverse(nullptr, XMLoadFloat4x4(&proj));
		_vector viewPos = XMVector4Transform(ndcPos, projInv);
		viewPos /= XMVectorGetW(viewPos);   // perspective divide

		// 3. View → World (뷰 역행렬)
		_matrix viewInv = XMMatrixInverse(nullptr, XMLoadFloat4x4(&view));
		_vector worldPos = XMVector3TransformCoord(viewPos, viewInv);

		return worldPos;

	}

	static _float3 QuaternionToEuler(_vector vQuaternion)
	{
		XMFLOAT4 q;
		XMStoreFloat4(&q, vQuaternion);

		// build 쿼터니언 -> 오일러 변환
		float sinr_cosp = 2 * (q.w * q.x + q.y * q.z);
		float cosr_cosp = 1 - 2 * (q.x * q.x + q.y * q.y);
		float roll = atan2f(sinr_cosp, cosr_cosp); // X

		float sinp = 2 * (q.w * q.y - q.z * q.x);
		float pitch;
		if (fabs(sinp) >= 1)
			pitch = copysignf(XM_PIDIV2, sinp);
		else
			pitch = asinf(sinp); // Y

		float siny_cosp = 2 * (q.w * q.z + q.x * q.y);
		float cosy_cosp = 1 - 2 * (q.y * q.y + q.z * q.z);
		float yaw = atan2f(siny_cosp, cosy_cosp); // Z

		// 라디안 -> 도 변환
		float degX = XMConvertToDegrees(roll);
		float degY = XMConvertToDegrees(pitch);
		float degZ = XMConvertToDegrees(yaw);

		return _float3(degX, degY, degZ);
	}
	template <typename T>
	inline T Clamp(T value, T min, T max)
	{
		if (value < min) return min;
		if (value > max) return max;
		return value;
	}

	inline float Lerp(float Start, float Target, float t)
	{
		return Start + (Target - Start) * t;
	}

	inline Ray CreateRay(HWND hWnd, ComPtr<ID3D11DeviceContext> m_pContext, CGameObject* pOther)
	{
		Ray newRay;

		CGameInstance* pGameInstance = CGameInstance::GetInstance();

		POINT ptMouse{};
		GetCursorPos(&ptMouse);
		ScreenToClient(hWnd, &ptMouse);

		//뷰포트->투영
		_vector vMousePos = {};

		D3D11_VIEWPORT          ViewportDesc{};
		_uint           iNumViewports = { 1 };
		m_pContext->RSGetViewports(&iNumViewports, &ViewportDesc);

		vMousePos=XMVectorSet(ptMouse.x / (ViewportDesc.Width * 0.5f) - 1.f,
			ptMouse.y / -(ViewportDesc.Height * 0.5f) + 1.f,
			0,
			1.f);

		// 투영 -> 뷰스페이스
		_matrix	matProj = XMMatrixIdentity();
		matProj=XMLoadFloat4x4(&pGameInstance->Get_ProjMatrix(ENUM_TO_UINT(CAMERA_TYPE::FREE)));
		matProj=XMMatrixInverse(nullptr, matProj);
		vMousePos=XMVector3TransformCoord(vMousePos, matProj);


		//뷰스페이스->월드
		_matrix	matView = {};


		matView=XMLoadFloat4x4(&pGameInstance->Get_ViewMatrix(ENUM_TO_UINT(CAMERA_TYPE::FREE)));
		matView=XMMatrixInverse(nullptr, matView);

		_vector	vRayPos{ 0.f, 0.f, 0.f };		// 뷰 스페이스
		_vector	vRayDir = vMousePos - vRayPos;

		vRayPos=XMVector3TransformCoord(vRayPos, matView);
		vRayDir= XMVector3TransformNormal(vRayDir, matView);


		// 월드 -> 로컬
		_matrix	matWorld = {};

		matWorld = XMLoadFloat4x4(&(pOther->Get_Transform()->Get_World(TransformScope::WORLD)));
		matWorld=XMMatrixInverse(nullptr, matWorld);


		vRayPos=XMVector3TransformCoord(vRayPos, matWorld);
		vRayDir= XMVector3TransformNormal(vRayDir, matWorld);

		newRay.Dir = XMVector3Normalize(vRayDir);
		newRay.Origin = vRayPos;

		return newRay;


	}
}