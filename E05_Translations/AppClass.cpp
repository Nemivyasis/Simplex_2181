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
	static float moveTime = 0;
	moveTime += 0.01f;

	for (int i = 0; i < boxCount; i++)
	{
		vector3* temp = buildTranslation[i];
		matrix4 translationMatrix = glm::scale(IDENTITY_M4, vector3(2, 2, 2));
		translationMatrix = glm::translate(translationMatrix, *buildTranslation[i]);
		translationMatrix = glm::translate(translationMatrix, vector3(moveTime, sinf(moveTime), 0));

		meshObj[i]->Render(m_pCameraMngr->GetProjectionMatrix(), m_pCameraMngr->GetViewMatrix(), translationMatrix);
	}
		
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
	for (int i = boxCount - 1 ; i >= 0; i--)
	{
		SafeDelete(meshObj[i]);
		SafeDelete(buildTranslation[i]);
	}
	//release GUI
	ShutdownGUI();
}