#include <APP_LoadFbx.h>
#include <vector>


//#define STB_IMAGE_IMPLEMENTATION //declared in fbx loader
#include <stb_image.h>
#include <FBXFile.h>

#include <iostream>
#include <fstream>

using namespace std;

APP_LoadFbx::APP_LoadFbx()
{

}
APP_LoadFbx::~APP_LoadFbx()
{

}

void APP_LoadFbx::Update(float a_dt)
{
	GameCam->Update(a_dt); //update camera
}

void APP_LoadFbx::Draw()
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
	
	// bind the light position
	int lightDirUniform = glGetUniformLocation(m_program, "lightDir");	//get the Time uniform index from the vertex shader
	glm::vec3 light = glm::vec3(1, 1, 0); //controls the lights position in the world	
	glUniform3fv(lightDirUniform, 1, glm::value_ptr(light));	//set the lightDir uniform variabe in the vertex shader

	// bind change the light colour
	int lightColUniform = glGetUniformLocation(m_program, "lightColour");	//get the Time uniform index from the vertex shader
	glm::vec4 lightColour = glm::vec4(.2, .2, .8, 1); //controls the lights position in the world	
	glUniform4fv(lightColUniform, 1, glm::value_ptr(lightColour));	//set the lightDir uniform variabe in the vertex shader

	// bind change the camera position
	int cameraPosUniform = glGetUniformLocation(m_program, "CameraPos");	//get the Time uniform index from the vertex shader
	glm::vec3 cameraPos = glm::vec3(GameCam->m_worldTransform[3]); //controls the lights position in the world		
	glUniform3fv(cameraPosUniform, 1, glm::value_ptr(cameraPos));	//set the lightDir uniform variabe in the vertex shader

	// bind change the spec power
	int specPosUniform = glGetUniformLocation(m_program, "SpecPow");	//get the Time uniform index from the vertex shader
	GLfloat specPow = 128.0f; //controls the lights position in the world		
	glUniform1f(specPosUniform, specPow);	//set the lightDir uniform variabe in the vertex shader

	// bind our vertex array object and draw the mesh
	for (unsigned int i = 0; i < m_fbx->getMeshCount(); ++i) {
		FBXMeshNode* mesh = m_fbx->getMeshByIndex(i);
		unsigned int* glData = (unsigned int*)mesh->m_userData;
		glBindVertexArray(glData[0]);
		glDrawElements(GL_TRIANGLES,
			(unsigned int)mesh->m_indices.size(), GL_UNSIGNED_INT, 0);
	}
}

//void APP_LoadFbx::loadImg(int* a_height, int* a_width, int* a_format, const char* a_path, unsigned int* a_id)
//{
//	unsigned char* data = stbi_load(a_path, a_width, a_height, a_format, STBI_rgb); //request no alpha
//
//	glGenTextures(1, a_id);
//	glBindTexture(GL_TEXTURE_2D, (*a_id));
//	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, (*a_width), (*a_height), 0, GL_RGB, GL_UNSIGNED_BYTE, data);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//
//	stbi_image_free(data); //unload the image data
//}

std::string APP_LoadFbx::LoadShader(const char *a_filePath)
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

bool APP_LoadFbx::Start()
{
	Gizmos::create();
	GameCam = new Camera();

	//FBXFile* fbxFile = new FBXFile();

	m_fbx = new FBXFile();

	//fbxFile->unload();


	std::string strShaderCode; //file info holder --TODO create array of file names
	//open shader file
	std::ifstream shaderStream("./assets/fbxToLoad.txt");
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

	//issue tracked to line 184 in FBXFile.cpp
	bool didLoad = m_fbx->load(path, m_fbx->UNITS_METER, true, true, true);
	if (didLoad)
		printf("loaded");
	else
		printf("no load");
	
	createOpenGLBuffers(m_fbx);

	//////////////create shaders and program	
	const char* vsSource = nullptr;
	std::string vsResult = LoadShader("./assets/shaders/FBXVertexShader.glsl");
	vsSource = vsResult.c_str();	
	const char* fsSource = nullptr;
	std::string fsResult = LoadShader("./assets/shaders/FBXFragShader.glsl");
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
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);



