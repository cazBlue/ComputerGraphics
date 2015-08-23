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

public:

	APP_particleEmitter* m_emitter;

	unsigned int m_vao, m_vbo, m_ibo;
	
	unsigned int m_programID;
};


#endif APP_PARTICLES_H