#include "AppClass.h"
void Application::InitVariables(void)
{
	//Make MyMesh object
	m_pMesh = new MyMesh();
	m_pMesh->GenerateCube(2.0f, C_BROWN);
	//read the file with word list
	std::ifstream readFile;
	readFile.open("Shape.txt");

	bool successfulFileLoad = false;

	int** boxPointArray = nullptr;
	int cols;
	int rows;
	//read each line and set the success boolean to true TODO finish
	if (readFile.is_open())
	{
		std::string line;
		std::getline(readFile, line);
		cols = std::stoi(line);

		std::getline(readFile, line);
		rows = std::stoi(line);

		boxPointArray = new int*[rows];
		for (int i = 0; i < rows; i++)
		{
			boxPointArray[i] = new int[cols];
		}

		for (int i = 0; i < rows; i++)
		{
			std::getline(readFile, line);
			for (int j = 0; j < cols; j++)
			{
				boxPointArray[i][j] = ((int)line[j]) - 48;
			}
		}

		readFile.close();

		successfulFileLoad = true;
	}

	for (int i = 0; i < rows; i++)
	{
		for (int j = 0; j < cols; j++)
		{
			std::cout << boxPointArray[i][j];
		}
		std::cout << std::endl;
	}
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

	m_pMesh->Render(m_pCameraMngr->GetProjectionMatrix(), m_pCameraMngr->GetViewMatrix(), ToMatrix4(m_qArcBall));
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