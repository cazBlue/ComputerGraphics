#include <APP_Animation.h>
#include <vector>


//#define STB_IMAGE_IMPLEMENTATION //declared in fbx loader
#include <stb_image.h>
#include <FBXFile.h>

#include <iostream>
#include <fstream>

using namespace std;

APP_Animation::APP_Animation()
{

}
APP_Animation::~APP_Animation()
{

}

void APP_Animation::Update(float a_dt)
{
	GameCam->Update(a_dt); //update camera

//	if (m_timer > 5)
//		m_timer = 0;
	m_timer += a_dt;

	// grab the skeleton and animation we want to use
	FBXSkeleton* skeleton = m_fbx->getSkeletonByIndex(0);
	FBXAnimation* animation = m_fbx->getAnimationByIndex(0);
	// evaluate the animation to update bones
	skeleton->evaluate(animation, m_timer);
	for (unsigned int bone_index = 0; bone_index < skeleton->m_boneCount; ++bone_index)
	{
		skeleton->m_nodes[bone_index]->updateGlobalTransform();
	}
}

void APP_Animation::Draw()
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
	glm::vec3 dirLight = glm::vec3(0, 1, -10); //controls the lights position in the world	
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
	GLfloat specPow = 512; //controls the lights position in the world		
	glUniform1f(specPosUniform, specPow);	//set the lightDir uniform variabe in the vertex shader

	// grab the skeleton and animation we want to use
	FBXSkeleton* skeleton = m_fbx->getSkeletonByIndex(0);
	skeleton->updateBones();
	int bones_location = glGetUniformLocation(m_program, "bones");
	glUniformMatrix4fv(bones_location, skeleton->m_boneCount, GL_FALSE, (float*)skeleton->m_bones);

	int global_location = glGetUniformLocation(m_program, "Global"); //apply a global scale
	glUniformMatrix4fv(global_location, 1, GL_FALSE, glm::value_ptr(glm::scale(glm::vec3(.005, .005, .005))));

	// bind our vertex array object and draw the mesh
	unsigned int meshCount = m_fbx->getMeshCount();
	for (unsigned int i = 0; i < meshCount; ++i) {
		FBXMeshNode* mesh = m_fbx->getMeshByIndex(i);
		unsigned int* glData = (unsigned int*)mesh->m_userData;
		glBindVertexArray(glData[0]);

		//set diffuse texture	
		glActiveTexture(GL_TEXTURE0); //set for initial active texture		
		glBindTexture(GL_TEXTURE_2D, glData[3]);	//bind the diffuse texture
		int difLoc = glGetUniformLocation(m_program, "Diffuse"); //get diffuse location
		glUniform1i(difLoc, 0); //set to the diffuse to the texture index	

		glDrawElements(GL_TRIANGLES,
			(unsigned int)mesh->m_indices.size(), GL_UNSIGNED_INT, 0);
	}
}

void APP_Animation::loadImg(FBXTexture* a_tex)
{
	a_tex->data = stbi_load(a_tex->path.c_str(), &a_tex->width, 
		&a_tex->height, &a_tex->format, STBI_rgb); //request no alpha - works with PNG's

	//stbi_image_free(a_tex->data); //unload the image data
}

std::string APP_Animation::LoadShader(const char *a_filePath)
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

bool APP_Animation::Start()
{
	m_appName = "animation from FBX";

	Gizmos::create();
	GameCam = new Camera();

	m_timer = 0;
	//FBXFile* fbxFile = new FBXFile();

	m_fbx = new FBXFile();

	//fbxFile->unload();


	std::string strShaderCode; //file info holder --TODO create array of file names
	//open shader file
	std::ifstream shaderStream("./assets/animFbxToLoad.txt");
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
	//bool didLoad = m_fbx->load(path, m_fbx->UNITS_MILLIMETER, true, true, true);
	//if (didLoad)
	//	printf("loaded");
	//else
	//	printf("no load");

	m_fbx = importCtrl->m_FBX_anim;

	createOpenGLBuffers(m_fbx);


	//move the camera for bigger models....
//	GameCam->m_moveSpeed = 1000;
	//GameCam->SetPosition(glm::vec3(-500, , -500));
	//GameCam->SetPerspective(glm::pi<float>() * 0.25f, 16 / 9.f, 0.1f, 1000.f);
	//////////////create shaders and program	
	const char* vsSource = nullptr;
	std::string vsResult = LoadShader("./assets/shaders/AnimVertexShader.glsl");
	vsSource = vsResult.c_str();

	const char* fsSource = nullptr;
	std::string fsResult = LoadShader("./assets/shaders/AnimFragShader.glsl");
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

	isLoaded = true;

	return true; //not being used in this lesson
}


