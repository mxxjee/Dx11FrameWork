#pragma once
#include "Engine_Define.h"

namespace MathUtils
{
	static _vector WorldToScreen(_vector& vWorldPos,
		_float4x4& view, _float4x4& proj, int screenWidth, int screenHeight)
	{
		//View,Proj 행렬을 연산용 데이터로 바꾸기.
		_matrix matView = XMLoadFloat4x4(&view);
		_matrix matProj = XMLoadFloat4x4(&proj);

		_matrix matViewProj = XMMatrixMultiply(matView, matProj); // view * proj
		vWorldPos=XMVector3TransformCoord(vWorldPos, matViewProj);


		_float4 screenPos;
		screenPos.x = XMVectorGetX(vWorldPos) + (screenWidth * 0.5f);
		screenPos.y = XMVectorGetY(vWorldPos) - (screenHeight * 0.5f);
		screenPos.z = XMVectorGetZ(vWorldPos);


		return XMLoadFloat4(&screenPos);


	}

	//현재 활성화된 view,proj행렬넣기
	//UI Obj 좌표 -> World (view : Identity, proj : 직교투영)
	//마우스클릭 -> (View : camera view/ proj:원근투영)

	static _vector ScreenToWorld(_vector& vScreenPos,
		_float4x4& view, _float4x4& proj, int screenWidth, int screenHeight)
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
}