#include <APP_Particles.h>

APP_Particles::APP_Particles()
: m_particles(nullptr),
m_firstDead(0),
m_maxParticles(0),
m_position(0, 0, 0),
m_vertexData(nullptr)
{

}
APP_Particles::~APP_Particles()
{
	delete[] m_particles;
	delete[] m_vertexData;
}

void APP_Particles::Update(float a_dt)
{
	GameCam->Update(a_dt); //update camera

	particleUpdate(a_dt);
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

	particleDraw();
}

void APP_Particles::ClearMenu()
{
	TwDeleteBar(m_bar); //reset the gui
}

void APP_Particles::CreateGui()
{
	m_bar = TwNewBar("CPUParticles");

	TwDefine(" CPUParticles position='10 10' "); // move bar to position (10, 10)
	TwDefine(" CPUParticles size='430 320' "); // resize bar	
	TwDefine(" CPUParticles color='128 128 128' alpha=32 ");   // semi-transparent blue bar
	TwDefine(" CPUParticles resizable=false "); // mybar cannot be resized

	TwAddButton(m_bar, "label_01", NULL, NULL, "label='pasrticles running on the CPU'"); //show as label		
	TwAddButton(m_bar, "mainMenu", Callback, this, "label='main menu'"); //show as button				
}

bool APP_Particles::Start()
{
	m_appName = "CPU Particles";

	Gizmos::create();
	GameCam = new Camera();
	
//	m_emitter = new APP_particleEmitter();

	const char* vsSource = "#version 410\n \
						   layout(location=0) in vec4 Position; \
						   layout(location=1) in vec4 Colour; \
						   out vec4 colour; \
						   uniform mat4 projectionView; \
						   void main() { \
						   colour = Colour; \
						   gl_Position = projectionView * Position;}";


	const char* fsSource = "#version 410\n \
						   in vec4 colour; \
						   out vec4 FragColor; \
						   void main() { \
						   FragColor = colour;}";
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

	glm::vec4 scolour = glm::vec4(1, 0, 0, 1);
	glm::vec4 ecolour = glm::vec4(1, 1, 0, 1);
	
	initalise(1000, 100, .01f, 3, .01f, 10.0f, .1f, 1.0f, scolour, ecolour);

	isLoaded = true;

	return true; //not being used in this lesson
}

bool APP_Particles::Shutdown()
{
	delete GameCam;
	Gizmos::destroy();
//	delete m_emitter;

	glDeleteVertexArrays(1, &m_vao);
	glDeleteBuffers(1, &m_vbo);
	glDeleteBuffers(1, &m_ibo);
	return true; //not being used in this lesson
}


void APP_Particles::initalise(unsigned int a_maxParticles,
	unsigned int a_emitRate,
	float a_lifetimeMin, float a_lifetimeMax,
	float a_velocityMin, float a_velocityMax,
	float a_startSize, float a_endSize,
	const glm::vec4& a_startColour, const glm::vec4& a_endColour) {
	// set up emit timers
	m_emitTimer = 0;
	m_emitRate = 1.0f / a_emitRate;
	// store all variables passed in
	m_startColour = a_startColour;
	m_endColour = a_endColour;
	m_startSize = a_startSize;
	m_endSize = a_endSize;
	m_velocityMin = a_velocityMin;
	m_velocityMax = a_velocityMax;
	m_lifespanMin = a_lifetimeMin;
	m_lifespanMax = a_lifetimeMax;
	m_maxParticles = a_maxParticles;
	// create particle array
	m_particles = new Particle[m_maxParticles];
	m_firstDead = 0;
	// create the array of vertices for the particles
	// 4 vertices per particle for a quad.
	// will be filled during update
	m_vertexData = new ParticleVertex[m_maxParticles * 4];

	// create the index buffeer data for the particles
	// 6 indices per quad of 2 triangles
	// fill it now as it never changes
	unsigned int* indexData = new unsigned int[m_maxParticles * 6];
	for (unsigned int i = 0; i < m_maxParticles; ++i) {
		indexData[i * 6 + 0] = i * 4 + 0;
		indexData[i * 6 + 1] = i * 4 + 1;
		indexData[i * 6 + 2] = i * 4 + 2;
		indexData[i * 6 + 3] = i * 4 + 0;
		indexData[i * 6 + 4] = i * 4 + 2;
		indexData[i * 6 + 5] = i * 4 + 3;
	}
	// create opengl buffers
//	glUseProgram(m_programID);
	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);
	glGenBuffers(1, &m_vbo);
	glGenBuffers(1, &m_ibo);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, m_maxParticles * 4 *
		sizeof(ParticleVertex), m_vertexData,
		GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_maxParticles * 6 *
		sizeof(unsigned int), indexData, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0); // position
	glEnableVertexAttribArray(1); // colour
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE,
		sizeof(ParticleVertex), 0);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE,
		sizeof(ParticleVertex), ((char*)0) + 16);
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	delete[] indexData;
}


