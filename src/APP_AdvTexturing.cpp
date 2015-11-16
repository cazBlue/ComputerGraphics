#include <APP_AdvTexturing.h>
#include <vector>


//#define STB_IMAGE_IMPLEMENTATION //declared in fbx loader
#include <stb_image.h>
#include <FBXFile.h>

#include <iostream>
#include <fstream>

using namespace std;

APP_AdvTex::APP_AdvTex()
{

}
APP_AdvTex::~APP_AdvTex()
{

}

void APP_AdvTex::Update(float a_dt)
{
	GameCam->Update(a_dt); //update camera
}

void APP_AdvTex::Draw()
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

	// bind the point light position for a directional light
	lightDirUniform = glGetUniformLocation(m_program, "pointLight");	//get the point light uniform index from the vertex shader
	glm::vec3 pointLight = glm::vec3(0, -1, -1); //controls the lights position in the world	
	glUniform3fv(lightDirUniform, 1, glm::value_ptr(pointLight));	//set the lightDir uniform variabe in the vertex shader

	// bind change the light colour
	int lightColUniform = glGetUniformLocation(m_program, "lightColour");	//get the Time uniform index from the vertex shader
	glm::vec3 lightColour = glm::vec3(1, 1, 1); //controls the lights colour
	glUniform3fv(lightColUniform, 1, glm::value_ptr(lightColour));	//set the lightDir uniform variabe in the vertex shader

	// bind change the camera position
	int cameraPosUniform = glGetUniformLocation(m_program, "CameraPos");	//get the Time uniform index from the vertex shader
	glm::vec3 cameraPos = glm::vec3(GameCam->GetWorldTransform()[3]); //controls the lights position in the world		
	glUniform3fv(cameraPosUniform, 1, glm::value_ptr(cameraPos));	//set the lightDir uniform variabe in the vertex shader

	// bind change the spec power
	int specPosUniform = glGetUniformLocation(m_program, "SpecPow");	//get the Time uniform index from the vertex shader
	GLfloat specPow = 1024; //controls the lights position in the world		
	glUniform1f(specPosUniform, specPow);	//set the lightDir uniform variabe in the vertex shader

	// bind our vertex array object and draw the mesh
	for (unsigned int i = 0; i < m_fbx->getMeshCount(); ++i) {
		FBXMeshNode* mesh = m_fbx->getMeshByIndex(i);
		unsigned int* glData = (unsigned int*)mesh->m_userData;

		//set diffuse texture	
		glActiveTexture(GL_TEXTURE0); //set for initial active texture		
		glBindTexture(GL_TEXTURE_2D, glData[3]);	//bind the diffuse texture
		int difLoc = glGetUniformLocation(m_program, "Diffuse"); //get diffuse location
		glUniform1i(difLoc, 0); //set to the diffuse to the texture index	

		//set normal texture	
		glActiveTexture(GL_TEXTURE0 + 1); //set for initial active texture		
		glBindTexture(GL_TEXTURE_2D, glData[4]);	//bind the crate texture
		int normalLoc = glGetUniformLocation(m_program, "NormalTex"); //get diffuse location
		glUniform1i(normalLoc, 1); //set to the diffuse to the texture index	

		glBindVertexArray(glData[0]);
		glDrawElements(GL_TRIANGLES,
			(unsigned int)mesh->m_indices.size(), GL_UNSIGNED_INT, 0);
	}
}

void APP_AdvTex::loadImg(int* a_height, int* a_width, int* a_format, const char* a_path, unsigned int* a_id)
{
	unsigned char* data = stbi_load(a_path, a_width, a_height, a_format, STBI_rgb); //request no alpha

	glGenTextures(1, a_id);
	glBindTexture(GL_TEXTURE_2D, (*a_id));
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, (*a_width), (*a_height), 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	stbi_image_free(data); //unload the image data
}

std::string APP_AdvTex::LoadShader(const char *a_filePath)
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

void APP_AdvTex::ClearMenu()
{
	TwDeleteBar(m_bar); //reset the gui
}

void APP_AdvTex::CreateGui()
{
	m_bar = TwNewBar("AdvancedTexturing");

	TwDefine(" AdvancedTexturing position='10 10' "); // move bar to position (10, 10)
	TwDefine(" AdvancedTexturing size='430 320' "); // resize bar	
	TwDefine(" AdvancedTexturing color='128 128 128' alpha=32 ");   // semi-transparent blue bar
	TwDefine(" AdvancedTexturing resizable=false "); // mybar cannot be resized


	TwAddButton(m_bar, "label_01", NULL, NULL, "label='advanced texturing, diffuse normal map and directional/point light'"); //show as label		
	TwAddButton(m_bar, "mainMenu", Callback, this, "label='main menu'"); //show as button				
}

