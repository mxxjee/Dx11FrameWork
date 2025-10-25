#include "CImgui_DataManager.h"
#include "MapTool_Defines.h"

USING(MapTool)
IMPLEMENT_SINGLETON(CImgui_DataManager)

CImgui_DataManager::CImgui_DataManager()
{
}

CImgui_DataManager::~CImgui_DataManager()
{
}

vector<wstring> CImgui_DataManager::GetImageFiles(const wstring& folderPath)
{
	vector<wstring> result;

	for (auto& entry : fs::recursive_directory_iterator(folderPath))
	{
		//올바르지 않은 파일이라면 continue
		if (!fs::is_regular_file(entry))
			continue;
		                                                                                                                        

		auto path = entry.path();
		auto ext = path.extension().wstring();

		if (ext == L".png" || ext == L".jpg" || ext == L".dds")
			result.push_back(path.wstring());

	}
	return result;

}

void CImgui_DataManager::Active_PlacementMode(PlaceObjectInfo Info)
{
	Data.m_bPlacementMode = true;
	m_PlaceObjInfo = Info;
}

void CImgui_DataManager::Update_MouseInput()
{
}

void CImgui_DataManager::Free()
{
	__super::Free();
}
