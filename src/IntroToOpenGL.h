#ifndef INTROOPENGL_H
#define INTROOPENGL_H
#include <Application.h>

class IntroOpenGl : public App
{
public:
	IntroOpenGl();
	~IntroOpenGl();
	void Update(float a_dt);
	void Draw();
	bool Start();
	bool Shutdown();

public:
	mat4 sphereTrans;
	mat4 parentTrans;
	float rot;
};


#endif INTROOPENGL_H