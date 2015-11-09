#include <APP_SpotRotate.h>
#include <fstream>
#include <string>
#include <iostream>

struct Vertex {
	vec4 position;
	vec4 colour;
};

APP_SpotRotate::APP_SpotRotate()
{

}
APP_SpotRotate::~APP_SpotRotate()
{

}

// function to create a flat plane grid
void APP_SpotRotate::generateGrid(unsigned int rows, unsigned int cols)
{
	rows++; //add one to show correct rows/cols
	cols++;	// ""
	//define vertex positions
	Vertex* aoVertices = new Vertex[rows * cols];
	for (unsigned int r = 0; r < rows; ++r)
	{
		for (unsigned int c = 0; c < cols; ++c)
		{
			aoVertices[r * cols + c].position = vec4((float)c, 0, (float)r, 1); // assign vertex position	
			//assign a rgb colour based on sin and cos (90 deg offset)
			float sinTime = sinf((m_time  * c / (float)(cols - 1)) * (r / (float)(rows - 1)));
			float cosTime = cosf((m_time  * c / (float)(cols - 1)) * (r / (float)(rows - 1)));
			vec3 colour = vec3(sinTime * -1, cosTime, sinTime);
			aoVertices[r * cols + c].colour = vec4(colour, 1);
		}
	}

	// defining index count based off quad count (2 triangles per quad)
	unsigned int indexCount = (rows - 1) * (cols - 1) * 6; //index count is the number of vertices to make triangle NOT the vertices with positions
	unsigned int* auiIndices = new unsigned int[indexCount];
	unsigned int index = 0;
	for (unsigned int r = 0; r < (rows - 1); ++r)
	{
		for (unsigned int c = 0; c < (cols - 1); ++c)
		{
			// triangle 1
			auiIndices[index++] = r * cols + c;
			auiIndices[index++] = (r + 1) * cols + c;
			auiIndices[index++] = (r + 1) * cols + (c + 1);
			// triangle 2
			auiIndices[index++] = r * cols + c;
			auiIndices[index++] = (r + 1) * cols + (c + 1);
			auiIndices[index++] = r * cols + (c + 1);
		}
	}

	glUseProgram(m_programID);						//select the program to use
	//bind buffers
	glBindVertexArray(m_VAO);						//bind vertex array object
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);			//vertex buffer objet
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBO);	//index buffer object
	//enable arrays for incoming data
	glEnableVertexAttribArray(0);					//enable vertex positions in the vertex shader
	glEnableVertexAttribArray(1);					//enable colour in the vertex shader

	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);	//index of poisition within the vertex object
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(vec4))); //index position of the colour data - 1 vec 4 into the vertex block

	glBufferData(GL_ARRAY_BUFFER, (rows * cols) * sizeof(Vertex), aoVertices, GL_STATIC_DRAW); //push the vertices into vertex buffer object
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, (rows - 1) * (cols - 1) * 6 * sizeof(unsigned int), auiIndices, GL_STATIC_DRAW); //push the vertice indexs into the index buffer object

	unsigned int projectionViewUniform = glGetUniformLocation(m_programID, "ProjectionView");			//get the index of the ProjectionView uniform variable from the vertex shader
	glUniformMatrix4fv(projectionViewUniform, 1, false, glm::value_ptr(GameCam->GetProjectionView()));	//set the Projection View uniform variabe in the vertex shader

	unsigned int timeUniform = glGetUniformLocation(m_programID, "Time");	//get the Time uniform index from the vertex shader
	glUniform1f(timeUniform, m_time);	//set the Time uniform variabe in the vertex shader

	unsigned int heightUniform = glGetUniformLocation(m_programID, "HeightScale"); //get the Time uniform index from the vertex shader
	float height = 1.0f; //TODO - do something more fun with the hieght value
	glUniform1f(heightUniform, height); //set the Time uniform variabe in the vertex shader

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); //enable wireframe render
	glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0); //draw all triangles to screen

	//unbind and delte pointers
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	delete[] auiIndices;
	delete[] aoVertices;
}

