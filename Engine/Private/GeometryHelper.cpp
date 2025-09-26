#include "GeometryHelper.h"
#include "CGeometry.h"

void GeometryHelper::CreateRectangle(shared_ptr<CGeometry<VertexColorData>> geometry, _float4 color)
{
	vector<VertexColorData>		_vertices;
	_vertices.resize(4);

	//정점을 정의
		//32
	_vertices[0].position = _float3(-0.5f, 0.5f, 0);
	_vertices[0].color = color;

	_vertices[1].position = _float3(0.5f, 0.5f, 0);
	_vertices[1].color = color;

	_vertices[2].position = _float3(0.5f, -0.5f, 0);
	_vertices[2].color = color;

	_vertices[3].position = _float3(-0.5f, -0.5f, 0);
	_vertices[3].color = color;
	geometry->SetVertices(_vertices);


	vector<UINT32> indices = { 0,1,2,0,2,3 };
	geometry->SetIndices(indices);
}

void GeometryHelper::CreateRectangle(shared_ptr<CGeometry<VertexTextureData>> geometry)
{
	vector<VertexTextureData>		_vertices;
	_vertices.resize(4);

	//정점을 정의
		//32
	_vertices[0].position = _float3(-0.5f, 0.5f, 0);
	_vertices[0].uv = _float2(0.f,0.f);

	_vertices[1].position = _float3(0.5f, 0.5f, 0);
	_vertices[1].uv = _float2(1.f, 0.f);

	_vertices[2].position = _float3(0.5f, -0.5f, 0);
	_vertices[2].uv = _float2(0.f, 1.f);

	_vertices[3].position = _float3(-0.5f, -0.5f, 0);
	_vertices[3].uv = _float2(1.f,1.f);
	geometry->SetVertices(_vertices);


	vector<UINT32> indices = { 0,1,2,0,2,3 };
	geometry->SetIndices(indices);
}

void GeometryHelper::CreateTriangle(shared_ptr<CGeometry<VertexColorData>> geometry, _float4 color)
{

	vector<VertexColorData> _vertices;
	_vertices.resize(3);

	////[정점 정의, 아직 CPU영역에 존재하는 정점들]
	_vertices[0].position = _float3(-0.5f, -0.5f, 0);
	_vertices[0].color = _float4(1.f, 0.f, 0.f, 1.f);

	_vertices[1].position = _float3(0, 0.5f, 0);
	_vertices[1].color = _float4(0.f, 1.f, 0.f, 1.f);

	_vertices[2].position = _float3(0.5f, -0.5f, 0);
	_vertices[2].color = _float4(0.f, 0.f, 1.f, 1.f);
	geometry->SetVertices(_vertices);


	vector<UINT32> indices = { 0,1,2, };
	geometry->SetIndices(indices);
}
