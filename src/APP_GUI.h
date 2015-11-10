#ifndef APPGUI_H
#define APPGUI_H
#include <Application.h>
#include <AntTweakBar.h>

class APP_GUI : public App
{
public:
	APP_GUI();
	~APP_GUI();
	void Update(float a_dt);
	void Draw();
	bool Start();
	bool Shutdown();
	void ClearMenu();
	void CreateGui();

public:
	mat4 sphereTrans;
	mat4 parentTrans;
	float rot;

public:	
	vec4 m_sphereColour;

};


#endif APPGUI_H