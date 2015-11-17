#include <APP_ProceduralGeneration.h>
#include <fstream>
#include <string>
#include <iostream>


APP_Proc_Generation::APP_Proc_Generation()
{

}
APP_Proc_Generation::~APP_Proc_Generation()
{
	delete perlinData;
}

void APP_Proc_Generation::Update(float a_dt)
{
	GameCam->Update(a_dt); //update camera
}

void APP_Proc_Generation::ClearMenu()
{
	TwDeleteBar(m_bar); //reset the gui
}

void APP_Proc_Generation::CreateGui()
{
	m_bar = TwNewBar("ProceduralGeneration");

	TwDefine(" ProceduralGeneration position='10 10' "); // move bar to position (10, 10)
	TwDefine(" ProceduralGeneration size='430 320' "); // resize bar	
	TwDefine(" ProceduralGeneration color='128 128 128' alpha=32 ");   // semi-transparent blue bar
	TwDefine(" ProceduralGeneration resizable=false "); // mybar cannot be resized


	TwAddButton(m_bar, "label_01", NULL, NULL, "label='procdural mesh, perlin noise and displacement creation'"); //show as label		
	TwAddButton(m_bar, "mainMenu", Callback, this, "label='main menu'"); //show as button				

	//resets the camera when app re-opens
	GameCam->SetPosition(vec3(52, 17.4, 70));
	GameCam->SetFront(vec3(-0.718, -0.47, -.8));
	GameCam->SetPitchYaw(-28, -114);

	GameCam->SetMouseSnapToCurrent();
	GameCam->ManualSnap();
}


void APP_Proc_Generation::Draw()
{
	Gizmos::clear();
	Gizmos::addTransform(glm::mat4(1));

	Gizmos::draw(GameCam->GetProjectionView());


	glUseProgram(m_program);						//select the program to use
	//bind buffers
	glBindVertexArray(m_VAO);						//bind vertex array object
	unsigned int projectionViewUniform = glGetUniformLocation(m_program, "view_proj");			//get the index of the ProjectionView uniform variable from the vertex shader
	glUniformMatrix4fv(projectionViewUniform, 1, false, glm::value_ptr(GameCam->GetProjectionView()));	//set the Projection View uniform variabe in the vertex shader

	//add perlin texture
	glActiveTexture(GL_TEXTURE0); //set for initial active texture
	glBindTexture(GL_TEXTURE_2D, m_perlin_texture);	//bind the crate texture
	// tell the shader where it is
	int difLoc = glGetUniformLocation(m_program, "perlin_texture"); //get diffuse location
	glUniform1i(difLoc, 0); //set to the diffuse to the texture index	
	difLoc = glGetUniformLocation(m_program, "perlin_texture"); //get diffuse location

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); //enable wireframe render
	glDrawElements(GL_TRIANGLES, m_indexCount, GL_UNSIGNED_INT, 0); //draw all triangles to screen
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); //enable fill render
	//unbind
	glBindVertexArray(0);

}

bool APP_Proc_Generation::Start()
{
	m_appName = "Procedural Generation";

	Gizmos::create();

	GameCam = new Camera();
//	GameCam->SetLookAt(vec3(100, 40, 50), vec3(32,0,32), vec3(0, 1, 0));
	
	//create shaders
	createShaders();

	//create base grid mesh
	rows = 64;
	cols = 64;

	createPerlinNoise();
	createPerlinBuffers();

	//add one to show correct rows/cols
	rows++;
	cols++;
	generateMesh();

	isLoaded = true;

	return true; //not being used in this lesson
}


void APP_Proc_Generation::createPerlinBuffers()
{
	glGenTextures(1, &m_perlin_texture);
	glBindTexture(GL_TEXTURE_2D, m_perlin_texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, 64, 64, 0, GL_RED, GL_FLOAT, perlinData);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}

