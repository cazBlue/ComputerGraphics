#include <APP_DeferredRendering.h>

#include <fstream>
#include <string>
#include <iostream>

#include <stb_image.h>
#include <FBXFile.h>

APP_DeferredRendering::APP_DeferredRendering()
{

}
APP_DeferredRendering::~APP_DeferredRendering()
{
	
}

void APP_DeferredRendering::Update(float a_dt)
{
	GameCam->Update(a_dt); //update camera
}

void APP_DeferredRendering::Draw()
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



	// G-Pass: render out the albedo, position and normal
	glEnable(GL_DEPTH_TEST);
	glBindFramebuffer(GL_FRAMEBUFFER, m_gpassFBO);
	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(m_programGBuffer);
	// bind camera transforms
	int loc = glGetUniformLocation(m_programGBuffer, "ProjectionView");
	glUniformMatrix4fv(loc, 1, GL_FALSE, &(GameCam->GetProjectionView()[0][0]));
	loc = glGetUniformLocation(m_programGBuffer, "View");
	glUniformMatrix4fv(loc, 1, GL_FALSE, &(GameCam->GetView()[0][0]));
	// draw our scene, in this example just the Stanford Bunny
	for (unsigned int i = 0; i < m_fbx->getMeshCount(); ++i) {
		FBXMeshNode* mesh = m_fbx->getMeshByIndex(i);
		unsigned int* glData = (unsigned int*)mesh->m_userData;
		glBindVertexArray(glData[0]);
		glDrawElements(GL_TRIANGLES, (unsigned int)mesh->m_indices.size(), GL_UNSIGNED_INT, 0);
	}


	// Light Pass: render lights as geometry, sampling position and
	// normals disable depth testing and enable additive blending
	glBindFramebuffer(GL_FRAMEBUFFER, m_lightFBO);
	glClear(GL_COLOR_BUFFER_BIT);
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE);
	glUseProgram(m_programDirectionalLight);
	//	loc = glGetUniformLocation(m_programDirectionalLight, "positionTexture");
	//	glUniform1i(loc, 0);
	//	glActiveTexture(GL_TEXTURE0);
	//	glBindTexture(GL_TEXTURE_2D, m_positionTexture);
	loc = glGetUniformLocation(m_programDirectionalLight, "normalTexture");
	glUniform1i(loc, 2);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, m_normalTexture);
	// draw lights as fullscreen quads
	drawDirectionalLight(glm::vec3(-1), glm::vec3(1));

	//draw point lights	
	
	glUseProgram(m_pointLightShader);
	loc = glGetUniformLocation(m_pointLightShader, "ProjectionView");
	glUniformMatrix4fv(loc, 1, GL_FALSE, &(GameCam->GetProjectionView()[0][0]));
	loc = glGetUniformLocation(m_pointLightShader, "positionTexture");
	glUniform1i(loc, 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_positionTexture);
	loc = glGetUniformLocation(m_pointLightShader, "normalTexture");
	glUniform1i(loc, 1);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, m_normalTexture);

	glCullFace(GL_FRONT);

	float t = (float)glfwGetTime();
	drawPointLight(glm::vec3(sinf(t) * 5, 3, cosf(t) * 5), 5, glm::vec3(1, 0, 0));
	drawPointLight(glm::vec3(sinf(t) * -5, 3, cosf(t) * -5), 5, glm::vec3(0, 1, 0));

	glCullFace(GL_BACK);

	//end point lights

	glDisable(GL_BLEND);

	// Composite Pass: render a quad and combine albedo and light
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_COLOR_BUFFER_BIT);
	glUseProgram(m_programComposite);
	loc = glGetUniformLocation(m_programComposite, "albedoTexture");
	glUniform1i(loc, 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_albedoTexture);
	loc = glGetUniformLocation(m_programComposite, "lightTexture");
	glUniform1i(loc, 1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, m_lightTexture);
	glBindVertexArray(m_fullScreenQuad_vao);
	glDrawArrays(GL_TRIANGLES, 0, 6);
}

