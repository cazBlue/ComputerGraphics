#include <APP_RenderTargets.h>

APP_RenderTargets::APP_RenderTargets()
{

}
APP_RenderTargets::~APP_RenderTargets()
{

}

void APP_RenderTargets::Update(float a_dt)
{
	//GameCam->Update(a_dt); //update camera

	m_time += a_dt;

	m_rotateMat = glm::rotate(m_time, vec3(0, 1, 0));
}

mat4 APP_RenderTargets::getSphereMat(int x, int y, int z)
{
	return mat4(
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		x, y, z, 1);
}

void APP_RenderTargets::Draw()
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

	//create new camera
	Camera* securityCam;

	mat4 sph1Wolrd = glm::inverse(m_rotateMat) * getSphereMat(-5, 0, 5);
	vec3 sph1Vec3 = vec3(sph1Wolrd[3].x, cos(m_time) - 1, sph1Wolrd[3].z);

	mat4 sph2Wolrd = m_rotateMat * getSphereMat(5, 0, -5);
	vec3 sph2Vec3 = vec3(sph2Wolrd[3].x, cos(m_time), sph2Wolrd[3].z);

	// draw our meshes, or gizmos, to the render target 
	Gizmos::addSphere(vec3(5, sin(m_time), 5), 0.5f, 8, 8, vec4(1, 1, 0, 1));
	Gizmos::addSphere(vec3(-5, cos(m_time), -5), 0.5f, 8, 8, vec4(1, 0, 0, 1));
	Gizmos::addSphere(sph2Vec3, 0.5f, 8, 8, vec4(0, 1, 0, 1));
	Gizmos::addSphere(sph1Vec3, 0.5f, 8, 8, vec4(0, 0, 1, 1));

	for (unsigned int i = 0; i < 4; ++i)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, m_fbo[i]);
		//render full res viewport (matches screen res)
		//this will be paired down
		glViewport(0, 0, 512, 512); 
		glClearColor(0.75f, 0.75f, 0.75f, 1);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		securityCam = new Camera();
		switch (i)
		{
		case 0: 
			securityCam->SetLookAt(vec3(-10, 10, 10), vec3(0), vec3(0, 1, 0));
			break;
		case 1:
			securityCam->SetLookAt(vec3(10, 10, -10), vec3(0), vec3(0, 1, 0));
			break;
		case 2:
			securityCam->SetLookAt(vec3(10, 10, 10), vec3(0), vec3(0, 1, 0));
			break;
		case 3:
			securityCam->SetLookAt(vec3(-10, 10, -10), vec3(0), vec3(0, 1, 0));
			break;
		default:
			securityCam->SetLookAt(vec3(10, 10, 10), vec3(0), vec3(0, 1, 0));
			break;
		}
		
		Gizmos::draw(securityCam->GetProjectionView());		
	}
	delete securityCam;

	//revert to the back buffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0); 
	glViewport(0, 0, 1280, 720);
	glClearColor(0.25f, 0.25f, 0.25f, 1); 
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(m_program);
	int loc = glGetUniformLocation(m_program, "ProjectionView"); 
	glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(GameCam->GetProjectionView()));

	for (unsigned int i = 0; i < 4; ++i)
	{
		glActiveTexture(GL_TEXTURE0); 
		glBindTexture(GL_TEXTURE_2D, m_fboTexture[i]);
		unsigned int difLoc = glGetUniformLocation(m_program, "diffuse");
		glUniform1i(difLoc, 0);

		glBindVertexArray(m_planes[i].m_vao);
		glBindBuffer(GL_ARRAY_BUFFER, m_planes[i].m_vbo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_planes[i].m_ibo);

		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
	}

	

//	Gizmos::addSphere(vec3(5, 0, 5), 0.5f, 8, 8, vec4(1, 1, 0, 1));
//	Gizmos::addSphere(vec3(-5, 0, -5), 0.5f, 8, 8, vec4(1, 0, 0, 1));
//	Gizmos::addSphere(vec3(5, 0, -5), 0.5f, 8, 8, vec4(0, 1, 0, 1));
//	Gizmos::addSphere(vec3(-5, 0, 5), 0.5f, 8, 8, vec4(0, 0, 1, 1));


	Gizmos::draw(GameCam->GetProjectionView());
}

void APP_RenderTargets::ClearMenu()
{
	TwDeleteBar(m_bar); //reset the gui
}

