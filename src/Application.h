#ifndef APPLICATION_H
#define APPLICATION_H

#include <gl_core_4_4.h>
#include <GLFW/glfw3.h>
#include <aie/Gizmos.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <string>
#include <Camera.h>


using glm::vec3;
using glm::vec4;
using glm::mat4;

//Abstract base class for all apps/lessens withing the 
//computer graphics assessment

//abstract base class definition reference http://www.cplusplus.com/doc/tutorial/polymorphism/

//more reference http://www.tutorialspoint.com/cplusplus/cpp_interfaces.htm

class App
{
public: //variables
	std::string m_appName;	//used by app controller to identify current app
	Camera *GameCam;	
	bool isLoaded = false;		//used by app controller to check if app is loaded



public: //functions
	virtual bool Start() =0;
	virtual bool Shutdown() =0;
	virtual void Update(float a_dt) =0;
	virtual void Draw() =0;

public: 
};

#endif APPLICATION_H