void APP_DeferredRendering::drawPointLight(const glm::vec3& position, float radius,
											const glm::vec3& diffuse) 
{
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, m_lightTexture); //bind light texture for adding to

	glm::vec4 viewSpacePosition = GameCam->GetView() * glm::vec4(position, 1);
	int loc = glGetUniformLocation(m_pointLightShader, "lightPosition"); 
	glUniform3fv(loc, 1, &position[0]);
	loc = glGetUniformLocation(m_pointLightShader, "lightPositionView");
	glUniform3fv(loc, 1, &viewSpacePosition[0]);
	loc = glGetUniformLocation(m_pointLightShader, "lightRadius");
	glUniform1f(loc, radius);
	loc = glGetUniformLocation(m_pointLightShader, "lightDiffuse");
	glUniform3fv(loc, 1, &diffuse[0]);
	glBindVertexArray(m_cubeVAO);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, nullptr);
}

void APP_DeferredRendering::drawDirectionalLight(const glm::vec3& direction, const glm::vec3& diffuse) {
	glm::vec4 viewSpaceLight = GameCam->GetView() * glm::vec4(glm::normalize(direction), 0);
	
	int loc = glGetUniformLocation(m_programDirectionalLight, "lightDirection");
	glUniform3fv(loc, 1, &viewSpaceLight[0]);
	
	loc = glGetUniformLocation(m_programDirectionalLight, "lightDiffuse");
	
	glUniform3fv(loc, 1, &diffuse[0]);
	glBindVertexArray(m_fullScreenQuad_vao);
	glDrawArrays(GL_TRIANGLES, 0, 6);
}

void APP_DeferredRendering::ClearMenu()
{
	TwDeleteBar(m_bar); //reset the gui
}

void APP_DeferredRendering::CreateGui()
{
	m_bar = TwNewBar("DeferredRendering");

	TwDefine(" DeferredRendering position='10 10' "); // move bar to position (10, 10)
	TwDefine(" DeferredRendering size='430 320' "); // resize bar	
	TwDefine(" DeferredRendering color='128 128 128' alpha=32 ");   // semi-transparent blue bar
	TwDefine(" DeferredRendering resizable=false "); // mybar cannot be resized


	TwAddButton(m_bar, "label_01", NULL, NULL, "label='deferred rendering'"); //show as label		
	TwAddButton(m_bar, "mainMenu", Callback, this, "label='main menu'"); //show as button				

	//resets the camera when app re-opens
	GameCam->SetPosition(vec3(7.4, 8.3, 17));
	GameCam->SetFront(vec3(-0.4268, -.241, -.87));
	GameCam->SetPitchYaw(-14, -116);

	GameCam->SetMouseSnapToCurrent();
	GameCam->ManualSnap();
}


bool APP_DeferredRendering::Start()
{
	m_appName = "Deferred Rendering";

	Gizmos::create();

	GameCam = new Camera();

	GameCam->SetPosition(vec3(7.4, 8.3, 17));
	GameCam->SetFront(vec3(-0.4268, -.241, -.87));
	GameCam->SetPitchYaw(-14, -116);

	m_fbx = importCtrl->m_FBX_bunny;

	createGpassBuffer();

	createLightBuffer();

	createFullSreenQuad();

	createScene();

	createBoundingCube();

	createCompositeBuffer();

	isLoaded = true;

	return true; //not being used in this lesson
}


void APP_DeferredRendering::createBoundingCube()
{
	float cubeVertexData[] = {
		-1, -1, 1, 1,
		1, -1, 1, 1,
		1, -1, -1, 1,
		-1, -1, -1, 1,
		-1, 1, 1, 1,
		1, 1, 1, 1,
		1, 1, -1, 1,
		-1, 1, -1, 1,
	};
	unsigned int cubeIndexData[] = {
		0, 5, 4,
		0, 1, 5,
		1, 6, 5,
		1, 2, 6,
		2, 7, 6,
		2, 3, 7,
		3, 4, 7,
		3, 0, 4,
		4, 6, 7,
		4, 5, 6,
		3, 1, 0,
		3, 2, 1
	};


	//create buffers
	glGenVertexArrays(1, &m_cubeVAO);	
	glBindVertexArray(m_cubeVAO);

	glGenBuffers(1, &m_cubeIBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_cubeIBO);

	glGenBuffers(1, &m_cubeVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_cubeVBO);

	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 32, cubeVertexData, GL_STATIC_DRAW);
	
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 36 * sizeof(unsigned int), cubeIndexData, GL_STATIC_DRAW);


	glEnableVertexAttribArray(0); // position
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(float)* 4, 0);
	
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);


	//create cube shader program
	const char* vsSource = nullptr;
	std::string vsResult = LoadShader("./assets/shaders/deferredRendering/pointLightVertShader.vert");
	vsSource = vsResult.c_str();

	const char* fsSource = nullptr;
	std::string fsResult = LoadShader("./assets/shaders/deferredRendering/pointLightFragShader.frag");
	fsSource = fsResult.c_str();

	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

	glShaderSource(vertexShader, 1, (const char**)&vsSource, 0);
	glCompileShader(vertexShader);
	glShaderSource(fragmentShader, 1, (const char**)&fsSource, 0);
	glCompileShader(fragmentShader);

	m_pointLightShader = glCreateProgram();
	glAttachShader(m_pointLightShader, vertexShader);
	glAttachShader(m_pointLightShader, fragmentShader);
	glLinkProgram(m_pointLightShader);
}