bool APP_AdvTex::Start()
{
	m_appName = "Advanced Texturing";

	Gizmos::create();
	GameCam = new Camera();

	//FBXFile* fbxFile = new FBXFile();

	m_fbx = new FBXFile();

	//fbxFile->unload();


	std::string strShaderCode; //file info holder --TODO create array of file names
	//open shader file
	std::ifstream shaderStream("./assets/fbxToLoadAdvTex.txt");
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


	m_fbx = importCtrl->m_FBX_soulSpear;
	
//	bool didLoad = m_fbx->load(path, m_fbx->UNITS_METER, true, true, true);
//	if (didLoad)
//		printf("loaded");
//	else
//		printf("no load");

	//////////////create shaders and program	
	const char* vsSource = nullptr;
	std::string vsResult = LoadShader("./assets/shaders/AdvTexVertexShader.glsl");
	vsSource = vsResult.c_str();

	const char* fsSource = nullptr;
	std::string fsResult = LoadShader("./assets/shaders/AdvTexFragShader.glsl");
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

//	createOpenGLBuffers(m_fbx);

	isLoaded = true;

	return true; //not being used in this lesson
}


void APP_AdvTex::createOpenGLBuffers(FBXFile* fbx)
{
	unsigned int meshCount = fbx->getMeshCount();
	unsigned int materialCount = fbx->getMaterialCount();
	// create the GL VAO/VBO/IBO data for each mesh	
	for (unsigned int i = 0; i < meshCount; ++i)
	{
		FBXMeshNode* mesh = fbx->getMeshByIndex(i);

		FBXMaterial* mat = mesh->m_material;

		FBXTexture* diffuesTex = mat->textures[FBXMaterial::TextureTypes::DiffuseTexture];
		FBXTexture* normalTex = mat->textures[FBXMaterial::TextureTypes::NormalTexture];

		// storage for the opengl data in 4 unsigned int (includes diffuse texID (3) and normal texID (4))
		unsigned int* glData = new unsigned int[5];

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
		glEnableVertexAttribArray(2); // colour / tex cords
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_TRUE, sizeof(FBXVertex), ((char*)0) + FBXVertex::TexCoord1Offset);
		glEnableVertexAttribArray(3); // tangent coords / tex cords
		glVertexAttribPointer(3, 4, GL_FLOAT, GL_TRUE, sizeof(FBXVertex), ((char*)0) + FBXVertex::TangentOffset);
		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		//gen diffuse texture id and bind texture to buffer 
		glActiveTexture(GL_TEXTURE0); //texture are we binding to
		glGenTextures(1, &glData[3]);
		glBindTexture(GL_TEXTURE_2D, glData[3]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, diffuesTex->width, diffuesTex->height, 0, GL_RGB, GL_UNSIGNED_BYTE, diffuesTex->data);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

//		int difLoc = glGetUniformLocation(m_program, "Diffuse"); //get diffuse location
//		glUniform1i(difLoc, 0); //set to the diffuse to the texture index	

		//gen normal texture id and bind texture to buffer 
//		glActiveTexture(GL_TEXTURE0 + 1);
		glActiveTexture(GL_TEXTURE0 + 1); //texture are we binding to
		glGenTextures(1, &glData[4]);
		glBindTexture(GL_TEXTURE_2D, glData[4]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, normalTex->width, normalTex->height, 0, GL_RGB, GL_UNSIGNED_BYTE, normalTex->data);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);	

//		glActiveTexture(GL_TEXTURE0 + 1);
//		int normalLoc = glGetUniformLocation(m_program, "Normal"); //get normal location
//		glUniform1i(normalLoc, 1); //set to the diffuse to the texture index			

		mesh->m_userData = glData;
	}

	//create GL id for textures and load then to buffer
	for (unsigned int i = 0; i < materialCount; ++i)
	{
		FBXMaterial* mat = fbx->getMaterialByIndex(i);

		FBXTexture* tex = mat->textures[FBXMaterial::TextureTypes::DiffuseTexture];

		int temp = 0;
		//		std::string path = mat->textures

		//		unsigned char* data = stbi_load(a_path, a_width, a_height, a_format, STBI_rgb); //request no alpha
		//
		//		glGenTextures(1, a_id);
		//		glBindTexture(GL_TEXTURE_2D, (*a_id));
		//		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, (*a_width), (*a_height), 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		//		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		//		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		//
		//		stbi_image_free(data); //unload the image data
	}
}


void APP_AdvTex::cleanupOpenGLBuffers(FBXFile* fbx) {
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



bool APP_AdvTex::Shutdown()
{
	delete GameCam;
	Gizmos::destroy();

//	delete m_fbx;
	
	glDeleteProgram(m_program);

	return true; //not being used in this lesson
}