//	FBXMeshNode* mesh = fbxFile.getMeshByIndex(0);

//	std::vector<FBXVertex> verts = mesh->m_vertices;
//	
//	for (auto it = verts.begin(); it != verts.end(); ++it)
//	{
//		it->position;
//		int temp = 0;
//	}
//
	//FBXVertex* vertex = mesh->get;

//	m_gl_info.resize(fbxFile->getMeshCount()); //set gl object array size

	//loop through all meshes loaded
//	for (unsigned int meshIndex = 0; meshIndex < fbxFile->getMeshCount(); ++meshIndex)
//	{
//		printf("looping through: %d ", meshIndex);
//
//		FBXMeshNode* meshNode = fbxFile->getMeshByIndex(0);
//		//std::vector<FBXVertex> vertices = meshNode->m_vertices;
//
//		unsigned int size = (*meshNode).m_vertices.size();
//
//		for (unsigned int vertIndex = 0; vertIndex < size; ++vertIndex)
//		{
//			printf("added vert: %d", vertIndex);
//		}
//
//		std::vector<float> vertex_data;
//		//unsigned int size = meshNode->m_vertices;
//		
//
//		glGenVertexArrays(1, &m_gl_info[meshIndex].m_VAO);
//		glGenBuffers(1, &m_gl_info[meshIndex].m_IBO);
//		glGenBuffers(1, &m_gl_info[meshIndex].m_VBO);
//
//		glBindVertexArray(m_gl_info[meshIndex].m_VAO);
//		glBindBuffer(GL_ARRAY_BUFFER, m_gl_info[meshIndex].m_VBO);
//		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_gl_info[meshIndex].m_IBO);
//
//
//
//		glBufferData(GL_ARRAY_BUFFER, sizeof(float)* 6 * 4, vertexData , GL_STATIC_DRAW);
//		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int)* 6, indexData, GL_STATIC_DRAW);
//		
//		//set position
//		glEnableVertexAttribArray(0);
//		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(float)* 6, 0);
//		
//		//set colour - TODO check if colour or tex verts
//		glEnableVertexAttribArray(1);		
//		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float)* 6, ((char*)0) + 16);
//		
//		//clear buffers
//		glBindVertexArray(0);
//		glBindBuffer(GL_ARRAY_BUFFER, 0);
//		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
//	}

	return true; //not being used in this lesson
}


void APP_LoadFbx::createOpenGLBuffers(FBXFile* fbx)
{
	// create the GL VAO/VBO/IBO data for each mesh
	for (unsigned int i = 0; i < fbx->getMeshCount(); ++i)
	{
		FBXMeshNode* mesh = fbx->getMeshByIndex(i);
		// storage for the opengl data in 3 unsigned int
		unsigned int* glData = new unsigned int[3];

		glGenVertexArrays(1, &glData[0]);
		glBindVertexArray(glData[0]);
		glGenBuffers(1, &glData[1]);
		glGenBuffers(1, &glData[2]);
		glBindBuffer(GL_ARRAY_BUFFER, glData[1]);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, glData[2]);
		glBufferData(GL_ARRAY_BUFFER,
			mesh->m_vertices.size() * sizeof(FBXVertex),
			mesh->m_vertices.data(), GL_STATIC_DRAW);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER,
			mesh->m_indices.size() * sizeof(unsigned int),
			mesh->m_indices.data(), GL_STATIC_DRAW);
		glEnableVertexAttribArray(0); // position
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE,
			sizeof(FBXVertex), 0);
		glEnableVertexAttribArray(1); // normal
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_TRUE,
			sizeof(FBXVertex),
			((char*)0) + FBXVertex::NormalOffset);
		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		mesh->m_userData = glData;
	}
}

void APP_LoadFbx::cleanupOpenGLBuffers(FBXFile* fbx) {
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



bool APP_LoadFbx::Shutdown()
{
	delete GameCam;
	Gizmos::destroy();

	delete m_fbx;

	cleanupOpenGLBuffers(m_fbx);
	glDeleteProgram(m_program);

	return true; //not being used in this lesson
}