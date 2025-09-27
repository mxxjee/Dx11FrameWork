#pragma once

#include "Engine_Define.h"
#include "VertexData.h"

NS_BEGIN(Engine)

template <typename T>
class CGeometry;


class ENGINE_DLL GeometryHelper
{
public:
	static void		CreateRectangle(shared_ptr<CGeometry<VertexColorData>> geometry, _float4 color);
	static void		CreateRectangle(shared_ptr<CGeometry<VertexTextureData>> geometry);


	static void CreateTriangle(shared_ptr<CGeometry<VertexColorData>> geometry, _float4 color);
};
NS_END

