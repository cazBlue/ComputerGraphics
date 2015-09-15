#include <APP_Shadows.h>
#include <vector>


//#define STB_IMAGE_IMPLEMENTATION //declared in fbx loader
#include <stb_image.h>
#include <FBXFile.h>

#include <iostream>
#include <fstream>

using namespace std;

APP_Shadows::APP_Shadows()
{

}
APP_Shadows::~APP_Shadows()
{

}

void APP_Shadows::Update(float a_dt)
{
	GameCam->Update(a_dt); //update camera

	//animate the light
	m_time += a_dt;
	m_lightDirection = glm::normalize(glm::vec3(glm::sin(m_time) * 1, 2.5f, glm::cos(m_time) * 1));
	glm::mat4 lightProjection = glm::ortho<float>(-10, 10, -10, 10, -10, 10);
	glm::mat4 lightView = glm::lookAt(m_lightDirection, glm::vec3(0), glm::vec3(0, 1, 0));

	m_lightMatrix = lightProjection * lightView;
}

void APP_Shadows::Draw()
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


	// shadow pass: bind our shadow map target and clear the depth
	glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
	glViewport(0, 0, 1024, 1024);
	glClear(GL_DEPTH_BUFFER_BIT);
	glUseProgram(m_shadowGenProgram);
	// bind the light matrix
	int loc = glGetUniformLocation(m_shadowGenProgram, "LightMatrix");
	glUniformMatrix4fv(loc, 1, GL_FALSE, &(m_lightMatrix[0][0]));
	// draw all shadow-casting geometry
	for (unsigned int i = 0; i < m_fbx->getMeshCount(); ++i) {
		FBXMeshNode* mesh = m_fbx->getMeshByIndex(i);
		unsigned int* glData = (unsigned int*)mesh->m_userData;
		glBindVertexArray(glData[0]);
		glDrawElements(GL_TRIANGLES, (unsigned int)mesh->m_indices.size(), GL_UNSIGNED_INT, 0);
	}


	glUseProgram(m_useShadowProgram);
	// bind the camera
	loc = glGetUniformLocation(m_useShadowProgram, "ProjectionView");
	glUniformMatrix4fv(loc, 1, GL_FALSE, &(GameCam->GetProjectionView()[0][0]));

	int lightDirUniform = -1;
	// bind the directional light position for a directional light
	lightDirUniform = glGetUniformLocation(m_useShadowProgram, "directionalLight");	//get the directional light uniform index from the vertex shader
//	glm::vec3 dirLight = glm::vec3(0, 1, 10); //controls the lights position in the world	
	glUniform3fv(lightDirUniform, 1, glm::value_ptr(m_lightDirection));	//set the lightDir uniform variabe in the vertex shader

//	generatePlane(); //draw the plane
//	glBindVertexArray(m_plane_vao);	
//	glBindBuffer(GL_ARRAY_BUFFER, m_plane_vbo);
//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_plane_ibo);
//	
//	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
//
//	// bind our vertex array object and draw the fbx mesh
//	for (unsigned int i = 0; i < m_fbx->getMeshCount(); ++i) {
//		FBXMeshNode* mesh = m_fbx->getMeshByIndex(i);
//		unsigned int* glData = (unsigned int*)mesh->m_userData;
//		glBindVertexArray(glData[0]);
//		glDrawElements(GL_TRIANGLES, (unsigned int)mesh->m_indices.size(), GL_UNSIGNED_INT, 0);
//	}

	// final pass: bind back-buffer and clear colour and depth
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, 1280, 720);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(m_useShadowProgram);
	// bind the camera
	loc = glGetUniformLocation(m_useShadowProgram, "ProjectionView");
	glUniformMatrix4fv(loc, 1, GL_FALSE, &(GameCam->GetProjectionView()[0][0]));
	// bind the light matrix
	glm::mat4 textureSpaceOffset(
		0.5f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.5f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.5f, 0.0f,
		0.5f, 0.5f, 0.5f, 1.0f
		);

	glm::mat4 lightMatrix = textureSpaceOffset * m_lightMatrix;
	loc = glGetUniformLocation(m_useShadowProgram, "LightMatrix");
	glUniformMatrix4fv(loc, 1, GL_FALSE, &lightMatrix[0][0]);
	
	loc = glGetUniformLocation(m_useShadowProgram, "lightDir");
	glUniform3fv(loc, 1, &m_lightDirection[0]);
	
	loc = glGetUniformLocation(m_useShadowProgram, "shadowMap");
	glUniform1i(loc, 0);
	glActiveTexture(GL_TEXTURE0);
	
