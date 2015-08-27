#ifndef GPUPARTICLES_H
#define GPUPARTICLES_H
#include <Application.h>

class APP_GPUParticles : public App
{
public:
	APP_GPUParticles();
	~APP_GPUParticles();
	void Update(float a_dt);
	void Draw();
	bool Start();
	bool Shutdown();

public:

};


#endif GPUPARTICLES_H