#pragma once
#include "CBase.h"

/*각 레벨별로의 네브메쉬들을 보관한다..*/
/*현재 레벨에 맞는 네브메쉬 목록을 Get할 수 있음*/
/*NavMeshcomponent가 이 매니저에게 요청해서 셀 리스트받아오기.*/
NS_BEGIN(Engine)
class CCell;
class CGameInstance;

class CNavMesh_Manager :
    public CBase
{
private:
    CNavMesh_Manager(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pContext);
    virtual ~CNavMesh_Manager() = default;


public:
    HRESULT        Initialize();

#ifdef _DEBUG
    HRESULT          Render();//메인 셀들을 렌더한다.

public:
    void    Set_Shader(class CShader* pShader);
private:
    class CShader* m_pShader = { nullptr };
#endif

public:
    HRESULT             Load_NavMesh(_uint iLevelIdx, const string& Filepath);
    vector<CCell*>*     Find_Cells(_uint iLevelIdx);
    void                Set_ParentMatrix(_float4x4* pParent) { m_pParentMatrix = pParent; }
    void                Set_DrawDebug(bool b) { m_bDrawDebug = b; }

public:
    //레벨아이디로 들어온 셀들을 메인셀로 설정한다. 레벨 onenter마다 부르기? 
    void            Set_MainCells(_uint LevelID);
    

public:
    vector<CCell*>*   Get_MainCells() { return m_MainCells; }
    _float4x4*        Get_ParentMatrix() { return m_pParentMatrix; }
public:
    static CNavMesh_Manager* Create(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pContext);
    virtual void                Free() override;

private:
    vector<CCell*>*             m_MainCells = nullptr;
    UMap<_uint, vector<CCell*>> m_LevelCells;
    _float4x4*                  m_pParentMatrix=nullptr;
    bool                        m_bDrawDebug = true;


private:
    ComPtr<ID3D11Device>		m_pDevice = { nullptr };
    ComPtr<ID3D11DeviceContext>	m_pDeviceContext = { nullptr };

private:
    class CGameInstance* m_pGameInstance = { nullptr };

private:
    _float4x4           m_IdentityMatrix;
};

NS_END