bool APP_DeferredRendering::Shutdown()
{
	delete GameCam;
//	cleanupOpenGLBuffers(m_fbx);
//	delete m_fbx;
	Gizmos::destroy();

	return true; //not being used in this lesson
}

void APP_DeferredRendering::createGpassBuffer()
{
	// setup gpass framebuffer
	glGenFramebuffers(1, &m_gpassFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, m_gpassFBO);
	glGenTextures(1, &m_albedoTexture);
	glBindTexture(GL_TEXTURE_2D, m_albedoTexture);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB8, 1280, 720);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glGenTextures(1, &m_positionTexture);
	glBindTexture(GL_TEXTURE_2D, m_positionTexture);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB32F, 1280, 720);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glGenTextures(1, &m_normalTexture);
	glBindTexture(GL_TEXTURE_2D, m_normalTexture);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB32F, 1280, 720);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glGenRenderbuffers(1, &m_gpassDepth);
	glBindRenderbuffer(GL_RENDERBUFFER, m_gpassDepth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, 1280, 720);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_albedoTexture, 0);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, m_positionTexture, 0);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, m_normalTexture, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_gpassDepth);
	
	GLenum gpassTargets[] = { GL_COLOR_ATTACHMENT0,
		GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
	
	glDrawBuffers(3, gpassTargets);
	
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void APP_DeferredRendering::createLightBuffer()
{
	// setup light framebuffer
	glGenFramebuffers(1, &m_lightFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, m_lightFBO);
	glGenTextures(1, &m_lightTexture);
	glBindTexture(GL_TEXTURE_2D, m_lightTexture);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB8, 1280, 720);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_lightTexture, 0);
	GLenum lightTargets[] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, lightTargets);
	int status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE)
		printf("Framebuffer Error!\n");
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//create light shaders
	const char* vsSource = nullptr;
	std::string vsResult = LoadShader("./assets/shaders/deferredRendering/directionalLightVertShader.vert");
	vsSource = vsResult.c_str();

	const char* fsSource = nullptr;
	std::string fsResult = LoadShader("./assets/shaders/deferredRendering/directionalLightFragShader.frag");
	fsSource = fsResult.c_str();

	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

	glShaderSource(vertexShader, 1, (const char**)&vsSource, 0);
	glCompileShader(vertexShader);
	glShaderSource(fragmentShader, 1, (const char**)&fsSource, 0);
	glCompileShader(fragmentShader);

	m_programDirectionalLight = glCreateProgram();
	glAttachShader(m_programDirectionalLight, vertexShader);
	glAttachShader(m_programDirectionalLight, fragmentShader);
	glLinkProgram(m_programDirectionalLight);
}


void APP_DeferredRendering::createCompositeBuffer()
{
	//create composite shaders
	const char* vsSource = nullptr;
	std::string vsResult = LoadShader("./assets/shaders/deferredRendering/compositeVertShader.vert");
	vsSource = vsResult.c_str();

	const char* fsSource = nullptr;
	std::string fsResult = LoadShader("./assets/shaders/deferredRendering/compositeFragShader.frag");
	fsSource = fsResult.c_str();

	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

	glShaderSource(vertexShader, 1, (const char**)&vsSource, 0);
	glCompileShader(vertexShader);
	glShaderSource(fragmentShader, 1, (const char**)&fsSource, 0);
	glCompileShader(fragmentShader);

	m_programComposite = glCreateProgram();
	glAttachShader(m_programComposite, vertexShader);
	glAttachShader(m_programComposite, fragmentShader);
	glLinkProgram(m_programComposite);
}


