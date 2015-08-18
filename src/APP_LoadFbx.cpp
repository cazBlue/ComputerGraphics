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

	// use our texture program
	glUseProgram(m_programID);
	// bind the camera
	int loc = glGetUniformLocation(m_programID, "ProjectionView");
	glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(GameCam->GetProjectionView()));
	// set texture slot
	glActiveTexture(GL_TEXTURE0); //set for initial active texture
	glBindTexture(GL_TEXTURE_2D, m_textureID1);	//bind the crate texture
	// tell the shader where it is
	int difLoc = glGetUniformLocation(m_programID, "diffuse"); //get diffuse location
	glUniform1i(difLoc, 0); //set to the diffuse to the texture index	
	difLoc = glGetUniformLocation(m_programID, "diffuse"); //get diffuse location

	glActiveTexture(GL_TEXTURE0 + 1);
	glBindTexture(GL_TEXTURE_2D, m_textureID2);
	int blendLoc = glGetUniformLocation(m_programID, "blend");
	glUniform1i(blendLoc, 1);

	// draw
	glBindVertexArray(m_vao);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
}

void APP_LoadFbx::loadImg(int* a_height, int* a_width, int* a_format, const char* a_path, unsigned int* a_id)
{
	unsigned char* data = stbi_load(a_path, a_width, a_height, a_format, STBI_rgb); //request no alpha

	glGenTextures(1, a_id);
	glBindTexture(GL_TEXTURE_2D, (*a_id));
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, (*a_width), (*a_height), 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	stbi_image_free(data); //unload the image data
}

bool APP_LoadFbx::Start()
{
	Gizmos::create();
	GameCam = new Camera();

	//FBXFile* fbxFile = new FBXFile();

	fbxFile = new FBXFile();

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
	bool didLoad = fbxFile->load(path, fbxFile->UNITS_METER, true, true, true);
	if (didLoad)
		printf("loaded");
	else
		printf("no load");



	//hell year
	FBXNode* root = fbxFile->getRoot();

	
	//fbxFile->unload(); //called in deconstructor

//	delete fbxFile;

	fbxFile->initialiseOpenGLTextures();

	//fbxFile.getMeshByName()

//	int imageWidth = 0, imageHeight = 0, imageFormat = 0;
//	int imageWidth2 = 0, imageHeight2 = 0, imageFormat2 = 0;
//	m_textureID1 = 0;
//	m_textureID2 = 0;
//	m_textureID3 = 0;

//	loadImg(&imageHeight, &imageWidth, &imageFormat, "./assets/textures/crate.png", &m_textureID1);
//	loadImg(&imageHeight2, &imageWidth2, &imageFormat2, "./assets/textures/redarrow.png", &m_textureID2);	

	//////////////create shaders and program
	const char* vsSource = "#version 410\n \
		layout(location=0) in vec4 Position; \
		layout(location=1) in vec2 TexCoord; \
		out vec2 vTexCoord; \
		uniform mat4 ProjectionView; \
		void main() { \
		vTexCoord = TexCoord; \
		gl_Position= ProjectionView * Position;\
	}";
	const char* fsSource = "#version 410\n \
		in vec2 vTexCoord; \
		out vec4 FragColor; \
		uniform sampler2D diffuse; \
		uniform sampler2D blend; \
		void main() { \
		FragColor = texture(diffuse,vTexCoord) * texture(blend,vTexCoord);\
	}";

	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, (const char**)&vsSource, 0);
	glCompileShader(vertexShader);
	unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, (const char**)&fsSource, 0);
	glCompileShader(fragmentShader);
	m_programID = glCreateProgram();
	glAttachShader(m_programID, vertexShader);
	glAttachShader(m_programID, fragmentShader);
	glLinkProgram(m_programID);
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	//generate buffers and mesh information
	float vertexData[] = {
		-5, 0, 5, 1, 0, 1, //vertex position
		5, 0, 5, 1, 1, 1,
		5, 0, -5, 1, 1, 0, //tex coords
		-5, 0, -5, 1, 0, 0,
	};
	unsigned int indexData[] = {
		0, 1, 2,
		0, 2, 3,
	};

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

	m_gl_info.resize(fbxFile->getMeshCount()); //set gl object array size

	//loop through all meshes loaded
	for (unsigned int meshIndex = 0; meshIndex < fbxFile->getMeshCount(); ++meshIndex)
	{
		printf("looping through: %d ", meshIndex);

		FBXMeshNode* meshNode = fbxFile->getMeshByIndex(0);
		//std::vector<FBXVertex> vertices = meshNode->m_vertices;

		unsigned int size = (*meshNode).m_vertices.size();

		for (unsigned int vertIndex = 0; vertIndex < size; ++vertIndex)
		{
			printf("added vert: %d", vertIndex);
		}

		std::vector<float> vertex_data;
		//unsigned int size = meshNode->m_vertices;
		

		glGenVertexArrays(1, &m_gl_info[meshIndex].m_VAO);
		glGenBuffers(1, &m_gl_info[meshIndex].m_IBO);
		glGenBuffers(1, &m_gl_info[meshIndex].m_VBO);

		glBindVertexArray(m_gl_info[meshIndex].m_VAO);
		glBindBuffer(GL_ARRAY_BUFFER, m_gl_info[meshIndex].m_VBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_gl_info[meshIndex].m_IBO);



		glBufferData(GL_ARRAY_BUFFER, sizeof(float)* 6 * 4, vertexData , GL_STATIC_DRAW);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int)* 6, indexData, GL_STATIC_DRAW);
		
		//set position
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(float)* 6, 0);
		
		//set colour - TODO check if colour or tex verts
		glEnableVertexAttribArray(1);		
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float)* 6, ((char*)0) + 16);
		
		//clear buffers
		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

	return true; //not being used in this lesson
}


void APP_LoadFbx::createOpenGLBuffers()
{
	//loop through all meshes in fbx 
}

bool APP_LoadFbx::Shutdown()
{
	delete GameCam;
	Gizmos::destroy();

	delete fbxFile;

	return true; //not being used in this lesson
}