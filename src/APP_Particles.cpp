#include <APP_Particles.h>


APP_Particles::APP_Particles() : m_vao(0), m_vbo(0), m_ibo(0)
{

}
APP_Particles::~APP_Particles()
{

}

void APP_Particles::Update(float a_dt)
{
	GameCam->Update(a_dt); //update camera

	m_emitter->update(a_dt, GameCam->GetProjectionView());
}

void APP_Particles::Draw()
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

	m_emitter->draw(m_vao, m_vbo, m_programID, GameCam);
}

bool APP_Particles::Start()
{
	Gizmos::create();
	GameCam = new Camera();
	
	m_emitter = new APP_particleEmitter();

	const char* vsSource = "#version 410\n \
						   in vec4 Position; \
						   in vec4 Colour; \
						   out vec4 colour; \
						   uniform mat4 projectionView; \
						   void main() { \
						   colour = Colour; \
						   gl_Position = projectionView * Position;}";
	const char* fsSource = "#version 410\n \
						   in vec4 colour; \
						   void main() { \
						   gl_FragColor = colour;}";
	unsigned int vs = glCreateShader(GL_VERTEX_SHADER);
	unsigned int fs = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(vs, 1, &vsSource, nullptr);
	glShaderSource(fs, 1, &fsSource, nullptr);
	glCompileShader(fs);
	glCompileShader(vs);
	m_programID = glCreateProgram();
	glAttachShader(m_programID, vs);
	glAttachShader(m_programID, fs);
	glLinkProgram(m_programID);
	glDeleteShader(fs);
	glDeleteShader(vs);
	glm::vec4 scolour = glm::vec4(.3, 0, .3, 1);
	glm::vec4 ecolour = glm::vec4(.8, .2, .6, 1);
	
	m_emitter->initalise(1000, 100, .01f, 1, 1, 10.0f, 1.0f, .1f, 
		scolour, ecolour, &m_vao, &m_vbo, &m_ibo, m_programID);

	return true; //not being used in this lesson
}

bool APP_Particles::Shutdown()
{
	delete GameCam;
	Gizmos::destroy();
	delete m_emitter;

	glDeleteVertexArrays(1, &m_vao);
	glDeleteBuffers(1, &m_vbo);
	glDeleteBuffers(1, &m_ibo);
	return true; //not being used in this lesson
}