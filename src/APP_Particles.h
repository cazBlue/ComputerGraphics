#ifndef APP_PARTICLES_H
#define APP_PARTICLES_H
#include <Application.h>
#include <APP_Particles_emitter.h>

class APP_Particles : public App
{
public:
	APP_Particles();
	~APP_Particles();
	void Update(float a_dt);
	void Draw();
	bool Start();
	bool Shutdown();
	void ClearMenu();
	void CreateGui();

public:

//	APP_particleEmitter* m_emitter;

//	unsigned int m_vao, m_vbo, m_ibo;
	
	unsigned int m_programID;

	void particleUpdate(float a_dt);
	void particleDraw();

	//gui interactive variables
	vec3 m_windDirection;
	float m_windStrength;
	double m_windDir[3];
		


	struct Particle {
		glm::vec3 position;
		glm::vec3 velocity;
		glm::vec4 colour;
		float size;
		float lifetime;
		float lifespan;
	};
	struct ParticleVertex {
		glm::vec4 position;
		glm::vec4 colour;
	};

public:
	void initalise(unsigned int a_maxParticles,
		unsigned int a_emitRate,
		float a_lifetimeMin, float a_lifetimeMax,
		float a_velocityMin, float a_velocityMax,
		float a_startSize, float a_endSize,
		const glm::vec4& a_startColour, const glm::vec4& a_endColour);

	void emit();

	void update(float a_deltaTime);
	void draw();

protected:
	Particle* m_particles;
	unsigned int m_firstDead;
	unsigned int m_maxParticles;
	unsigned int m_vao, m_vbo, m_ibo;
	ParticleVertex* m_vertexData;

	//member variabes	
	float m_emitTimer;
	float m_emitRate;
	float m_lifespanMin;
	float m_lifespanMax;
	float m_velocityMin;
	float m_velocityMax;
	float m_startSize;
	float m_endSize;
	glm::vec4 m_startColour;
	glm::vec4 m_endColour;
	glm::vec3 m_position;

};


#endif APP_PARTICLES_H