void APP_DeferredRendering::createFullSreenQuad()
{
	// fullscreen quad
	glm::vec2 halfTexel = 1.0f / glm::vec2(1280, 720) * 0.5f; //half texel

	float vertexData[] = {
		-1, -1, 0, 1, halfTexel.x, halfTexel.y,
		1, 1, 0, 1, 1 - halfTexel.x, 1 - halfTexel.y,
		-1, 1, 0, 1, halfTexel.x, 1 - halfTexel.y,
		-1, -1, 0, 1, halfTexel.x, halfTexel.y,
		1, -1, 0, 1, 1 - halfTexel.x, halfTexel.y,
		1, 1, 0, 1, 1 - halfTexel.x, 1 - halfTexel.y,
	};
	glGenVertexArrays(1, &m_fullScreenQuad_vao);
	glBindVertexArray(m_fullScreenQuad_vao);
	glGenBuffers(1, &m_fullScreenQuad_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, m_fullScreenQuad_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float)* 6 * 6, vertexData, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(float)* 6, 0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float)* 6, ((char*)0) + 16);
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

}

std::string APP_DeferredRendering::LoadShader(const char *a_filePath)
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

void APP_DeferredRendering::createScene()
{
	//load shaders
	// create shaders
	const char* vsSource = nullptr;
	std::string vsResult = LoadShader("./assets/shaders/deferredRendering/gBufferVertShader.vert");
	vsSource = vsResult.c_str();

	const char* fsSource = nullptr;
	std::string fsResult = LoadShader("./assets/shaders/deferredRendering/gBufferFragShader.frag");
	fsSource = fsResult.c_str();

	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

	glShaderSource(vertexShader, 1, (const char**)&vsSource, 0);
	glCompileShader(vertexShader);
	glShaderSource(fragmentShader, 1, (const char**)&fsSource, 0);
	glCompileShader(fragmentShader);

	m_programGBuffer = glCreateProgram();
	glAttachShader(m_programGBuffer, vertexShader);
	glAttachShader(m_programGBuffer, fragmentShader);
	glLinkProgram(m_programGBuffer);



//	createOpenGLBuffers(m_fbx);
}


void APP_DeferredRendering::createOpenGLBuffers(FBXFile* fbx)
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
//		glEnableVertexAttribArray(2); // colour / tex cords
//		glVertexAttribPointer(2, 2, GL_FLOAT, GL_TRUE, sizeof(FBXVertex), ((char*)0) + FBXVertex::TexCoord1Offset);
//		glEnableVertexAttribArray(3); // tangent coords / tex cords
//		glVertexAttribPointer(3, 4, GL_FLOAT, GL_TRUE, sizeof(FBXVertex), ((char*)0) + FBXVertex::TangentOffset);
		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);


		//stanford bunny has no textures!
		//gen diffuse texture id and bind texture to buffer 
//		glActiveTexture(GL_TEXTURE0); //texture are we binding to
//		glGenTextures(1, &glData[3]);
//		glBindTexture(GL_TEXTURE_2D, glData[3]);
//		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, diffuesTex->width, diffuesTex->height, 0, GL_RGB, GL_UNSIGNED_BYTE, diffuesTex->data);
//		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//
//		//		int difLoc = glGetUniformLocation(m_program, "Diffuse"); //get diffuse location
//		//		glUniform1i(difLoc, 0); //set to the diffuse to the texture index	
//
//		//gen normal texture id and bind texture to buffer 
//		//		glActiveTexture(GL_TEXTURE0 + 1);
//		glActiveTexture(GL_TEXTURE0 + 1); //texture are we binding to
//		glGenTextures(1, &glData[4]);
//		glBindTexture(GL_TEXTURE_2D, glData[4]);
//		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, normalTex->width, normalTex->height, 0, GL_RGB, GL_UNSIGNED_BYTE, normalTex->data);
//		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

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

void APP_DeferredRendering::cleanupOpenGLBuffers(FBXFile* fbx) {
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