void APP_Particles::emit() {
	// only emit if there is a dead particle to use
	if (m_firstDead >= m_maxParticles)
		return;
	// resurrect the first dead particle
	Particle& particle = m_particles[m_firstDead++];
	// assign its starting position
	particle.position = m_position;
	// randomise its lifespan
	particle.lifetime = 0;
	particle.lifespan = (rand() / (float)RAND_MAX) *
		(m_lifespanMax - m_lifespanMin) + m_lifespanMin;
	// set starting size and colour
	particle.colour = m_startColour;
	particle.size = m_startSize;
	// randomise velocity direction and strength
	float velocity = (rand() / (float)RAND_MAX) * (m_velocityMax - m_velocityMin) + m_velocityMin;
	particle.velocity.x = (rand() / (float)RAND_MAX) * 2 - 1;
	particle.velocity.y = (rand() / (float)RAND_MAX) * 2 - 1;
	particle.velocity.z = (rand() / (float)RAND_MAX) * 2 - 1;
	particle.velocity = glm::normalize(particle.velocity) * velocity;

	//random spawn within a sphere : http://stackoverflow.com/questions/5531827/random-point-on-a-given-sphere
	//refernce also: http://mathworld.wolfram.com/SpherePointPicking.html

	float radius = 2.0f; //sphere radius
	glm::vec3 pos = glm::vec3(0,2,0); //sphere position effectively moves the emitter
	
	//http://stackoverflow.com/questions/686353/c-random-float-number-generation
	float u = static_cast <float> (rand()) / static_cast <float> (RAND_MAX); 	
	float v = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
	float theta = 2 * glm::pi<float>() * u;
	float phi = glm::acos(2 * v - 1);
	float x = pos.x + (radius * glm::sin(phi) * glm::cos(theta));
	float y = pos.y + (radius * glm::sin(phi) * glm::sin(theta));
	float z = pos.z + (radius * glm::cos(phi));
	
	particle.position = vec3(x,y,z);
}


void APP_Particles::particleUpdate(float a_deltaTime) {
	using glm::vec3;
	using glm::vec4;
	// spawn particles
	m_emitTimer += a_deltaTime;
	while (m_emitTimer > m_emitRate) {
		emit();
		m_emitTimer -= m_emitRate;
	}

	unsigned int quad = 0;
	// update particles and turn live particles into billboard quads
	for (unsigned int i = 0; i < m_firstDead; i++) {
		Particle* particle = &m_particles[i];
		particle->lifetime += a_deltaTime;
		if (particle->lifetime >= particle->lifespan) {
			// swap last alive with this one
			*particle = m_particles[m_firstDead - 1];
			m_firstDead--;
		}
		else {
			// move particle
			particle->position += particle->velocity * a_deltaTime;
			//apply gravity: http://gamedev.stackexchange.com/questions/32631/easy-way-to-do-gravity-in-a-simple-game
			particle->velocity += glm::vec3(0, -.01, 0);
			//check that we haven't gone below 0
//			if (particle->position.y < 0)
//				particle->position.y = 0;
			// size particle
			particle->size = glm::mix(m_startSize, m_endSize,
				particle->lifetime / particle->lifespan);
			// colour particle
			particle->colour = glm::mix(m_startColour, m_endColour,
				particle->lifetime / particle->lifespan);
			// make a quad the correct size and colour
			float halfSize = particle->size * 0.5f;
			m_vertexData[quad * 4 + 0].position = vec4(halfSize,
				halfSize, 0, 1);
			m_vertexData[quad * 4 + 0].colour = particle->colour;
			m_vertexData[quad * 4 + 1].position = vec4(-halfSize,
				halfSize, 0, 1);
			m_vertexData[quad * 4 + 1].colour = particle->colour;
			m_vertexData[quad * 4 + 2].position = vec4(-halfSize,
				-halfSize, 0, 1);
			m_vertexData[quad * 4 + 2].colour = particle->colour;
			m_vertexData[quad * 4 + 3].position = vec4(halfSize,
				-halfSize, 0, 1);
			m_vertexData[quad * 4 + 3].colour = particle->colour;
			// create billboard transform

			vec3 temp = vec3(GameCam->GetWorldTransform()[1]);
			vec3 temp2 = vec3(GameCam->GetWorldTransform()[3]);



			vec3 zAxis = glm::normalize(vec3(GameCam->GetWorldTransform()[3]) - particle->position);
			vec3 xAxis = glm::cross(vec3(GameCam->GetWorldTransform()[1]), zAxis);
			vec3 yAxis = glm::cross(zAxis, xAxis);
			glm::mat4 billboard(vec4(xAxis, 0),
				vec4(yAxis, 0),
				vec4(zAxis, 0),
				vec4(0, 0, 0, 1));
			m_vertexData[quad * 4 + 0].position = billboard *
				m_vertexData[quad * 4 + 0].position +
				vec4(particle->position, 0);
			m_vertexData[quad * 4 + 1].position = billboard *
				m_vertexData[quad * 4 + 1].position +
				vec4(particle->position, 0);
			m_vertexData[quad * 4 + 2].position = billboard *
				m_vertexData[quad * 4 + 2].position +
				vec4(particle->position, 0);
			m_vertexData[quad * 4 + 3].position = billboard *
				m_vertexData[quad * 4 + 3].position +
				vec4(particle->position, 0);
			++quad;
		}
	}
}


void APP_Particles::particleDraw()
{

	//	int view_proj_uniform = glGetUniformLocation(m_programID, "ProjectionView");
	//	glUniformMatrix4fv(view_proj_uniform, 1, GL_FALSE, glm::value_ptr(GameCam->GetProjectionView()));

	glUseProgram(m_programID);
//
//	//update camera project view

	int view_proj_uniform = glGetUniformLocation(m_programID, "projectionView");
	glUniformMatrix4fv(view_proj_uniform, 1, GL_FALSE, glm::value_ptr(GameCam->GetProjectionView()));

	// sync the particle vertex buffer
	// based on how many alive particles there are
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferSubData(GL_ARRAY_BUFFER, 0, m_firstDead * 4 * sizeof(ParticleVertex), m_vertexData);
//	glBufferData(GL_ARRAY_BUFFER, m_firstDead * 4 *
//		sizeof(ParticleVertex), m_vertexData,
//		GL_STATIC_DRAW);
	// draw particles
	glBindVertexArray(m_vao);
//	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); //enable wireframe render
	glDrawElements(GL_TRIANGLES, m_firstDead * 6, GL_UNSIGNED_INT, nullptr);

	glUseProgram(0);
}