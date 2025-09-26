#pragma once
#include "CBase.h"

NS_BEGIN(Engine)
template <typename T>
class ENGINE_DLL CGeometry :
    public CBase
{
public:
    CGeometry();
    ~CGeometry();


public:
    UINT32   GetVertexCount() { return static_cast<UINT32>(_vertices.size()); }
    void* GetVertexData() { return _vertices.data(); }
    const vector<T>& GetVertices() { return _vertices; }

    UINT32   GetIndexCount() { return static_cast<UINT32>(_Indices.size()); }
    void* GetIndexData() { return _Indices.data(); }
    const vector<UINT32>& GetIndices() { return _Indices; }


public:
    //V.pushback()
    void        AddVertex(const T& vertex) { _vertices.push_back(vertex); }
    
    //V.pushback(vector)
    void        AddVertices(const vector<T>& vTemp) { _vertices.insert(_vertices.end(), vTemp.begin(), vTemp.end()); }
    void        SetVertices(const vector<T>& vTemp) { _vertices = vTemp; }


public:
    void        AddIndex(UINT32 index) { _Indices.push_back(index); }
    void        AddIndices(const vector<UINT32>& _vTmp) { _Indices.insert(_Indices.end(), _vTmp.begin(), _vTmp.end()); }
    void        SetIndices(const vector<UINT32>& _vTmp) { _Indices = _vTmp; }
    


private:
    vector<T>       _vertices;  //정점배열
    vector<UINT32>  _Indices;
};

NS_END

template<typename T>
inline CGeometry<T>::CGeometry()
{
}

template<typename T>
inline CGeometry<T>::~CGeometry()
{
}
