#include <APP_PostProcessing.h>

#include <fstream>
#include <string>
#include <iostream>

APP_postProcess::APP_postProcess()
{
	
}

APP_postProcess::~APP_postProcess()
{

}

void APP_postProcess::Update(float a_dt)
{
	GameCam->Update(a_dt); //update camera

}

void APP_postProcess::Draw()
{
	Gizmos::clear();
//	Gizmos::addTransform(glm::mat4(1));

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


	// bind our target
	glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
	glViewport(0, 0, 1280, 720);
	// clear the target
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// draw our 3D scene
	// gizmos for now

//	Gizmos::addSphere(vec3(0,0,0), 1.0f, 8, 8, vec4(1,0,0,1));
//	Gizmos::addSphere(vec3(0, 2, 0), 0.5f, 8, 8, vec4(1, 1, 0, 1));
//	Gizmos::addSphere(vec3(0, 6, 0), 1.5f, 8, 8, vec4(0, 0, 1, 1));
//
//	Gizmos::draw(GameCam->GetProjectionView());

	glUseProgram(m_programTarget);
	int view_proj_uniform = glGetUniformLocation(m_programTarget, "ProjectionView");
	glUniformMatrix4fv(view_proj_uniform, 1, GL_FALSE, glm::value_ptr(GameCam->GetProjectionView()));
	for (unsigned int i = 0; i < m_gl_info.size(); ++i)
	{
		glBindVertexArray(m_gl_info[i].m_VAO);
		glDrawElements(GL_TRIANGLES, m_gl_info[i].m_index_count, GL_UNSIGNED_INT, 0);
	}

//	Gizmos::draw(GameCam->GetProjectionView());
	// bind the back-buffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, 1280, 720);
	// just clear the back-buffer depth as
	// each pixel will be filled
	glClear(GL_DEPTH_BUFFER_BIT);
	// draw out full-screen quad
	glUseProgram(m_programBackBuffer);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_fboTexture);
	int loc = glGetUniformLocation(m_programBackBuffer, "target");
	glUniform1i(loc, 0);

//	int view_proj_uniform = glGetUniformLocation(m_program, "ProjectionView");
//	glUniformMatrix4fv(view_proj_uniform, 1, GL_FALSE, glm::value_ptr(GameCam->GetProjectionView()));

	glBindVertexArray(m_vao);
	glDrawArrays(GL_TRIANGLES, 0, 6);		



//	Gizmos::draw(GameCam->GetProjectionView());
//	
}


void APP_postProcess::ClearMenu()
{
	TwDeleteBar(m_bar); //reset the gui
}

void APP_postProcess::CreateGui()
{
	m_bar = TwNewBar("PostProcess");

	TwDefine(" PostProcess position='10 10' "); // move bar to position (10, 10)
	TwDefine(" PostProcess size='430 320' "); // resize bar	
	TwDefine(" PostProcess color='128 128 128' alpha=32 ");   // semi-transparent blue bar
	TwDefine(" PostProcess resizable=false "); // mybar cannot be resized


	TwAddButton(m_bar, "label_01", NULL, NULL, "label='Post processing effects, including Sobel operator'"); //show as label		
	TwAddButton(m_bar, "mainMenu", Callback, this, "label='main menu'"); //show as button				
}

bool APP_postProcess::Start()
{
	m_appName = "Post Processing";

	Gizmos::create();

	GameCam = new Camera();

	CreateFrameBuffer();
	CreateFullSreenQuad();

	createTargetBuffers();
	createBackBufferBuffers();

	isLoaded = true;
	
	return true; //not being used in this lesson
}

void APP_postProcess::createBackBufferBuffers()
{
	//load shaders
	// create shaders
	const char* vsSource = nullptr;
	std::string vsResult = LoadShader("./assets/shaders/PostProcessWorldVertShader.vert");
	vsSource = vsResult.c_str();

	const char* fsSource = nullptr;
	std::string fsResult = LoadShader("./assets/shaders/PostProcessFragShader.frag");
	fsSource = fsResult.c_str();

	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

	glShaderSource(vertexShader, 1, (const char**)&vsSource, 0);
	glCompileShader(vertexShader);
	glShaderSource(fragmentShader, 1, (const char**)&fsSource, 0);
	glCompileShader(fragmentShader);

	m_programBackBuffer = glCreateProgram();
	glAttachShader(m_programBackBuffer, vertexShader);
	glAttachShader(m_programBackBuffer, fragmentShader);
	glLinkProgram(m_programBackBuffer);
}

