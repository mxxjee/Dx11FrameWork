#include "CMainApp.h"

USING(Client)

CMainApp::CMainApp()
{
}

HRESULT CMainApp::Initialize()
{
	/*내 게임의 초기화 작업을 수행*/
	return S_OK;
}

void CMainApp::Update(_float fTimeDelta)
{
	/*내 게임의 반복적인 작업 수행*/
}

void CMainApp::Render()
{
	/*내 게임의 반복적인 렌더.*/
}

CMainApp* CMainApp::Create()
{
	//메인앱을 완벽하게 생성하기 위한 함수
	//new 뿐만아니라 초기화작업(Initialize)수행
	CMainApp* pMainApp = new CMainApp;

	if (FAILED(pMainApp->Initialize()))
	{
		/*생성하지 못했다면 Safe_Release()로 삭제*/
		MSG_BOX("Failed to Create : CMainApp");
		Safe_Release(pMainApp);
	}

	return pMainApp;
}

void CMainApp::Free()
{
	//자신의 리소스정리
	//상속계층을 따르기 위해 부모  Free호출 
	__super::Free();

}
