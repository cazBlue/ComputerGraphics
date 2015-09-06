#include <APP_RenderTargets.h>

APP_RenderTargets::APP_RenderTargets()
{

}
APP_RenderTargets::~APP_RenderTargets()
{

}

void APP_RenderTargets::Update(float a_dt)
{
	GameCam->Update(a_dt); //update camera

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

	for (unsigned int i = 0; i < 4; ++i)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, m_fbo[i]);
		//render full res viewport (matches screen res)
		//this will be paired down
		glViewport(0, 0, 512, 512); 
		glClearColor(0.75f, 0.75f, 0.75f, 1);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// draw our meshes, or gizmos, to the render target Gizmos::draw(m_camera->getProjectionView());
		Gizmos::addSphere(vec3(5, 0, 5), 0.5f, 8, 8, vec4(1, 1, 0, 1));
		Gizmos::addSphere(vec3(-5, 0, -5), 0.5f, 8, 8, vec4(1, 0, 0, 1));
		Gizmos::addSphere(vec3(5, 0, -5), 0.5f, 8, 8, vec4(0, 1, 0, 1));
		Gizmos::addSphere(vec3(-5, 0, 5), 0.5f, 8, 8, vec4(0, 0, 1, 1));
		Gizmos::draw(GameCam->GetProjectionView());
	}
	//revert to the back buffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0); 
	glViewport(0, 0, 1280, 720);
	glClearColor(0.25f, 0.25f, 0.25f, 1); 
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(m_program);
	int loc = glGetUniformLocation(m_program, "ProjectionView"); 
	glUniformMatrix4fv(loc, 1, GL_FALSE, &(GameCam->GetProjectionView()[0][0]));
	glActiveTexture(GL_TEXTURE0); 
	glBindTexture(GL_TEXTURE_2D, m_fboTexture[0]);
	unsigned int difLoc = glGetUniformLocation(m_program, "diffuse");
	glUniform1i(difLoc, 0);
	for (unsigned int i = 0; i < 4; ++i)
	{
		glBindVertexArray(m_planes[i].m_vao);
		glBindBuffer(GL_ARRAY_BUFFER, m_planes[i].m_vbo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_planes[i].m_ibo);
	}

	glDrawElements(GL_TRIANGLES, 6 * 4, GL_UNSIGNED_INT, 0);

	Gizmos::addSphere(vec3(5, 0, 5), 0.5f, 8, 8, vec4(1, 1, 0, 1));
	Gizmos::addSphere(vec3(-5, 0, -5), 0.5f, 8, 8, vec4(1, 0, 0, 1));
	Gizmos::addSphere(vec3(5, 0, -5), 0.5f, 8, 8, vec4(0, 1, 0, 1));
	Gizmos::addSphere(vec3(-5, 0, 5), 0.5f, 8, 8, vec4(0, 0, 1, 1));


	Gizmos::draw(GameCam->GetProjectionView());
}

bool APP_RenderTargets::Start()
{
	Gizmos::create();

	GameCam = new Camera();

	createFBO();
	for (unsigned int i = 0; i < 4; ++i)
		createGLplane(i);
	createGLShaderProgram();


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
	vec3 tl;
	vec3 tr;
	vec3 bl;
	vec3 br;	
	
	switch (bufNum)
	{
	case 0:
		bl = vec3(-6,6,-5);
		br = vec3(0,6,-5);
		tr = vec3(-6,3,-5);
		tl = vec3(0,3,-5);
		break;
	case 1:
		bl = vec3(0, 6, -5);
		br = vec3(6, 6, -5);
		tr = vec3(0, 3, -5);
		tl = vec3(6, 3, -5);
		break;
	case 2:
		bl = vec3(-6, 3, -5);
		br = vec3(0, 0, -5);
		tr = vec3(-6, 3, -5);
		tl = vec3(0, 0, -5);
		break;
	case 3:
		bl = vec3(0, 3, -5);
		br = vec3(6, 3, -5);
		tr = vec3(0, 0, -5);
		tl = vec3(6, 0, -5);
		break;
	}

	float vertexData[] = 
	{ 
		tl.x, tl.y, -5, 1,			0, 0,
		tr.x, tr.y, -5, 1,			1, 0,
		bl.x, bl.y, -5, 1,			1, 1,
		br.x, br.y, -5, 1,			0, 1,
	};

	unsigned int indexData[] = 
	{ 
		0, 1, 2, 
		0, 2, 3, 
	};

	glGenVertexArrays(1, &m_planes[bufNum].m_vao); 
	glBindVertexArray(m_planes->m_vao);
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
	glBindVertexArray(0); glBindBuffer(GL_ARRAY_BUFFER, 0); 
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

bool APP_RenderTargets::Shutdown()
{
	delete GameCam;
	Gizmos::destroy();

	return true; //not being used in this lesson
}