#ifndef APPLICATION_H
#define APPLICATION_H

#include <gl_core_4_4.h>
#include <GLFW/glfw3.h>
#include <aie/Gizmos.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <string>
#include <Camera.h>
#include <AntTweakBar.h>
#include <ImportControl.h>

using glm::vec3;
using glm::vec4;
using glm::mat4;
using std::string;

//Abstract base class for all apps/lessens withing the 
//computer graphics assessment

//abstract base class definition reference http://www.cplusplus.com/doc/tutorial/polymorphism/

//more reference http://www.tutorialspoint.com/cplusplus/cpp_interfaces.htm

struct OpenGLInfo
{
	unsigned int m_VAO;
	unsigned int m_VBO;
	unsigned int m_IBO;
	unsigned int m_index_count;
};

class App
{
public: //variables
	std::string m_appName;			//used by app controller to identify current app
	Camera *GameCam;	
	bool isLoaded = false;			//used by app controller to check if app is loaded
	TwBar* m_bar;					//gui
	static std::string nextScene;	//the next scene that should be loaded
	ImportCTRL* importCtrl;			//setup in the import controller

public: //functions
	virtual bool Start() =0;
	virtual bool Shutdown() =0;
	virtual void Update(float a_dt) =0;
	virtual void Draw() =0;
	virtual void ClearMenu() =0;
	virtual void CreateGui() = 0;
	static void TW_CALL Callback(void *clientData);

public: 
};

#endif APPLICATION_H