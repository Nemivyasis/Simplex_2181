#include "AppClass.h"
void Application::InitVariables(void)
{
	int boxPoints[8][11] = {
		{0,0,1,0,0,0,0,0,1,0,0},
		{0,0,0,1,0,0,0,1,0,0,0},
		{0,0,1,1,1,1,1,1,1,0,0},
		{0,1,1,0,1,1,1,0,1,1,0},
		{1,1,1,1,1,1,1,1,1,1,1},
		{1,0,1,1,1,1,1,1,1,0,1},
		{1,0,1,0,0,0,0,0,1,0,1},
		{0,0,0,1,1,0,1,1,0,0,0}
	};

	boxCount = 0;
	for (int i = 0; i < 8; i++)
	{
		for (int j = 0; j < 11; j++)
		{
			if (boxPoints[i][j] == 1) {
				boxCount++;
				meshObj.push_back(new MyMesh());
				meshObj[boxCount - 1]->GenerateCube(1.0f, C_RED);
				buildTranslation.push_back(new vector3((5 - j), (3.5f - i), 0));
			}
		}
	}
	//Make MyMesh object
	m_pMesh = new MyMesh();
	m_pMesh->GenerateCube(2.0f, C_BROWN);

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
	// Clear the screen
	ClearScreen();

	for (int i = 0; i < boxCount; i++)
	{
		vector3* temp = buildTranslation[i];
		meshObj[i]->Render(m_pCameraMngr->GetProjectionMatrix(), m_pCameraMngr->GetViewMatrix(), glm::translate(*temp));
	}
	
	m_pMesh1->Render(m_pCameraMngr->GetProjectionMatrix(), m_pCameraMngr->GetViewMatrix(), glm::translate(vector3( 3.0f, 0.0f, 0.0f)));
		
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