void APP_Animation::ClearMenu()
{
	TwDeleteBar(m_bar); //reset the gui
}

void APP_Animation::CreateGui()
{
	m_bar = TwNewBar("FBXAnimation");

	TwDefine(" FBXAnimation position='10 10' "); // move bar to position (10, 10)
	TwDefine(" FBXAnimation size='430 320' "); // resize bar	
	TwDefine(" FBXAnimation color='128 128 128' alpha=32 ");   // semi-transparent blue bar
	TwDefine(" FBXAnimation resizable=false "); // mybar cannot be resized


	TwAddButton(m_bar, "label_01", NULL, NULL, "label='animation from loaded FBX'"); //show as label		
	TwAddButton(m_bar, "mainMenu", Callback, this, "label='main menu'"); //show as button	

	//resets the camera when app re-opens
	GameCam->SetPosition(vec3(11.45, 8, 12.55));
	GameCam->SetFront(vec3(-.85, -.214, -.477));
	GameCam->SetPitchYaw(-14, -150);

	GameCam->SetMouseSnapToCurrent();
	GameCam->ManualSnap();
}

void APP_Animation::createOpenGLBuffers(FBXFile* fbx)
{
	unsigned int meshCount = fbx->getMeshCount();
	
	// create the GL VAO/VBO/IBO data for each mesh	
	for (unsigned int i = 0; i < meshCount; ++i)
	{
		FBXMeshNode* mesh = fbx->getMeshByIndex(i);

		FBXMaterial* mat = mesh->m_material;

		FBXTexture* diffuesTex = mat->textures[FBXMaterial::TextureTypes::DiffuseTexture];

		// storage for the opengl data in 4 unsigned int (includes diffuse texID)
		unsigned int* glData = new unsigned int[4];


		glGenVertexArrays(1, &glData[0]);
		glBindVertexArray(glData[0]);
		glGenBuffers(1, &glData[1]);
		glGenBuffers(1, &glData[2]);
		glBindBuffer(GL_ARRAY_BUFFER, glData[1]);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, glData[2]);
		glBufferData(GL_ARRAY_BUFFER, mesh->m_vertices.size() * sizeof(FBXVertex), mesh->m_vertices.data(), GL_STATIC_DRAW);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh->m_indices.size() * sizeof(unsigned int), mesh->m_indices.data(), GL_STATIC_DRAW);
//		glEnableVertexAttribArray(0); // position
//		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(FBXVertex), 0);
//		glEnableVertexAttribArray(1); // normal
//		glVertexAttribPointer(1, 4, GL_FLOAT, GL_TRUE, sizeof(FBXVertex), ((char*)0) + FBXVertex::NormalOffset);
//		glEnableVertexAttribArray(2); // colour / tex cords
//		glVertexAttribPointer(2, 2, GL_FLOAT, GL_TRUE, sizeof(FBXVertex), ((char*)0) + FBXVertex::TexCoord1Offset);
		//enable attrib pointers
		glEnableVertexAttribArray(0); //position
		glEnableVertexAttribArray(1); //normals
		glEnableVertexAttribArray(2); //texcoords
		glEnableVertexAttribArray(3); //tangents
		glEnableVertexAttribArray(4); //weights
		glEnableVertexAttribArray(5); //indices

		//assign info positions
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(FBXVertex),
			(void*)FBXVertex::PositionOffset);
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_TRUE, sizeof(FBXVertex),
			(void*)FBXVertex::NormalOffset);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_TRUE, sizeof(FBXVertex),
			(void*)FBXVertex::TexCoord1Offset);
		glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(FBXVertex),
			(void*)FBXVertex::TangentOffset);
		glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(FBXVertex),
			(void*)FBXVertex::WeightsOffset);
		glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(FBXVertex),
			(void*)FBXVertex::IndicesOffset);
		
		//we are done, unbind
		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		loadImg(diffuesTex); //issues found with png, removing alpha manually as temp work around

		//gen texture id and bind texture to buffer 
		glGenTextures(1, &glData[3]);
		glBindTexture(GL_TEXTURE_2D, glData[3]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, diffuesTex->width, diffuesTex->height, 0, GL_RGB, GL_UNSIGNED_BYTE, diffuesTex->data);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

		mesh->m_userData = glData;
	}
}


void APP_Animation::cleanupOpenGLBuffers(FBXFile* fbx) {
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



bool APP_Animation::Shutdown()
{
	delete GameCam;
	Gizmos::destroy();

//	delete m_fbx;

//	cleanupOpenGLBuffers(m_fbx);
	glDeleteProgram(m_program);

	return true; //not being used in this lesson
}