void APP_RenderTargets::CreateGui()
{
	m_bar = TwNewBar("RenderTargets");

	TwDefine(" RenderTargets position='10 10' "); // move bar to position (10, 10)
	TwDefine(" RenderTargets size='430 320' "); // resize bar	
	TwDefine(" RenderTargets color='128 128 128' alpha=32 ");   // semi-transparent blue bar
	TwDefine(" RenderTargets resizable=false "); // mybar cannot be resized


	TwAddButton(m_bar, "label_01", NULL, NULL, "label='4 cameras rendering to 4 render targets'"); //show as label		
	TwAddButton(m_bar, "mainMenu", Callback, this, "label='main menu'"); //show as button				
}

bool APP_RenderTargets::Start()
{
	m_appName = "Render Targets";

	Gizmos::create();

	m_time = 0;
	GameCam = new Camera();

	createFBO();
	for (unsigned int i = 0; i < 4; ++i)
		createGLplane(i);
	createGLShaderProgram();

	m_rotateMat = mat4(
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1);

	isLoaded = true;

	return true; //not being used in this lesson
}

void APP_RenderTargets::createFBOBuffers(unsigned int bufNum)
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_fbo[bufNum]);
	// TODO: we would attach render targets here!

	glBindTexture(GL_TEXTURE_2D, m_fboTexture[bufNum]);
	// specify texture format for storage 
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, 512, 512); //matches screen res
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// attach it to the framebuffer as the first colour attachment 
	// the FBO MUST still be bound 
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_fboTexture[bufNum], 0);

	// while the FBO is still bound 
	GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, drawBuffers);
}

void APP_RenderTargets::createFBO()
{
	// setup and bind a framebuffer 
	glGenFramebuffers(4, m_fbo);
	// create a texture and bind it 
	glGenTextures(4, m_fboTexture);

	for (unsigned int i = 0; i < 4; ++i)
	{
		createFBOBuffers(i);
	}

	//FBO depth buffer, not being used
//	// setup and bind a 24bit depth buffer as a render buffer 
//	glGenRenderbuffers(1, &m_fboDepth);
//	glBindRenderbuffer(GL_RENDERBUFFER, m_fboDepth);
//	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);
//	// while the FBO is still bound 
//	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
//		GL_RENDERBUFFER, m_fboDepth);

	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE)
		printf("Framebuffer Error!\n");

	// unbind the FBO so that we can render to the back buffer 
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void APP_RenderTargets::createGLShaderProgram()
{
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
		
	m_program = glCreateProgram(); 
	glAttachShader(m_program, vertexShader); 
	glAttachShader(m_program, fragmentShader); 
	glLinkProgram(m_program); 
	glDeleteShader(vertexShader); 
	glDeleteShader(fragmentShader);
}

void APP_RenderTargets::createGLplane(unsigned int bufNum)
{
	glm::vec2 tl;
	glm::vec2 tr;
	glm::vec2 bl;
	glm::vec2 br;	
	
	switch (bufNum)
	{
	case 0:
		bl = glm::vec2(-6,6);
		br = glm::vec2(0,6);
		tr = glm::vec2(-6,3);
		tl = glm::vec2(0,3);
		break;
	case 1:
		bl = glm::vec2(0, 6);
		br = glm::vec2(6, 6);
		tr = glm::vec2(0, 3);
		tl = glm::vec2(6, 3);
		break;
	case 2:
		bl = glm::vec2(-6, 3);
		br = glm::vec2(0, 3);
		tr = glm::vec2(-6, 0);
		tl = glm::vec2(0, 0);
		break;
	case 3:
		bl = glm::vec2(0, 3);
		br = glm::vec2(6, 3);
		tr = glm::vec2(0, 0);
		tl = glm::vec2(6, 0);
		break;
	}

	float vertexData[] = 
	{ 
		tl.x, tl.y, -10, 1,			0, 0,
		tr.x, tr.y, -10, 1,			1, 0,
		bl.x, bl.y, -10, 1,			1, 1,
		br.x, br.y, -10, 1,			0, 1,
	};

	unsigned int indexData[] = 
	{ 
		0, 1, 2, 
		0, 2, 3, 
	};

	glGenVertexArrays(1, &m_planes[bufNum].m_vao); 
	glBindVertexArray(m_planes[bufNum].m_vao);
	glGenBuffers(1, &m_planes[bufNum].m_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, m_planes[bufNum].m_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float)* 6 * 4, vertexData, GL_STATIC_DRAW); 
	glGenBuffers(1, &m_planes[bufNum].m_ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_planes[bufNum].m_ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int)* 6, indexData, GL_STATIC_DRAW); 
	glEnableVertexAttribArray(0); 
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(float)* 6, 0); 
	glEnableVertexAttribArray(1); 
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float)* 6, ((char*)0) + 16); 
	glBindVertexArray(0); 
	glBindBuffer(GL_ARRAY_BUFFER, 0); 
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

bool APP_RenderTargets::Shutdown()
{
	delete GameCam;
	Gizmos::destroy();

	return true; //not being used in this lesson
}