//	glTexParameteri(GL_TEXTURE_2D, GL_COMPARE_REF_TO_TEXTURE, GL_NONE);
//	glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE, GL_INTENSITY);
	

//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_READ_COLOR);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_READ_COLOR);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
//	glTexParameteri(GL_TEXTURE_2D, GL_MAX_DEPTH_TEXTURE_SAMPLES, BACKGROUND_INTENSITY);


	glBindTexture(GL_TEXTURE_2D, m_fboDepth);
	// bind our vertex array object and draw the mesh
	for (unsigned int i = 0; i < m_fbx->getMeshCount(); ++i) {
		FBXMeshNode* mesh = m_fbx->getMeshByIndex(i);
		unsigned int* glData = (unsigned int*)mesh->m_userData;
		glBindVertexArray(glData[0]);
		glDrawElements(GL_TRIANGLES,
			(unsigned int)mesh->m_indices.size(),
			GL_UNSIGNED_INT, 0);
	}
	// draw a plane under the bunny
	glBindVertexArray(m_plane_vao);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

}

void APP_Shadows::loadImg(int* a_height, int* a_width, int* a_format, const char* a_path, unsigned int* a_id)
{
	unsigned char* data = stbi_load(a_path, a_width, a_height, a_format, STBI_rgb); //request no alpha

	glGenTextures(1, a_id);
	glBindTexture(GL_TEXTURE_2D, (*a_id));
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, (*a_width), (*a_height), 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	stbi_image_free(data); //unload the image data
}

std::string APP_Shadows::LoadShader(const char *a_filePath)
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

bool APP_Shadows::Start()
{
	Gizmos::create();
	GameCam = new Camera();	

	m_time = 0;

	m_fbx = new FBXFile();	

	std::string strShaderCode; //file info holder --TODO create array of file names
	//open shader file
	std::ifstream shaderStream("./assets/ShadowFBX.txt");
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

	createLightProgram();

	createShadowMapBuffers();

	m_lightDirection = glm::normalize(glm::vec3(1, 2.5f, 1));
	
	glm::mat4 lightProjection = glm::ortho<float>(-10, 10, -10, 10, -10, 10);
	glm::mat4 lightView = glm::lookAt(m_lightDirection, glm::vec3(0), glm::vec3(0, 1, 0));
	
	m_lightMatrix = lightProjection * lightView;

	createShadowGenProgram(); //requires lights to be bound first

	generatePlane();

	return true; //not being used in this lesson
}

void APP_Shadows::createShadowGenProgram()
{

	//////////////create shaders and program	
	const char* vsSource = nullptr;
	std::string vsResult = LoadShader("./assets/shaders/Shadow_shadowVertexShader.vert");
	vsSource = vsResult.c_str();

	const char* fsSource = nullptr;
	std::string fsResult = LoadShader("./assets/shaders/Shadow_shadowFragShader.frag");
	fsSource = fsResult.c_str();

	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, (const char**)&vsSource, 0);
	glCompileShader(vertexShader);
	unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, (const char**)&fsSource, 0);
	glCompileShader(fragmentShader);
	m_shadowGenProgram = glCreateProgram();
	glAttachShader(m_shadowGenProgram, vertexShader);
	glAttachShader(m_shadowGenProgram, fragmentShader);
	glLinkProgram(m_shadowGenProgram);



}