void APP_SpotRotate::generatePlane()
{
	//Vertex* aoVertices = new Vertex();
	
	glm::vec4 tl = glm::vec4(-5, 5, 0, 1);
	glm::vec4 tr = glm::vec4(5, 5, 0, 1);
	glm::vec4 bl = glm::vec4(5, -5, 0, 1);
	glm::vec4 br = glm::vec4(-5, -5, 0, 1);

	//build rotation matrix
	float rot = m_time;
	glm::mat4 rotation = glm::rotate( rot, glm::vec3(0, 0, 1));	

	// create billboard transform
	vec3 zAxis = glm::normalize(vec3(GameCam->GetWorldTransform()[3]) - vec3(0, 0, 0)); //vec3 is the quad pos, left in for reference
	vec3 xAxis = glm::cross(vec3(GameCam->GetWorldTransform()[1]), zAxis);
	vec3 yAxis = glm::cross(zAxis, xAxis);
	glm::mat4 billboard(vec4(xAxis, 0),
		vec4(yAxis, 0),
		vec4(zAxis, 0),
		vec4(0, 0, 0, 1));

	rotation = billboard * rotation;

	glm::mat4 position; //resulting vert position
	glm::mat4 trans;

	//top left vert
	trans = glm::translate(vec3(tl.xyz));
	position = rotation * trans;		
	tl = glm::vec4(position[3]);
	
	//top right vert
	trans = glm::translate(vec3(tr.xyz));
	position = rotation * trans;
	tr = glm::vec4(position[3]);

	//bottom left vert
	trans = glm::translate(vec3(bl.xyz));
	position = rotation * trans;
	bl = glm::vec4(position[3]);

	//bottom right vert
	trans = glm::translate(vec3(br.xyz));
	position = rotation * trans;
	br = glm::vec4(position[3]);




	//generate buffers and mesh information
	float vertexData[] = {
		tl.x, tl.y, tl.z, 1,	0, 1,
		tr.x, tr.y, tr.z, 1,	1, 1,
		bl.x, bl.y, bl.z, 1,	1, 0,
		br.x, br.y, br.z, 1,	0, 0,
	};
	unsigned int indexData[] = {
		0, 1, 2,
		0, 2, 3,
	};
	
	glUseProgram(m_programID);						//select the program to use
	//bind buffers
	glBindVertexArray(m_VAO);						//bind vertex array object
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);			//vertex buffer objet
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBO);	//index buffer object
	//enable arrays for incoming data
	glEnableVertexAttribArray(0);					//enable vertex positions in the vertex shader
	glEnableVertexAttribArray(1);					//enable texture coord in the vertex shader
	
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(float)* 6, 0);	
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float)* 6, ((char*)0) + 16);

	glBufferData(GL_ARRAY_BUFFER, sizeof(float)* 6 * 4, vertexData, GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int)* 6, indexData, GL_STATIC_DRAW);

	unsigned int projectionViewUniform = glGetUniformLocation(m_programID, "ProjectionView");			//get the index of the ProjectionView uniform variable from the vertex shader
	glUniformMatrix4fv(projectionViewUniform, 1, false, glm::value_ptr(GameCam->GetProjectionView()));	//set the Projection View uniform variabe in the vertex shader

//	unsigned int timeUniform = glGetUniformLocation(m_programID, "Time");	//get the Time uniform index from the vertex shader
//	glUniform1f(timeUniform, m_time);	//set the Time uniform variabe in the vertex shader
//
//	unsigned int heightUniform = glGetUniformLocation(m_programID, "HeightScale"); //get the Time uniform index from the vertex shader
//	float height = 1.0f; //TODO - do something more fun with the hieght value
//	glUniform1f(heightUniform, height); //set the Time uniform variabe in the vertex shader

//	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); //enable wireframe render
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

	//unbind and delte pointers
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);	
}

void APP_SpotRotate::Update(float a_dt)
{
	GameCam->Update(a_dt); //update camera

	m_time += a_dt;
}

