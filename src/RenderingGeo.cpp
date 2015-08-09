#include <RenderingGeo.h>
#include <fstream>
#include <string>
#include <iostream>

struct Vertex {
	vec4 position;
	vec4 colour;
};

RenderGeo::RenderGeo()
{

}
RenderGeo::~RenderGeo()
{

}

// function to create a grid
void RenderGeo::generateGrid(unsigned int rows, unsigned int cols) {
	Vertex* aoVertices = new Vertex[rows * cols];
	for (unsigned int r = 0; r < rows; ++r) {
		for (unsigned int c = 0; c < cols; ++c) {
			aoVertices[r * cols + c].position = vec4(
				(float)c, 0, (float)r, 1);
			// create some arbitrary colour based off something
			// that might not be related to tiling a texture
			vec3 colour = vec3(sinf((c / (float)(cols - 1)) *
				(r / (float)(rows - 1))));
			aoVertices[r * cols + c].colour = vec4(colour, 1);
		}
	}

	// defining index count based off quad count (2 triangles per quad)
	unsigned int* auiIndices = new unsigned int[(rows - 1) * (cols - 1) *
		6];
	unsigned int index = 0;
	for (unsigned int r = 0; r < (rows - 1); ++r) {
		for (unsigned int c = 0; c < (cols - 1); ++c) {
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


	// Generate our GL Buffers
	// Lets move these so that they are all generated together
	glGenBuffers(1, &m_VBO);
	glGenBuffers(1, &m_IBO);
	//Add the following line to generate a VertexArrayObject
	glGenVertexArrays(1, &m_VAO);
	glBindVertexArray(m_VAO);

	// create and bind buffers to a vertex array object
	//glGenBuffers(1, &m_VBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBufferData(GL_ARRAY_BUFFER, (rows * cols) * sizeof(Vertex), aoVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(vec4)));
	//glBindBuffer(GL_ARRAY_BUFFER, 0);

	//glGenBuffers(1, &m_IBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, (rows - 1) * (cols - 1) * 6 * sizeof(unsigned int), auiIndices, GL_STREAM_DRAW);
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	// ....Code Segment here to bind and fill VBO + IBO

	glUseProgram(m_programID);
	unsigned int projectionViewUniform = glGetUniformLocation(m_programID, "ProjectionView");
	glUniformMatrix4fv(projectionViewUniform, 1, false, glm::value_ptr(GameCam->GetProjectionView()));
	glBindVertexArray(m_VAO);
	unsigned int indexCount = (rows - 1) * (cols - 1) * 6;
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	delete[] auiIndices;
	delete[] aoVertices;
}

void RenderGeo::Update(float a_dt)
{
	GameCam->Update(a_dt); //update camera


}

void RenderGeo::Draw()
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

	generateGrid(6, 6); //draw the grid
}

std::string RenderGeo::LoadShader(const char *a_filePath)
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

bool RenderGeo::Start()
{
	Gizmos::create();

	GameCam = new Camera();

	// create shaders
	const char* vsSource = nullptr;
	std::string vsResult = LoadShader("./assets/shaders/VertexShader.glsl");
	vsSource = vsResult.c_str();

	const char* fsSource = nullptr;
	std::string fsResult = LoadShader("./assets/shaders/FragShader.glsl");
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

	return true; //not being used in this lesson
}

bool RenderGeo::Shutdown()
{
	delete GameCam;
	Gizmos::destroy();

	return true; //not being used in this lesson
}
