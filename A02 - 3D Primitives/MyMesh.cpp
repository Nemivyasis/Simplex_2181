#include "MyMesh.h"
void MyMesh::Init(void)
{
	m_bBinded = false;
	m_uVertexCount = 0;

	m_VAO = 0;
	m_VBO = 0;

	m_pShaderMngr = ShaderManager::GetInstance();
}
void MyMesh::Release(void)
{
	m_pShaderMngr = nullptr;

	if (m_VBO > 0)
		glDeleteBuffers(1, &m_VBO);

	if (m_VAO > 0)
		glDeleteVertexArrays(1, &m_VAO);

	m_lVertex.clear();
	m_lVertexPos.clear();
	m_lVertexCol.clear();
}
MyMesh::MyMesh()
{
	Init();
}
MyMesh::~MyMesh() { Release(); }
MyMesh::MyMesh(MyMesh& other)
{
	m_bBinded = other.m_bBinded;

	m_pShaderMngr = other.m_pShaderMngr;

	m_uVertexCount = other.m_uVertexCount;

	m_VAO = other.m_VAO;
	m_VBO = other.m_VBO;
}
MyMesh& MyMesh::operator=(MyMesh& other)
{
	if (this != &other)
	{
		Release();
		Init();
		MyMesh temp(other);
		Swap(temp);
	}
	return *this;
}
void MyMesh::Swap(MyMesh& other)
{
	std::swap(m_bBinded, other.m_bBinded);
	std::swap(m_uVertexCount, other.m_uVertexCount);

	std::swap(m_VAO, other.m_VAO);
	std::swap(m_VBO, other.m_VBO);

	std::swap(m_lVertex, other.m_lVertex);
	std::swap(m_lVertexPos, other.m_lVertexPos);
	std::swap(m_lVertexCol, other.m_lVertexCol);

	std::swap(m_pShaderMngr, other.m_pShaderMngr);
}
void MyMesh::CompleteMesh(vector3 a_v3Color)
{
	uint uColorCount = m_lVertexCol.size();
	for (uint i = uColorCount; i < m_uVertexCount; ++i)
	{
		m_lVertexCol.push_back(a_v3Color);
	}
}
void MyMesh::AddVertexPosition(vector3 a_v3Input)
{
	m_lVertexPos.push_back(a_v3Input);
	m_uVertexCount = m_lVertexPos.size();
}
void MyMesh::AddVertexColor(vector3 a_v3Input)
{
	m_lVertexCol.push_back(a_v3Input);
}
void MyMesh::CompileOpenGL3X(void)
{
	if (m_bBinded)
		return;

	if (m_uVertexCount == 0)
		return;

	CompleteMesh();

	for (uint i = 0; i < m_uVertexCount; i++)
	{
		//Position
		m_lVertex.push_back(m_lVertexPos[i]);
		//Color
		m_lVertex.push_back(m_lVertexCol[i]);
	}
	glGenVertexArrays(1, &m_VAO);//Generate vertex array object
	glGenBuffers(1, &m_VBO);//Generate Vertex Buffered Object

	glBindVertexArray(m_VAO);//Bind the VAO
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);//Bind the VBO
	glBufferData(GL_ARRAY_BUFFER, m_uVertexCount * 2 * sizeof(vector3), &m_lVertex[0], GL_STATIC_DRAW);//Generate space for the VBO

	// Position attribute
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(vector3), (GLvoid*)0);

	// Color attribute
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(vector3), (GLvoid*)(1 * sizeof(vector3)));

	m_bBinded = true;

	glBindVertexArray(0); // Unbind VAO
}
void MyMesh::Render(matrix4 a_mProjection, matrix4 a_mView, matrix4 a_mModel)
{
	// Use the buffer and shader
	GLuint nShader = m_pShaderMngr->GetShaderID("Basic");
	glUseProgram(nShader); 

	//Bind the VAO of this object
	glBindVertexArray(m_VAO);

	// Get the GPU variables by their name and hook them to CPU variables
	GLuint MVP = glGetUniformLocation(nShader, "MVP");
	GLuint wire = glGetUniformLocation(nShader, "wire");

	//Final Projection of the Camera
	matrix4 m4MVP = a_mProjection * a_mView * a_mModel;
	glUniformMatrix4fv(MVP, 1, GL_FALSE, glm::value_ptr(m4MVP));
	
	//Solid
	glUniform3f(wire, -1.0f, -1.0f, -1.0f);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glDrawArrays(GL_TRIANGLES, 0, m_uVertexCount);  

	//Wire
	glUniform3f(wire, 1.0f, 0.0f, 1.0f);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glEnable(GL_POLYGON_OFFSET_LINE);
	glPolygonOffset(-1.f, -1.f);
	glDrawArrays(GL_TRIANGLES, 0, m_uVertexCount);
	glDisable(GL_POLYGON_OFFSET_LINE);

	glBindVertexArray(0);// Unbind VAO so it does not get in the way of other objects
}
void MyMesh::AddTri(vector3 a_vBottomLeft, vector3 a_vBottomRight, vector3 a_vTopLeft)
{
	//C
	//| \
	//A--B
	//This will make the triangle A->B->C 
	AddVertexPosition(a_vBottomLeft);
	AddVertexPosition(a_vBottomRight);
	AddVertexPosition(a_vTopLeft);
}
void MyMesh::AddQuad(vector3 a_vBottomLeft, vector3 a_vBottomRight, vector3 a_vTopLeft, vector3 a_vTopRight)
{
	//C--D
	//|  |
	//A--B
	//This will make the triangle A->B->C and then the triangle C->B->D
	AddVertexPosition(a_vBottomLeft);
	AddVertexPosition(a_vBottomRight);
	AddVertexPosition(a_vTopLeft);

	AddVertexPosition(a_vTopLeft);
	AddVertexPosition(a_vBottomRight);
	AddVertexPosition(a_vTopRight);
}
void MyMesh::GenerateCube(float a_fSize, vector3 a_v3Color)
{
	if (a_fSize < 0.01f)
		a_fSize = 0.01f;

	Release();
	Init();

	float fValue = a_fSize * 0.5f;
	//3--2
	//|  |
	//0--1

	vector3 point0(-fValue,-fValue, fValue); //0
	vector3 point1( fValue,-fValue, fValue); //1
	vector3 point2( fValue, fValue, fValue); //2
	vector3 point3(-fValue, fValue, fValue); //3

	vector3 point4(-fValue,-fValue,-fValue); //4
	vector3 point5( fValue,-fValue,-fValue); //5
	vector3 point6( fValue, fValue,-fValue); //6
	vector3 point7(-fValue, fValue,-fValue); //7

	//F
	AddQuad(point0, point1, point3, point2);

	//B
	AddQuad(point5, point4, point6, point7);

	//L
	AddQuad(point4, point0, point7, point3);

	//R
	AddQuad(point1, point5, point2, point6);

	//U
	AddQuad(point3, point2, point7, point6);

	//D
	AddQuad(point4, point5, point0, point1);

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
void MyMesh::GenerateCuboid(vector3 a_v3Dimensions, vector3 a_v3Color)
{
	Release();
	Init();

	vector3 v3Value = a_v3Dimensions * 0.5f;
	//3--2
	//|  |
	//0--1
	vector3 point0(-v3Value.x, -v3Value.y, v3Value.z); //0
	vector3 point1(v3Value.x, -v3Value.y, v3Value.z); //1
	vector3 point2(v3Value.x, v3Value.y, v3Value.z); //2
	vector3 point3(-v3Value.x, v3Value.y, v3Value.z); //3

	vector3 point4(-v3Value.x, -v3Value.y, -v3Value.z); //4
	vector3 point5(v3Value.x, -v3Value.y, -v3Value.z); //5
	vector3 point6(v3Value.x, v3Value.y, -v3Value.z); //6
	vector3 point7(-v3Value.x, v3Value.y, -v3Value.z); //7

	//F
	AddQuad(point0, point1, point3, point2);

	//B
	AddQuad(point5, point4, point6, point7);

	//L
	AddQuad(point4, point0, point7, point3);

	//R
	AddQuad(point1, point5, point2, point6);

	//U
	AddQuad(point3, point2, point7, point6);

	//D
	AddQuad(point4, point5, point0, point1);

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
void MyMesh::GenerateCone(float a_fRadius, float a_fHeight, int a_nSubdivisions, vector3 a_v3Color)
{
	if (a_fRadius < 0.01f)
		a_fRadius = 0.01f;

	if (a_fHeight < 0.01f)
		a_fHeight = 0.01f;

	if (a_nSubdivisions < 3)
		a_nSubdivisions = 3;
	if (a_nSubdivisions > 360)
		a_nSubdivisions = 360;

	Release();
	Init();

	// Replace this with your code
	vector3 center = vector3(0, -a_fHeight / 2, 0);
	//make the circle bottom
	vector3* vertices = new vector3[a_nSubdivisions];

	//angle between each point in the circle
	float angle = (float) PI * 2 / a_nSubdivisions;

	//finds subdivision number of points on bottom circle
	for (int i = 0; i < a_nSubdivisions; i++)
	{
		vertices[i] = vector3(a_fRadius * cosf(angle * i), -a_fHeight / 2, -a_fRadius * sinf(angle * i));
	}

	//add the tris for the bottom circle and pyramid
	vector3 top = vector3(0, a_fHeight / 2, 0);
	for (int i = 0; i < a_nSubdivisions - 1; i++)
	{
		AddTri(center, vertices[i + 1], vertices[i]);
		AddTri(top, vertices[i], vertices[i + 1]);
	}

	//final 2 tris (from the end of the array to front)
	AddTri(center, vertices[0], vertices[a_nSubdivisions - 1]);
	AddTri(top, vertices[a_nSubdivisions - 1], vertices[0]);

	//clean memory
	if (vertices != nullptr) 
	{
		delete[] vertices;
		vertices = nullptr;
	}

	// -------------------------------

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
void MyMesh::GenerateCylinder(float a_fRadius, float a_fHeight, int a_nSubdivisions, vector3 a_v3Color)
{
	if (a_fRadius < 0.01f)
		a_fRadius = 0.01f;

	if (a_fHeight < 0.01f)
		a_fHeight = 0.01f;

	if (a_nSubdivisions < 3)
		a_nSubdivisions = 3;
	if (a_nSubdivisions > 360)
		a_nSubdivisions = 360;

	Release();
	Init();

	// Replace this with your code
	//center of top center and bottom center
	vector3 botCenter = vector3(0, -a_fHeight / 2, 0);
	vector3 topCenter = vector3(0, a_fHeight / 2, 0);

	//arrays for points of circles
	vector3* botVertices = new vector3[a_nSubdivisions];
	vector3* topVertices = new vector3[a_nSubdivisions];

	//angle between each point in the circle
	float angle = (float) PI * 2 / a_nSubdivisions;

	//simultaneously gets points for top and bottom circle
	for (int i = 0; i < a_nSubdivisions; i++)
	{
		//bottom circle
		botVertices[i] = vector3(a_fRadius * cosf(angle * i), -a_fHeight / 2, -a_fRadius * sinf(angle * i));

		//top circle
		topVertices[i] = vector3(a_fRadius * cosf(angle * i), a_fHeight / 2, -a_fRadius * sinf(angle * i));
	}

	//draw tris for top and bottom circles as well as the sides
	for (int i = 0; i < a_nSubdivisions - 1; i++)
	{
		//bottom circle
		AddTri(botCenter, botVertices[i + 1], botVertices[i]);

		//top circle
		AddTri(topCenter, topVertices[i], topVertices[i + 1]);

		//sides
		AddQuad(botVertices[i], botVertices[i + 1], topVertices[i], topVertices[i + 1]);
	}

	//final tris and quads (from end of array to beginning)
	AddQuad(botVertices[a_nSubdivisions - 1], botVertices[0], topVertices[a_nSubdivisions - 1], topVertices[0]);
	AddTri(botCenter, botVertices[0], botVertices[a_nSubdivisions - 1]);
	AddTri(topCenter, topVertices[a_nSubdivisions - 1], topVertices[0]);

	//clean memory
	if (botVertices != nullptr) 
	{
		delete[] botVertices;
		botVertices = nullptr;
	}
	if (topVertices != nullptr) 
	{
		delete[] topVertices;
		topVertices = nullptr;
	}
	// -------------------------------

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
void MyMesh::GenerateTube(float a_fOuterRadius, float a_fInnerRadius, float a_fHeight, int a_nSubdivisions, vector3 a_v3Color)
{
	if (a_fOuterRadius < 0.01f)
		a_fOuterRadius = 0.01f;

	if (a_fInnerRadius < 0.005f)
		a_fInnerRadius = 0.005f;

	if (a_fInnerRadius > a_fOuterRadius)
		std::swap(a_fInnerRadius, a_fOuterRadius);

	if (a_fHeight < 0.01f)
		a_fHeight = 0.01f;

	if (a_nSubdivisions < 3)
		a_nSubdivisions = 3;
	if (a_nSubdivisions > 360)
		a_nSubdivisions = 360;

	Release();
	Init();

	// Replace this with your code
	//array of circle vertices
	vector3* botVertices = new vector3[a_nSubdivisions];
	vector3* topVertices = new vector3[a_nSubdivisions];
	vector3* botInnerVertices = new vector3[a_nSubdivisions];
	vector3* topInnerVertices = new vector3[a_nSubdivisions];

	//angle between each point in the circle
	float angle = (float) PI * 2 / a_nSubdivisions;

	//simultaneously calculate each circle's points
	for (int i = 0; i < a_nSubdivisions; i++)
	{
		botVertices[i] = vector3(a_fOuterRadius * cosf(angle * i), -a_fHeight / 2, -a_fOuterRadius * sinf(angle * i));
		topVertices[i] = vector3(a_fOuterRadius * cosf(angle * i), a_fHeight / 2, -a_fOuterRadius * sinf(angle * i));
		botInnerVertices[i] = vector3(a_fInnerRadius * cosf(angle * i), -a_fHeight / 2, -a_fInnerRadius * sinf(angle * i));
		topInnerVertices[i] = vector3(a_fInnerRadius * cosf(angle * i), a_fHeight / 2, -a_fInnerRadius * sinf(angle * i));
	}

	//add 4 quads for each subdivision
	for (int i = 0; i < a_nSubdivisions - 1; i++)
	{
		//top donut
		AddQuad(topVertices[i], topVertices[i + 1], topInnerVertices[i], topInnerVertices[i + 1]);

		//bottom donut
		AddQuad(botVertices[i + 1], botVertices[i], botInnerVertices[i + 1], botInnerVertices[i]);

		//outside side
		AddQuad(botVertices[i], botVertices[i + 1], topVertices[i], topVertices[i + 1]);

		//inside side
		AddQuad(topInnerVertices[i], topInnerVertices[i + 1], botInnerVertices[i], botInnerVertices[i + 1]);
	}

	//final quads (from end of array to beginning)
	AddQuad(topVertices[a_nSubdivisions - 1], topVertices[0], topInnerVertices[a_nSubdivisions - 1], topInnerVertices[0]);
	AddQuad(botVertices[0], botVertices[a_nSubdivisions - 1], botInnerVertices[0], botInnerVertices[a_nSubdivisions - 1]);
	AddQuad(botVertices[a_nSubdivisions - 1], botVertices[0], topVertices[a_nSubdivisions - 1], topVertices[0]);
	AddQuad(topInnerVertices[a_nSubdivisions - 1], topInnerVertices[0], botInnerVertices[a_nSubdivisions - 1], botInnerVertices[0]);

	//clean memory
	if (botVertices != nullptr) 
	{
		delete[] botVertices;
		botVertices = nullptr;
	}
	if (topVertices != nullptr) 
	{
		delete[] topVertices;
		topVertices = nullptr;
	}
	if (botInnerVertices != nullptr) 
	{
		delete[] botInnerVertices;
		botInnerVertices = nullptr;
	}
	if (topInnerVertices != nullptr) 
	{
		delete[] topInnerVertices;
		topInnerVertices = nullptr;
	}
	// -------------------------------

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
void MyMesh::GenerateTorus(float a_fOuterRadius, float a_fInnerRadius, int a_nSubdivisionsA, int a_nSubdivisionsB, vector3 a_v3Color)
{
	if (a_fOuterRadius < 0.01f)
		a_fOuterRadius = 0.01f;

	if (a_fInnerRadius < 0.005f)
		a_fInnerRadius = 0.005f;

	if (a_fInnerRadius > a_fOuterRadius)
		std::swap(a_fInnerRadius, a_fOuterRadius);

	if (a_nSubdivisionsA < 3)
		a_nSubdivisionsA = 3;
	if (a_nSubdivisionsA > 360)
		a_nSubdivisionsA = 360;

	if (a_nSubdivisionsB < 3)
		a_nSubdivisionsB = 3;
	if (a_nSubdivisionsB > 360)
		a_nSubdivisionsB = 360;

	Release();
	Init();

	// Replace this with your code
	vector3** vertices = new vector3*[a_nSubdivisionsA];
	for (int i = 0; i < a_nSubdivisionsA; i++)
	{
		vertices[i] = new vector3[a_nSubdivisionsB];
	}


	//make the small circle
	//radius of circles along tube
	float tubeRadius = (a_fOuterRadius - a_fInnerRadius) / 2;

	//angle between points on small circle
	float angle = (float)PI * 2 / a_nSubdivisionsA;

	//get points on small circle and translate them out so the circle is along the tube
	for (int i = 0; i < a_nSubdivisionsA; i++)
	{
		vertices[i][0] = vector3(tubeRadius * cosf(angle * i) - tubeRadius + a_fOuterRadius, tubeRadius * sinf(angle * i), 0);
	}

	//angle between circles on tube
	angle = (float)PI * 2 / a_nSubdivisionsB;

	//for each point along the first circle I made, calculate a large circle flat along the xz plane to form the big tube
	for (int i = 0; i < a_nSubdivisionsA; i++)
	{
		//radius of the big circle
		float circRadius = sqrt(vertices[i][0].x * vertices[i][0].x + vertices[i][0].z * vertices[i][0].z);

		//find each point
		for (int j = 1; j < a_nSubdivisionsB; j++)
		{
			vertices[i][j] = vector3(circRadius * cosf(angle * j), vertices[i][0].y, circRadius * sinf(angle * j));
		}
	}

	//make the quads
	for (int i = 0; i < a_nSubdivisionsA; i++)
	{
		for (int j = 0; j < a_nSubdivisionsB; j++)
		{
			//quads attaching a point to the one next to it along the small circle, next along the big circle, and diagonal to it
			//the modulus makes it so that i + 1 wraps to 0 when i = subdivisions - 1
			AddQuad(vertices[i][j], vertices[(i + 1) % a_nSubdivisionsA][j], vertices[i][(j + 1) % a_nSubdivisionsB],  vertices[(i + 1) % a_nSubdivisionsA][(j + 1) % a_nSubdivisionsB]);
		}
	}

	//clean memory
	for (int i = 0; i < a_nSubdivisionsA; i++)
	{
		if (vertices[i] != nullptr) {
			delete[] vertices[i];
			vertices[i] = nullptr;
		}
	}

	if (vertices != nullptr) {
		delete[] vertices;
		vertices = nullptr;
	}
	// -------------------------------

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
void MyMesh::GenerateSphere(float a_fRadius, int a_nSubdivisions, vector3 a_v3Color)
{
	if (a_fRadius < 0.01f)
		a_fRadius = 0.01f;

	//Sets minimum and maximum of subdivisions
	if (a_nSubdivisions < 1)
	{
		GenerateCube(a_fRadius * 2.0f, a_v3Color);
		return;
	}
	if (a_nSubdivisions > 36)
		a_nSubdivisions = 36;

	Release();
	Init();

	// Replace this with your code
	//top and bottom of sphere
	vector3 top(0, a_fRadius, 0);
	vector3 bottom(0, -a_fRadius, 0);

	//holds the vertices of full sphere
	//the -2 is because I am counting top and bottom as a subdivision, so there are the same number of face divisions vertical as horizontal
	vector3** vertices = new vector3*[a_nSubdivisions - 2];
	for (int i = 0; i < a_nSubdivisions - 2; i++)
	{
		vertices[i] = new vector3[a_nSubdivisions];
	}

	//I'm rotating this point up and down to find where to start each xz-flat circle
	vector3 midstartpoint(a_fRadius, 0, 0);

	//angle each point is from the others
	float angle = (float) PI * 2 / (a_nSubdivisions);
	//make circle
	for (int i = 0; i < a_nSubdivisions - 2; i++)
	{

		//makes it so that the sphere is centered and that we are finding one of the sides along the y-axis
		float centeringNumber = i + 1 - (a_nSubdivisions - 1) / 2.0f;

		//angle / 2 makes it so that we only get points for half the circle
		//we make a rotation matrix that finds a point given the far right side of the sphere, then we rotate that far right point to find the point we want
		matrix3 transformation = matrix3(cosf(angle / 2 * centeringNumber), -sinf(angle / 2 * centeringNumber), 0, sinf(angle / 2 * centeringNumber), cosf(angle / 2 * centeringNumber), 0, 0, 0, 1);
		vertices[i][0] = midstartpoint * transformation;
	}

	//take each point along the right side of the sphere and create a circle on the xz plane out of it
	for (int i = 0; i < a_nSubdivisions - 2; i++)
	{
		for (int j = 0; j < a_nSubdivisions; j++)
		{
			//make a rotation matrix and rotate the point to find the new points
			matrix3 transformation = matrix3(cosf(angle * j), 0, sinf(angle * j), 0, 1, 0, -sinf(angle * j), 0, cosf(angle * j));
			vertices[i][j] = vertices[i][0] * transformation;
		}
	}

	//add the quads
	for (int i = 0; i < a_nSubdivisions - 3; i++)
	{
		for (int j = 0; j < a_nSubdivisions - 1; j++)
		{
			//add a quad along each xz circle
			AddQuad(vertices[i][j], vertices[i][j + 1], vertices[i + 1][j], vertices[i + 1][j + 1]);
		}
		//add the final quad from the end of the array to beginning
		AddQuad(vertices[i][a_nSubdivisions - 1], vertices[i][0], vertices[i + 1][a_nSubdivisions - 1], vertices[i + 1][0]);
	}

	//make the top and bottom using the top circle and bottom circle and drawing tris to top and bottom point
	for (int i = 0; i < a_nSubdivisions - 1; i++)
	{
		AddTri(vertices[0][i], bottom, vertices[0][i + 1]);
		AddTri(vertices[a_nSubdivisions - 3][i], vertices[a_nSubdivisions - 3][i + 1], top);
	}

	//draw the final two tris
	AddTri(vertices[0][a_nSubdivisions - 1], bottom, vertices[0][0]);
	AddTri(vertices[a_nSubdivisions - 3][a_nSubdivisions - 1], vertices[a_nSubdivisions - 3][0], top);

	//clean memory
	for (int i = 0; i < a_nSubdivisions - 2; i++)
	{
		if (vertices[i] != nullptr) {
			delete[] vertices[i];
			vertices[i] = nullptr;
		}
	}
	
	if (vertices != nullptr) {
		delete[] vertices;
		vertices = nullptr;
	}
	// -------------------------------

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}