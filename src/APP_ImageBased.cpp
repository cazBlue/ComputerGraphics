#include <APP_ImageBased.h>
#include <vector>


//#define STB_IMAGE_IMPLEMENTATION //declared in fbx loader
#include <stb_image.h>
#include <FBXFile.h>

#include <iostream>
#include <fstream>

using namespace std;

APP_ImageBased::APP_ImageBased()
{

}
APP_ImageBased::~APP_ImageBased()
{

}

void APP_ImageBased::Update(float a_dt)
{
	GameCam->Update(a_dt); //update camera
}

void APP_ImageBased::Draw()
{
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

	Gizmos::draw(GameCam->GetProjectionView());

	glUseProgram(m_program);
	// bind the camera
	int loc = glGetUniformLocation(m_program, "ProjectionView");
	glUniformMatrix4fv(loc, 1, GL_FALSE, &(GameCam->GetProjectionView()[0][0]));
	
	int lightDirUniform = -1;
	// bind the directional light position for a directional light
	lightDirUniform = glGetUniformLocation(m_program, "directionalLight");	//get the directional light uniform index from the vertex shader
	glm::vec3 dirLight = glm::vec3(sin(glfwGetTime()), 1, cos(glfwGetTime())); //controls the lights position in the world	
	glUniform3fv(lightDirUniform, 1, glm::value_ptr(dirLight));	//set the lightDir uniform variabe in the vertex shader


	// bind our vertex array object and draw the mesh
	for (unsigned int i = 0; i < m_fbx->getMeshCount(); ++i) {
		FBXMeshNode* mesh = m_fbx->getMeshByIndex(i);
		unsigned int* glData = (unsigned int*)mesh->m_userData;

		//set diffuse texture	
		glActiveTexture(GL_TEXTURE0); //set for initial active texture		
		int difLoc = glGetUniformLocation(m_program, "WarpTexture"); //get diffuse location
		glUniform1i(difLoc, 0); //set to the diffuse to the texture index	

		glBindVertexArray(glData[0]);
		glDrawElements(GL_TRIANGLES,
			(unsigned int)mesh->m_indices.size(), GL_UNSIGNED_INT, 0);
	}
}

void APP_ImageBased::loadImg(int* a_height, int* a_width, int* a_format, const char* a_path, unsigned int* a_id)
{
	unsigned char* data = stbi_load(a_path, a_width, a_height, a_format, STBI_rgb); //request no alpha

	glGenTextures(1, a_id);
	glBindTexture(GL_TEXTURE_2D, (*a_id));
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, (*a_width), (*a_height), 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	stbi_image_free(data); //unload the image data
}

std::string APP_ImageBased::LoadShader(const char *a_filePath)
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

bool APP_ImageBased::Start()
{
	Gizmos::create();
	GameCam = new Camera();	

	int imageWidth = 0, imageHeight = 0, imageFormat = 0;
	m_textureID1 = 0;

	loadImg(&imageHeight, &imageWidth, &imageFormat, "./assets/textures/1d_blue.tga", &m_textureID1);


	m_fbx = new FBXFile();

	//fbxFile->unload();


	std::string strShaderCode; //file info holder --TODO create array of file names
	//open shader file
	std::ifstream shaderStream("./assets/deferredFBX.txt");
	//if that worked ok, load file line by line

	if (shaderStream.is_open())
	{
		std::string Line = "";
		while (std::getline(shaderStream, Line))
		{
			//strShaderCode += "\n" + Line;
			strShaderCode += Line;
		}
		shaderStream.close();
	}

	const char* path = strShaderCode.c_str();
	//fsSource = fsResult.c_str();

	
	bool didLoad = m_fbx->load(path, m_fbx->UNITS_METER, true, true, true);
	if (didLoad)
		printf("loaded");
	else
		printf("no load");

	//////////////create shaders and program	
	const char* vsSource = nullptr;
	std::string vsResult = LoadShader("./assets/shaders/imageBased/ImageVertexShader.glsl");
	vsSource = vsResult.c_str();

	const char* fsSource = nullptr;
	std::string fsResult = LoadShader("./assets/shaders/imageBased/ImageFragShader.glsl");
	fsSource = fsResult.c_str();

	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, (const char**)&vsSource, 0);
	glCompileShader(vertexShader);
	unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, (const char**)&fsSource, 0);
	glCompileShader(fragmentShader);
	m_program = glCreateProgram();
	glAttachShader(m_program, vertexShader);
	glAttachShader(m_program, fragmentShader);
	glLinkProgram(m_program);

	int success;

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

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	createOpenGLBuffers(m_fbx);

	return true; //not being used in this lesson
}


void APP_ImageBased::createOpenGLBuffers(FBXFile* fbx)
{
	unsigned int meshCount = fbx->getMeshCount();
	unsigned int materialCount = fbx->getMaterialCount();
	// create the GL VAO/VBO/IBO data for each mesh	
	for (unsigned int i = 0; i < meshCount; ++i)
	{
		FBXMeshNode* mesh = fbx->getMeshByIndex(i);

		FBXMaterial* mat = mesh->m_material;

//		FBXTexture* diffuesTex = mat->textures[FBXMaterial::TextureTypes::DiffuseTexture];
//		FBXTexture* normalTex = mat->textures[FBXMaterial::TextureTypes::NormalTexture];
//		FBXTexture* specTex = mat->textures[FBXMaterial::TextureTypes::SpecularTexture];

		// storage for the opengl data in 4 unsigned int (includes diffuse texID (3) and normal texID (4))
		unsigned int* glData = new unsigned int[3];

		glGenVertexArrays(1, &glData[0]);
		glBindVertexArray(glData[0]);
		glGenBuffers(1, &glData[1]);
		glGenBuffers(1, &glData[2]);
		glBindBuffer(GL_ARRAY_BUFFER, glData[1]);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, glData[2]);
		glBufferData(GL_ARRAY_BUFFER, mesh->m_vertices.size() * sizeof(FBXVertex), mesh->m_vertices.data(), GL_STATIC_DRAW);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh->m_indices.size() * sizeof(unsigned int), mesh->m_indices.data(), GL_STATIC_DRAW);
		glEnableVertexAttribArray(0); // position
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(FBXVertex), 0);
		glEnableVertexAttribArray(1); // normal
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_TRUE, sizeof(FBXVertex), ((char*)0) + FBXVertex::NormalOffset);

		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		mesh->m_userData = glData;
	}
}

void APP_ImageBased::cleanupOpenGLBuffers(FBXFile* fbx) {
	// clean up the vertex data attached to each mesh
	for (unsigned int i = 0; i < fbx->getMeshCount(); ++i) {
		FBXMeshNode* mesh = fbx->getMeshByIndex(i);
		unsigned int* glData = (unsigned int*)mesh->m_userData;
		glDeleteVertexArrays(1, &glData[0]);
		glDeleteBuffers(1, &glData[1]);
		glDeleteBuffers(1, &glData[2]);
		delete[] glData;
	}
}

bool APP_ImageBased::Shutdown()
{
	delete GameCam;
	Gizmos::destroy();

	delete m_fbx;

	cleanupOpenGLBuffers(m_fbx);
	glDeleteProgram(m_program);

	return true; //not being used in this lesson
}