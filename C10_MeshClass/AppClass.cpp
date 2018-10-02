#include "AppClass.h"
void Application::InitVariables(void)
{
	////Make MyMesh object
	//m_pMesh = new MyMesh();
	//m_pMesh->GenerateCube(2.0f, C_BROWN);

	//Make MyMesh object
	m_pMesh1 = new MyMesh();
	m_pMesh1->GenerateCube(1.0f, C_WHITE);
}
void Application::Update(void)
{
	//Update the system so it knows how much time has passed since the last call
	m_pSystem->Update();

	//Is the arcball active?
	ArcBall();

	//Is the first person camera active?
	CameraRotation();
}
void Application::Display(void)
{
	vector3 v3InitialPoint = vector3(0, 0, 0);
	vector3 v3EndPoint = vector3(5, 0, 0);

	// Clear the screen
	ClearScreen();
	static DWORD DStartingTime = GetTickCount();
	DWORD DCurrentTime;
	DCurrentTime = GetTickCount();

	float deltaTime = (DCurrentTime - DStartingTime) / 1000.0f;



	static int pointTravelTime = 2;
	int runThrough = floorf(deltaTime / pointTravelTime);
	float timer = (deltaTime / pointTravelTime - floorf(deltaTime / pointTravelTime)) ;

	if (runThrough % 2 == 1) {
		timer = 1 - timer;
	}
	std::cout << runThrough << std::endl;
	vector3 v3Position = glm::lerp(v3InitialPoint, v3EndPoint, timer);
	matrix4 m4Position = glm::translate(IDENTITY_M4, v3Position);

	m_pMesh1->Render(m_pCameraMngr->GetProjectionMatrix(), m_pCameraMngr->GetViewMatrix(), m4Position);
		

	// draw a skybox
	m_pMeshMngr->AddSkyboxToRenderList();
	
	//render list call
	m_uRenderCallCount = m_pMeshMngr->Render();

	//clear the render list
	m_pMeshMngr->ClearRenderList();
	
	//draw gui
	DrawGUI();
	
	//end the current frame (internally swaps the front and back buffers)
	m_pWindow->display();


}
void Application::Release(void)
{
	if (m_pMesh != nullptr)
	{
		delete m_pMesh;
		m_pMesh = nullptr;
	}
	SafeDelete(m_pMesh1);
	//release GUI
	ShutdownGUI();
}