void APP_postProcess::createTargetBuffers()
{

	//load shaders
	// create shaders
	const char* vsSource = nullptr;
	std::string vsResult = LoadShader("./assets/shaders/PostProcessObjVertShader.vert");
	vsSource = vsResult.c_str();

	const char* fsSource = nullptr;
	std::string fsResult = LoadShader("./assets/shaders/PostProcessFragShader.frag");
	fsSource = fsResult.c_str();

	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

	glShaderSource(vertexShader, 1, (const char**)&vsSource, 0);
	glCompileShader(vertexShader);
	glShaderSource(fragmentShader, 1, (const char**)&fsSource, 0);
	glCompileShader(fragmentShader);

	m_programTarget = glCreateProgram();
	glAttachShader(m_programTarget, vertexShader);
	glAttachShader(m_programTarget, fragmentShader);
	glLinkProgram(m_programTarget);


	printf("loading object, this can take a while!");
//	std::string err = tinyobj::LoadObj(shapes, materials, "./assets/stanford_objs/bunny.obj");
	//loads obj from file and creates a binary version, loads from that if it exists
	//objCtrl = new OBJIO();
	//objCtrl->ReadObj("./assets/stanford_objs/bunny.obj", "OBJbunny.dat", &shapes);
	


	//createOpenGLBuffers(shapes);
}

bool APP_postProcess::Shutdown()
{
	delete GameCam;
	Gizmos::destroy();

	delete objCtrl;

	return true; //not being used in this lesson
}

void APP_postProcess::CreateFrameBuffer()
{
	glUseProgram(m_programTarget);
	// setup framebuffer
	glGenFramebuffers(1, &m_fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
	glGenTextures(1, &m_fboTexture);
	glBindTexture(GL_TEXTURE_2D, m_fboTexture);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, 1280, 720);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_fboTexture, 0);
	glGenRenderbuffers(1, &m_fboDepth);
	glBindRenderbuffer(GL_RENDERBUFFER, m_fboDepth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 1280, 720);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_fboDepth);
	GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, drawBuffers);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glUseProgram(0);
}

void APP_postProcess::CreateFullSreenQuad()
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
	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);
	glGenBuffers(1, &m_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float)* 6 * 6, vertexData, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(float)* 6, 0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float)* 6, ((char*)0) + 16);
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

}

std::string APP_postProcess::LoadShader(const char *a_filePath)
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

void APP_postProcess::createOpenGLBuffers(std::vector<tinyobj::shape_t>& shapes)
{
	m_gl_info.resize(shapes.size());

	for (unsigned int mesh_index = 0; mesh_index < shapes.size(); ++mesh_index)
	{
		glGenVertexArrays(1, &m_gl_info[mesh_index].m_VAO);
		glGenBuffers(1, &m_gl_info[mesh_index].m_VBO);
		glGenBuffers(1, &m_gl_info[mesh_index].m_IBO);
		glBindVertexArray(m_gl_info[mesh_index].m_VAO);
		unsigned int float_count = shapes[mesh_index].mesh.positions.size();
		float_count += shapes[mesh_index].mesh.normals.size();
		float_count += shapes[mesh_index].mesh.texcoords.size();
		std::vector<float> vertex_data;
		vertex_data.reserve(float_count);
		vertex_data.insert(vertex_data.end(), shapes[mesh_index].mesh.positions.begin(), shapes[mesh_index].mesh.positions.end());
		vertex_data.insert(vertex_data.end(), shapes[mesh_index].mesh.normals.begin(), shapes[mesh_index].mesh.normals.end());
		m_gl_info[mesh_index].m_index_count = shapes[mesh_index].mesh.indices.size();
		glBindBuffer(GL_ARRAY_BUFFER, m_gl_info[mesh_index].m_VBO);
		glBufferData(GL_ARRAY_BUFFER, vertex_data.size() * sizeof(float), vertex_data.data(), GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_gl_info[mesh_index].m_IBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, shapes[mesh_index].mesh.indices.size() * sizeof(unsigned int), shapes[mesh_index].mesh.indices.data(), GL_STATIC_DRAW);
		glEnableVertexAttribArray(0); //position
		glEnableVertexAttribArray(1); //normal data (colour information
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_TRUE, 0, (void*)(sizeof(float)*shapes[mesh_index].mesh.positions.size()));
		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
}