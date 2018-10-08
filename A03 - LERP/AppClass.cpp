#include "AppClass.h"
void Application::InitVariables(void)
{
	//Change this to your name and email
	m_sProgrammer = "Alberto Bobadilla - labigm@rit.edu";
	
	//Set the position and target of the camera
	//(I'm at [0,0,10], looking at [0,0,0] and up is the positive Y axis)
	m_pCameraMngr->SetPositionTargetAndUpward(AXIS_Z * 20.0f, ZERO_V3, AXIS_Y);

	//if the light position is zero move it
	if (m_pLightMngr->GetPosition(1) == ZERO_V3)
		m_pLightMngr->SetPosition(vector3(0.0f, 0.0f, 3.0f));

	//if the background is cornflowerblue change it to black (its easier to see)
	if (vector3(m_v4ClearColor) == C_BLUE_CORNFLOWER)
	{
		m_v4ClearColor = vector4(ZERO_V3, 1.0f);
	}
	
	//if there are no segments create 7
	if(m_uOrbits < 1)
		m_uOrbits = 7;

	float fSize = 1.0f; //initial size of orbits

	//creating a color using the spectrum 
	uint uColor = 650; //650 is Red
	//prevent division by 0
	float decrements = 250.0f / (m_uOrbits > 1? static_cast<float>(m_uOrbits - 1) : 1.0f); //decrement until you get to 400 (which is violet)
	/*
		This part will create the orbits, it start at 3 because that is the minimum subdivisions a torus can have
	*/
	uint uSides = 3; //start with the minimal 3 sides

	//initialize points
	points = new vector3*[m_uOrbits];

	for (uint i = uSides; i < m_uOrbits + uSides; i++)
	{
		vector3 v3Color = WaveLengthToRGB(uColor); //calculate color based on wavelength
		
		//the number of points for the given circle
		points[i - uSides] = new vector3[i];

		//calculate the angle between each vertex by dividing 2pi by subdivisions
		float angle = PI * 2 / i;

		//calculate the vertices
		for (int j = 0; j < i; j++)
		{
			points[i - uSides][j] = vector3((fSize - .05) * cosf(angle * j), (fSize - .05) * sinf(angle * j), 0);
		}

		m_shapeList.push_back(m_pMeshMngr->GenerateTorus(fSize, fSize - 0.1f, 3, i, v3Color)); //generate a custom torus and add it to the meshmanager
		fSize += 0.5f; //increment the size for the next orbit
		uColor -= static_cast<uint>(decrements); //decrease the wavelength
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

	matrix4 m4View = m_pCameraMngr->GetViewMatrix(); //view Matrix
	matrix4 m4Projection = m_pCameraMngr->GetProjectionMatrix(); //Projection Matrix
	matrix4 m4Offset = IDENTITY_M4; //offset of the orbits, starts as the global coordinate system
	/*
		The following offset will orient the orbits as in the demo, start without it to make your life easier.
	*/
	//m4Offset = glm::rotate(IDENTITY_M4, 1.5708f, AXIS_Z);
	static float timer = 0;
	static uint clock = m_pSystem->GenClock();
	float deltaTime = m_pSystem->GetDeltaTime(clock);
	timer +=  deltaTime * m_fSpeed; //get the delta time for that timer
	
	static float scaleAmount = 1.0f;
	static float rotationAngle = 0.0f;
	
	if (m_bRot) {
		if (m_bReverseRot) {
			rotationAngle -= deltaTime * m_fRotSpeed;
		}
		else {
			rotationAngle += deltaTime * m_fRotSpeed;
		}
	}


	if (m_bScale) {
		if (m_bGrow) {
			scaleAmount += deltaTime * m_fScaleSpeed;

			if (scaleAmount >= m_fMaxScale) {
				m_bGrow = false;
			}
		}
		else {
			scaleAmount -= deltaTime * m_fScaleSpeed;

			if (scaleAmount <= m_fMinScale) {
				m_bGrow = true;
			}
		}
	}
	// draw a shapes
	for (uint i = 0; i < m_uOrbits; ++i)
	{
		int reverse = 1;
		if (i % 2 == 1) {
			reverse = -1;
		}
		matrix4 torusTransform = glm::scale(m4Offset, vector3(scaleAmount));
		torusTransform = glm::rotate(torusTransform, reverse * rotationAngle, vector3(0.0f, 0.0f, 1.0f));
		m_pMeshMngr->AddMeshToRenderList(m_shapeList[i], glm::rotate(torusTransform, 1.5708f, AXIS_X));
		//calculate the current position
		float lerpTime = timer - floorf(timer);
		int prevPointIndex = (int)floorf(timer) % (i + 3);
		int nextPointIndex = ((int)floorf(timer) + 1) % (i + 3);
		vector3 v3CurrentPos = glm::lerp(points[i][prevPointIndex] , points[i][nextPointIndex], lerpTime);

		matrix4 m4Model = glm::scale(m4Offset, vector3(scaleAmount));
		m4Model = glm::rotate(m4Model, reverse * rotationAngle, vector3(0.0f, 0.0f, 1.0f));
		m4Model = glm::translate(m4Model, v3CurrentPos);



		//draw spheres
		m_pMeshMngr->AddSphereToRenderList(m4Model * glm::scale(vector3(0.1)), C_WHITE);
	}

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
	//release GUI
	ShutdownGUI();
}