void APP_SpotRotate::Draw()
{
	//*********************gizmos and home grid
	Gizmos::clear();
	Gizmos::addTransform(glm::mat4(1));

	vec4 white(1);
	vec4 black(0, 0, 0, 1);


	//	draws the home grid
	for (int i = 0; i < 21; ++i) {
		Gizmos::addLine(vec3(-10 + i, 0, 10),
			vec3(-10 + i, 0, -10),
			i == 10 ? white : black);
		Gizmos::addLine(vec3(10, 0, -10 + i),
			vec3(-10, 0, -10 + i),
			i == 10 ? white : black);
	}
	//*********************end gizmos and home grid

	Gizmos::draw(GameCam->GetProjectionView());

	//generateGrid(6, 6); //draw the grid

	generatePlane();
}

std::string APP_SpotRotate::LoadShader(const char *a_filePath)
{
	std::string strShaderCode; //file info holder
	//open shader file
	std::ifstream shaderStream(a_filePath);
	//if that worked ok, load file line by line

	if (shaderStream.is_open())
	{
		std::string Line = "";
		while (std::getline(shaderStream, Line))
		{
			strShaderCode += "\n" + Line;
		}
		shaderStream.close();
	}

	//a_shaderResult = strShaderCode.c_str();
	//const char* result = strShaderCode.c_str();

	//(_result) = (*result);

	return strShaderCode; //not in use
}

void APP_SpotRotate::ClearMenu()
{
	TwDeleteBar(m_bar); //reset the gui
}

void APP_SpotRotate::CreateGui()
{
	m_bar = TwNewBar("SpotRotate");

	TwDefine(" SpotRotate position='10 10' "); // move bar to position (10, 10)
	TwDefine(" SpotRotate size='430 320' "); // resize bar	
	TwDefine(" SpotRotate color='128 128 128' alpha=32 ");   // semi-transparent blue bar
	TwDefine(" SpotRotate resizable=false "); // mybar cannot be resized


	TwAddButton(m_bar, "label_01", NULL, NULL, "label='proof of concept for bill borded sprites with rotation'"); //show as label		
	TwAddButton(m_bar, "mainMenu", Callback, this, "label='main menu'"); //show as button				
}

bool APP_SpotRotate::Start()
{
	m_appName = "Spot rotate";

	m_time = 0.0f;

	Gizmos::create();

	GameCam = new Camera();

	// create shaders
	const char* vsSource = nullptr;
	std::string vsResult = LoadShader("./assets/shaders/RotateVertShader.vert");
	vsSource = vsResult.c_str();

	const char* fsSource = nullptr;
	std::string fsResult = LoadShader("./assets/shaders/RotateFragShader.frag");
	fsSource = fsResult.c_str();

	int success = GL_FALSE;
	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

	//glBindAttributeLocation

	glShaderSource(vertexShader, 1, (const char**)&vsSource, 0);
	glCompileShader(vertexShader);
	glShaderSource(fragmentShader, 1, (const char**)&fsSource, 0);
	glCompileShader(fragmentShader);
	m_programID = glCreateProgram();
	glAttachShader(m_programID, vertexShader);
	glAttachShader(m_programID, fragmentShader);
	glLinkProgram(m_programID);

	glGetProgramiv(m_programID, GL_LINK_STATUS, &success);
	if (success == GL_FALSE) {
		int infoLogLength = 0;
		glGetProgramiv(m_programID, GL_INFO_LOG_LENGTH, &infoLogLength);
		char* infoLog = new char[infoLogLength];
		glGetProgramInfoLog(m_programID, infoLogLength, 0, infoLog);
		printf("Error: Failed to link shader program!\n");
		printf("%s\n", infoLog);
		delete[] infoLog;
	}
	glDeleteShader(fragmentShader);
	glDeleteShader(vertexShader);

	// Generate our GL Buffers	
	glGenBuffers(1, &m_VBO);
	glGenBuffers(1, &m_IBO);
	glGenVertexArrays(1, &m_VAO); //generate a VertexArrayObject

	isLoaded = true;

	return true; //not being used in this lesson
}

bool APP_SpotRotate::Shutdown()
{
	delete GameCam;
	Gizmos::destroy();

	return true; //not being used in this lesson
}
