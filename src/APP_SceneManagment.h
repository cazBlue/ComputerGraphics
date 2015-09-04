#ifndef APP_SCENEMANAGMENT_H
#define APP_SCENEMANAGMENT_H
#include <Application.h>

class APP_SCENEMANAGE : public App
{
public:
	APP_SCENEMANAGE();
	~APP_SCENEMANAGE();
	void Update(float a_dt);
	void Draw();
	bool Start();
	bool Shutdown();

public:
};


#endif APP_SCENEMANAGMENT_H