void APP_Shadows::createShadowMapBuffers()
{	
	// setup shadow map buffer
	glGenFramebuffers(1, &m_fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
	glGenTextures(1, &m_fboDepth);
	glBindTexture(GL_TEXTURE_2D, m_fboDepth);
	
	// texture uses a 16-bit depth component format
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, 1024, 1024,
		0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	
	// attached as a depth attachment to capture depth not colour
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, m_fboDepth, 0);
	
	// no colour targets are used
	glDrawBuffer(GL_NONE);

	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE)
		printf("Framebuffer Error!\n");
	
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void APP_Shadows::createLightProgram()
{
	//////////////create shaders and program	
	const char* vsSource = nullptr;
	std::string vsResult = LoadShader("./assets/shaders/ShadowVertexShader.vert");
	vsSource = vsResult.c_str();

	const char* fsSource = nullptr;
	std::string fsResult = LoadShader("./assets/shaders/ShadowFragShader.frag");
	fsSource = fsResult.c_str();

	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, (const char**)&vsSource, 0);
	glCompileShader(vertexShader);
	unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, (const char**)&fsSource, 0);
	glCompileShader(fragmentShader);
	m_useShadowProgram = glCreateProgram();
	glAttachShader(m_useShadowProgram, vertexShader);
	glAttachShader(m_useShadowProgram, fragmentShader);
	glLinkProgram(m_useShadowProgram);
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	// Generate our GL Buffers	for the plane
	glGenBuffers(1, &m_plane_vbo);
	glGenBuffers(1, &m_plane_ibo);
	glGenVertexArrays(1, &m_plane_vao); //generate a VertexArrayObject

}

struct Vertex {
	vec4 position;
	vec4 colour;
};

void APP_Shadows::generatePlane()
{
	Vertex* aoVertices = new Vertex[4];

	//generate mesh information
	aoVertices[0].position = glm::vec4(-15, 0, 15, 1);
	aoVertices[1].position = glm::vec4(15, 0, 15, 1);
	aoVertices[2].position = glm::vec4(15, 0, -15, 1);
	aoVertices[3].position = glm::vec4(-15, 0, -15, 1);

	aoVertices[0].colour = glm::vec4(1.0f, 1.0f, 1.0f, 1);
	aoVertices[1].colour = glm::vec4(1.0f, 1.0f, 1.0f, 1);
	aoVertices[2].colour = glm::vec4(1.0f, 1.0f, 1.0f, 1);
	aoVertices[3].colour = glm::vec4(1.0f, 1.0f, 1.0f, 1);

	unsigned int indexData[] = {
		0, 1, 2,
		0, 2, 3,
	};

	//bind buffers
	glBindVertexArray(m_plane_vao);						//bind vertex array object
	glBindBuffer(GL_ARRAY_BUFFER, m_plane_vbo);			//vertex buffer objet
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_plane_ibo);	//index buffer object
	//enable arrays for incoming data
	glEnableVertexAttribArray(0);					//enable vertex positions in the vertex shader
	glEnableVertexAttribArray(1);					//enable colour in the vertex shader

	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
//	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float)* 6, ((char*)0) + 16);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(vec4))); //colour of plane

	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * 4, aoVertices, GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int)* 6, indexData, GL_STATIC_DRAW);

	//	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); //enable wireframe render
//	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

	//unbind and delte pointers
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	delete[] aoVertices;
}

void APP_Shadows::createOpenGLBuffers(FBXFile* fbx)
{
	unsigned int meshCount = fbx->getMeshCount();
	unsigned int materialCount = fbx->getMaterialCount();
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
		glEnableVertexAttribArray(0); // position
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(FBXVertex), 0);
		glEnableVertexAttribArray(1); // normal
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_TRUE, sizeof(FBXVertex), ((char*)0) + FBXVertex::NormalOffset);
		glEnableVertexAttribArray(2); // colour / tex cords
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_TRUE, sizeof(FBXVertex), ((char*)0) + FBXVertex::TexCoord1Offset);
		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		//no textures on the standord bunny!
		//gen texture id and bind texture to buffer 
//		glGenTextures(1, &glData[3]);
//		glBindTexture(GL_TEXTURE_2D, glData[3]);
//		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, diffuesTex->width, diffuesTex->height, 0, GL_RGB, GL_UNSIGNED_BYTE, diffuesTex->data);
//		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

		mesh->m_userData = glData;
	}
}

void APP_Shadows::cleanupOpenGLBuffers(FBXFile* fbx) {
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

bool APP_Shadows::Shutdown()
{
	delete GameCam;
	Gizmos::destroy();

	delete m_fbx;

	cleanupOpenGLBuffers(m_fbx);
	glDeleteProgram(m_useShadowProgram);

	return true; //not being used in this lesson
}