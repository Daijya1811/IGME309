#include "MyMesh.h"
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

	vector3 point0(-fValue, -fValue, fValue); //0
	vector3 point1(fValue, -fValue, fValue); //1
	vector3 point2(fValue, fValue, fValue); //2
	vector3 point3(-fValue, fValue, fValue); //3

	vector3 point4(-fValue, -fValue, -fValue); //4
	vector3 point5(fValue, -fValue, -fValue); //5
	vector3 point6(fValue, fValue, -fValue); //6
	vector3 point7(-fValue, fValue, -fValue); //7

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

	//Creates the base of the cone
	std::vector<vector3> vertex;
	GLfloat delta = static_cast<GLfloat>((2.0 * PI) / static_cast<GLfloat>(a_nSubdivisions));
	for (uint i = 0; i < a_nSubdivisions; i++)
	{
		vector3 temp = vector3(cos(delta*i) * a_fRadius, 0.0f, sin(delta*i) * a_fRadius);
		vertex.push_back(temp);
	}

	//Adds the faces of the cone
	for (uint i = 0; i < a_nSubdivisions; i++)
	{
		AddTri(ZERO_V3, vertex[i], vertex[(i + 1) % a_nSubdivisions]);
		AddTri(vector3(0, a_fHeight,0), vertex[(i + 1) % a_nSubdivisions], vertex[i]);
	}

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

	std::vector<vector3> vertexTop; //Top circle of the cylider
	std::vector<vector3> vertexBottom; //Bottom of the cylinder

	//Creates the top and bottom of the cylinder
	GLfloat delta = static_cast<GLfloat>((2.0 * PI) / static_cast<GLfloat>(a_nSubdivisions));
	for (uint i = 0; i < a_nSubdivisions; i++)
	{
		vector3 temp = vector3(cos(delta * i) * a_fRadius, 0.0f, sin(delta * i) * a_fRadius);
		vertexBottom.push_back(temp);
		temp = vector3(cos(delta * i) * a_fRadius, a_fHeight, sin(delta * i) * a_fRadius);
		vertexTop.push_back(temp);
	}

	//Adds the faces to the cylinder
	for (uint i = 0; i < a_nSubdivisions; i++)
	{
		AddQuad(vertexTop[i], vertexTop[(i + 1) % a_nSubdivisions], vertexBottom[i], vertexBottom[(i + 1) % a_nSubdivisions]);
		AddQuad(vertexBottom[i], vertexBottom[(i + 1) % a_nSubdivisions], vertexTop[i], vertexTop[(i + 1) % a_nSubdivisions]);
		AddTri(ZERO_V3, vertexBottom[i], vertexBottom[(i + 1) % a_nSubdivisions]);
		AddTri(vector3(0.0, a_fHeight, 0.0), vertexTop[(i + 1) % a_nSubdivisions], vertexTop[i]);
	}

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

	std::vector<vector3> vertexInsideTop;//Inside circle of the top tube
	std::vector<vector3> vertexOutsideTop;//Outside cirlde of the top tube
	std::vector<vector3> vertexInsideBottom;//Inside circle of the bottom tube
	std::vector<vector3> vertexOutsideBottom;//Outside circle of the bottom tube

	//Creates the circles of the tube
	GLfloat delta = static_cast<GLfloat>((2.0 * PI) / static_cast<GLfloat>(a_nSubdivisions));
	for (uint i = 0; i < a_nSubdivisions; i++)
	{
		vector3 temp = vector3(cos(delta * i) * a_fInnerRadius, 0.0f, sin(delta * i) * a_fInnerRadius);
		vertexInsideBottom.push_back(temp);
		temp = vector3(cos(delta * i) * a_fOuterRadius, 0.0f, sin(delta * i) * a_fOuterRadius);
		vertexOutsideBottom.push_back(temp);

		temp = vector3(cos(delta * i) * a_fInnerRadius, a_fHeight, sin(delta * i) * a_fInnerRadius);
		vertexInsideTop.push_back(temp);
		temp = vector3(cos(delta * i) * a_fOuterRadius, a_fHeight, sin(delta * i) * a_fOuterRadius);
		vertexOutsideTop.push_back(temp);
	}

	//Adds the faces of the tube
	for (uint i = 0; i < a_nSubdivisions; i++)
	{
		AddQuad(vertexOutsideBottom[i], vertexOutsideBottom[(i + 1) % a_nSubdivisions], vertexInsideBottom[i], vertexInsideBottom[(i + 1) % a_nSubdivisions]);
		AddQuad(vertexInsideBottom[i], vertexInsideBottom[(i + 1) % a_nSubdivisions], vertexOutsideBottom[i], vertexOutsideBottom[(i + 1) % a_nSubdivisions]);
		
		AddQuad(vertexOutsideTop[i], vertexOutsideTop[(i + 1) % a_nSubdivisions], vertexInsideTop[i], vertexInsideTop[(i + 1) % a_nSubdivisions]);
		AddQuad(vertexInsideTop[i], vertexInsideTop[(i + 1) % a_nSubdivisions], vertexOutsideTop[i], vertexOutsideTop[(i + 1) % a_nSubdivisions]);

		AddQuad(vertexOutsideBottom[i], vertexOutsideBottom[(i + 1) % a_nSubdivisions], vertexOutsideTop[i], vertexOutsideTop[(i + 1) % a_nSubdivisions]);
		AddQuad(vertexOutsideTop[i], vertexOutsideTop[(i + 1) % a_nSubdivisions], vertexOutsideBottom[i], vertexOutsideBottom[(i + 1) % a_nSubdivisions]);

		AddQuad(vertexInsideBottom[i], vertexInsideBottom[(i + 1) % a_nSubdivisions], vertexInsideTop[i], vertexInsideTop[(i + 1) % a_nSubdivisions]);
		AddQuad(vertexInsideTop[i], vertexInsideTop[(i + 1) % a_nSubdivisions], vertexInsideBottom[i], vertexInsideBottom[(i + 1) % a_nSubdivisions]);
	}

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

	std::vector<vector3> vertex; //Create a circle
	std::vector<std::vector<vector3>> listOfCircles; //Holds the list of circles

	// Creating the circle
	GLfloat delta = static_cast<GLfloat>((2.0 * PI) / static_cast<GLfloat>(a_nSubdivisionsA));
	for (uint i = 0; i < a_nSubdivisionsB; i++)
	{
		for (uint j = 0; j < a_nSubdivisionsA; j++)
		{
			vector3 temp = vector3(cos(delta * j) * ((a_fOuterRadius - a_fInnerRadius) / 2), sin(delta * j) * ((a_fOuterRadius - a_fInnerRadius) / 2), 0.0f);
			vertex.push_back(temp);
		}

		//Temp list 
		std::vector<vector3> tempList = vertex;

		//Translate the circles
		matrix4 offSet = glm::rotate(matrix4(), delta * i, vector3(0.0f, 1.0f, 0.0f));
		offSet = offSet * glm::translate(matrix4(), vector3(a_fOuterRadius, 0.0f, 0.0f));

		for (uint i = 0; i < a_nSubdivisionsA; i++)
		{
			tempList[i] = offSet * vector4(tempList[i], 1.0f);
		}

		/*
		//Testing
		for (uint i = 0; i < a_nSubdivisionsA; i++)
		{
			AddTri(ZERO_V3, tempList[i], tempList[(i + 1) % a_nSubdivisionsA]);
		}
		*/
		
		//Adds circles to the list of circles
		listOfCircles.push_back(tempList);
	}

	//Prints the circles out and connects them
	for (uint i = 0; i < a_nSubdivisionsB; i++)
	{
		//Checks to see if it is the last circle in the list
		if (i == a_nSubdivisionsB-1)
		{
			for (uint j = 0; j < a_nSubdivisionsA; j++)
			{
				AddQuad(listOfCircles[i][j], listOfCircles[0][j], listOfCircles[i][(j + 1) % a_nSubdivisionsA], listOfCircles[0][(j + 1) % a_nSubdivisionsA]);
			}
		}
		else
		{
			for (uint j = 0; j < a_nSubdivisionsA; j++)
			{
				AddQuad(listOfCircles[i][j], listOfCircles[i + 1][j], listOfCircles[i][(j + 1) % a_nSubdivisionsA], listOfCircles[i + 1][(j + 1) % a_nSubdivisionsA]);
			}
		}
	}

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
	if (a_nSubdivisions > 20)
		a_nSubdivisions = 20;

	Release();
	Init();

	std::vector<vector3> vertex;//Creates circle
	std::vector<std::vector<vector3>> listOfCircles;//List of circles creates
	int circles = a_nSubdivisions * 4;//Number of circles created 

	//Creates the cirles
	GLfloat delta = static_cast<GLfloat>((2.0 * PI) / static_cast<GLfloat>(a_nSubdivisions));
	for (uint i = 0; i < a_nSubdivisions; i++)
	{
		vector3 circle = vector3((glm::cos(delta * i) * a_fRadius), 0.0f, (glm::sin(delta * i) * a_fRadius));
		vertex.push_back(circle);
	}

	//Creates the circles for the sphere
	for (uint i = 0; i < a_nSubdivisions; i++)
	{
		AddTri(ZERO_V3, vertex[i], vertex[(i + 1) % a_nSubdivisions]);
	}

	//Creates the inner circles of the sphere 
	for (uint i = 0; i < circles - 1; i++)
	{
		std::vector<vector3> tempList = vertex;

		//Rotates the circles 
		matrix4 offSet = glm::rotate(matrix4(), (delta * i), vector3(2.0f, 0.0f, 0.0f));

		//Applies the offSet matrix4 to all of them
		for (uint j = 0; j < a_nSubdivisions; j++)
		{
			tempList[j] = offSet * vector4(tempList[j], 1.0f);
		}
		listOfCircles.push_back(tempList);
	}

	//Prints the circles out and connects them
	for (uint i = 0; i < listOfCircles.size(); i++)
	{
		//Checks to see if it is the last circle in the list
		if (i == listOfCircles.size() - 1)
		{
			for (uint j = 0; j < a_nSubdivisions; j++)
			{
				AddQuad(listOfCircles[i][j], listOfCircles[0][j], listOfCircles[i][(j + 1) % a_nSubdivisions], listOfCircles[0][(j + 1) % a_nSubdivisions]);
			}
		}
		else
		{
			for (uint j = 0; j < a_nSubdivisions; j++)
			{
				AddQuad(listOfCircles[i][j], listOfCircles[i + 1][j], listOfCircles[i][(j + 1) % a_nSubdivisions], listOfCircles[i + 1][(j + 1) % a_nSubdivisions]);
			}
		}
	}

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
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