void APP_Proc_Generation::createPerlinNoise()
{	
	int dims = 64;
	perlinData = new float[dims * dims];	
	float scale = (1.0f / dims) * 3;
	int octaves = 6;

	for (int x = 0; x < 64; ++x)
	{
		for (int y = 0; y < 64; ++y)
		{
			float amplitude = 1.0f;
			float persistence = 0.3f;
			perlinData[y * dims + x] = 0;
			for (int o = 0; o < octaves; ++o)
			{
				float freq = powf(12, (float)o);
				float perlin_sample =
					glm::perlin(glm::vec2((float)x, (float)y) * scale * freq) * 0.5f + 0.5f;
				perlinData[y * dims + x] += perlin_sample * amplitude;
				amplitude *= persistence;
			}
		}
	}		
}


std::string APP_Proc_Generation::LoadShader(const char *a_filePath)
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

	return strShaderCode; //not in use
}



void APP_Proc_Generation::createShaders()
{
	// create shaders
	const char* vsSource = nullptr;
	std::string vsResult = LoadShader("./assets/shaders/procGeneration/proceduralVertShader.vert");
	vsSource = vsResult.c_str();

	const char* fsSource = nullptr;
	std::string fsResult = LoadShader("./assets/shaders/procGeneration/proceduralFragShader.frag");
	fsSource = fsResult.c_str();

	int success = GL_FALSE;
	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

	glShaderSource(vertexShader, 1, (const char**)&vsSource, 0);
	glCompileShader(vertexShader);
	glShaderSource(fragmentShader, 1, (const char**)&fsSource, 0);
	glCompileShader(fragmentShader);
	m_program = glCreateProgram();
	glAttachShader(m_program, vertexShader);
	glAttachShader(m_program, fragmentShader);
	glLinkProgram(m_program);

	glGetProgramiv(m_program, GL_LINK_STATUS, &success);
	if (success == GL_FALSE) {
		int infoLogLength = 0;
		glGetProgramiv(m_program, GL_INFO_LOG_LENGTH, &infoLogLength);
		char* infoLog = new char[infoLogLength];
		glGetProgramInfoLog(m_program, infoLogLength, 0, infoLog);
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
}

void APP_Proc_Generation::generateMesh()
{
	Vertex* aoVertices = new Vertex[rows * cols]; //big ass array to hold all vert positions

	for (unsigned int row = 0; row < rows; ++row)
	{
		for (unsigned int column = 0; column < cols; ++column)
		{			
			aoVertices[row * cols + column].position = vec4((float)column, 0, (float)row, 1); // assign vertex position
			float u = (float)column / (cols - 1);
			float v = (float)row / (rows - 1);
			aoVertices[row * cols + column].uv = glm::vec2(u, v); //assign UV coords
		}
	}

	// defining index count based off quad count (2 triangles per quad)
	m_indexCount = (rows - 1) * (cols - 1) * 6; //index count is the number of vertices to make triangle NOT the vertices with positions
	unsigned int* auiIndices = new unsigned int[m_indexCount];
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

	createMeshBuffers(auiIndices, aoVertices);

	delete[] auiIndices;
	delete[] aoVertices;
}

void APP_Proc_Generation::createMeshBuffers(unsigned int* auiIndices, Vertex* aoVertices)
{
	//create buffers

	//bind buffers
	glBindVertexArray(m_VAO);						//bind vertex array object
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);			//vertex buffer objet
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBO);	//index buffer object
	//enable arrays for incoming data
	glEnableVertexAttribArray(0);					//enable vertex positions in the vertex shader
	glEnableVertexAttribArray(1);					//enable colour in the vertex shader

	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);	//index of poisition within the vertex object
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(Offsets::TexCoord2Offset)); //index position of the uv data - 1 vec 4 into the vertex block

	glBufferData(GL_ARRAY_BUFFER, (rows * cols) * sizeof(Vertex), aoVertices, GL_STATIC_DRAW); //push the vertices into vertex buffer object
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, (rows - 1) * (cols - 1) * 6 * sizeof(unsigned int), auiIndices, GL_STATIC_DRAW); //push the vertice indexs into the index buffer object

	//unbind and delete pointers
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

bool APP_Proc_Generation::Shutdown()
{
	delete GameCam;
	Gizmos::destroy();

	return true; //not being used in this lesson
}