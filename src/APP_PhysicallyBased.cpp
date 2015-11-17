#include <APP_PhysicallyBased.h>
#include <vector>


//#define STB_IMAGE_IMPLEMENTATION //declared in fbx loader
#include <stb_image.h>
#include <FBXFile.h>

#include <iostream>
#include <fstream>

using namespace std;

APP_PhysicallyBased::APP_PhysicallyBased()
{

}
APP_PhysicallyBased::~APP_PhysicallyBased()
{

}

void APP_PhysicallyBased::Update(float a_dt)
{
	GameCam->Update(a_dt); //update camera
}

void APP_PhysicallyBased::Draw()
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
		glBindTexture(GL_TEXTURE_2D, glData[3]);
		int difLoc = glGetUniformLocation(m_program, "Diffuse"); //get diffuse location
		glUniform1i(difLoc, 0); //set to the diffuse to the texture index	

		//set normal texture	
		glActiveTexture(GL_TEXTURE0 + 1); //set for initial active texture		
		glBindTexture(GL_TEXTURE_2D, glData[4]);
		int normalLoc = glGetUniformLocation(m_program, "NormalTex"); //get diffuse location
		glUniform1i(normalLoc, 1); //set to the normal to the texture index	

		//set spec texture	
		glActiveTexture(GL_TEXTURE0 + 2); //set for initial active texture		
		glBindTexture(GL_TEXTURE_2D, glData[5]);
		int specLoc = glGetUniformLocation(m_program, "SpecTex"); //get diffuse location
		glUniform1i(specLoc, 2); //set to the specular to the texture index	

		glBindVertexArray(glData[0]);
		glDrawElements(GL_TRIANGLES,
			(unsigned int)mesh->m_indices.size(), GL_UNSIGNED_INT, 0);
	}
}

void APP_PhysicallyBased::loadImg(int* a_height, int* a_width, int* a_format, const char* a_path, unsigned int* a_id)
{
	unsigned char* data = stbi_load(a_path, a_width, a_height, a_format, STBI_rgb); //request no alpha

	glGenTextures(1, a_id);
	glBindTexture(GL_TEXTURE_2D, (*a_id));
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, (*a_width), (*a_height), 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	stbi_image_free(data); //unload the image data
}

std::string APP_PhysicallyBased::LoadShader(const char *a_filePath)
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

bool APP_PhysicallyBased::Start()
{
	m_appName = "Physically based";

	Gizmos::create();
	GameCam = new Camera();

	m_fbx = importCtrl->m_FBX_soulSpear;


	//////////////create shaders and program	
	const char* vsSource = nullptr;
	std::string vsResult = LoadShader("./assets/shaders/physicalShaders/PhysicalVertexShader.glsl");
	vsSource = vsResult.c_str();

	const char* fsSource = nullptr;
	std::string fsResult = LoadShader("./assets/shaders/physicalShaders/PhysicalFragShader.glsl");
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


void APP_PhysicallyBased::createOpenGLBuffers(FBXFile* fbx)
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
		FBXTexture* specTex = mat->textures[FBXMaterial::TextureTypes::SpecularTexture];

		// storage for the opengl data in 4 unsigned int (includes diffuse texID (3) and normal texID (4))
		unsigned int* glData = new unsigned int[6];

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

		glActiveTexture(GL_TEXTURE0 + 2); //texture are we binding to
		glGenTextures(1, &glData[5]);
		glBindTexture(GL_TEXTURE_2D, glData[5]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, specTex->width, specTex->height, 0, GL_RGB, GL_UNSIGNED_BYTE, specTex->data);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

		//		glActiveTexture(GL_TEXTURE0 + 1);
		//		int normalLoc = glGetUniformLocation(m_program, "Normal"); //get normal location
		//		glUniform1i(normalLoc, 1); //set to the diffuse to the texture index			

		mesh->m_userData = glData;
	}
}


void APP_PhysicallyBased::ClearMenu()
{
	TwDeleteBar(m_bar); //reset the gui
}

void APP_PhysicallyBased::CreateGui()
{
	m_bar = TwNewBar("IntroToOpenGL");

	TwDefine(" IntroToOpenGL position='10 10' "); // move bar to position (10, 10)
	TwDefine(" IntroToOpenGL size='430 320' "); // resize bar	
	TwDefine(" IntroToOpenGL color='128 128 128' alpha=32 ");   // semi-transparent blue bar
	TwDefine(" IntroToOpenGL resizable=false "); // mybar cannot be resized


	TwAddButton(m_bar, "label_01", NULL, NULL, "label='simply opengl scene, nothing much happening'"); //show as label		
	TwAddButton(m_bar, "mainMenu", Callback, this, "label='main menu'"); //show as button	

	//resets the camera when app re-opens
	GameCam->SetPosition(vec3(3, 2, 1.7));
	GameCam->SetFront(vec3(-0.89, -.0357, -.455));
	GameCam->SetPitchYaw(-2, -153);

	GameCam->SetMouseSnapToCurrent();
	GameCam->ManualSnap();
}

void APP_PhysicallyBased::cleanupOpenGLBuffers(FBXFile* fbx) {
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



bool APP_PhysicallyBased::Shutdown()
{
	delete GameCam;
	Gizmos::destroy();

	//delete m_fbx;

	cleanupOpenGLBuffers(m_fbx);
	glDeleteProgram(m_program);

	return true; //not being used in this lesson
}