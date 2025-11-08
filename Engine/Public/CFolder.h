#pragma once
#include "CBase.h"


NS_BEGIN(Engine)
class CTexture;
class CGameInstance;

class ENGINE_DLL CFolder :
    public CBase
{
public:
    typedef struct tagFolderDesc
    {
        char  Name[MAX_PATH];
        ImVec2 Size;
        int         iIdx = 0;
        string    Category = "";

    };
public:
    CFolder(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
        :m_pDevice(pDevice), m_pContext(pContext)
    {

    }
    virtual ~CFolder() = default;

public:
    HRESULT         Initialize(void* pArg);
    bool            Update();
    void            Render();
    int             Get_Idx() { return m_iIdx; }

public:
    const char* Get_Name() { return m_szName; }
    const string& Get_Category() { return m_Category; }

    vector<tagAssetInfo>* get_vector() { return &m_Vec; }


    void        Set_InfoVector(const vector<AssetInfo>& Vec) { m_Vec = Vec; }
    void        Add_Info(const tagAssetInfo& Info) { m_Vec.push_back(Info); }
    tagAssetInfo        Show_Folder();      //폴더 안의 것들을 imgui 로 표시한다.
public:
    static CFolder* Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, void* pArg);
    virtual void Free();

private:
    ComPtr<ID3D11Device>            m_pDevice;
    ComPtr<ID3D11DeviceContext>     m_pContext;



private:
    CGameInstance* m_pGameInstance = nullptr;

    class CTexture* m_pTexture; //폴더 아이콘표시
    vector<tagAssetInfo>       m_Vec;
    char                        m_szName[MAX_PATH];
    ImVec2                      m_iSize;
    int                         m_iIdx = 0;
    string                     m_Category = "";


    bool                    m_bClicked = false;
};

NS_END

