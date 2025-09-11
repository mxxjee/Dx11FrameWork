#include "CMainApp.h"

USING(Client)

CMainApp::CMainApp()
{
}

void CMainApp::Free()
{
	//자신의 리소스정리
	//상속계층을 따르기 위해 부모  Free호출 
	__super::Free();

}
