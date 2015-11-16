#include <gl_core_4_4.h>
#include <GLFW/glfw3.h>
#include <aie/Gizmos.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <iostream>
#include <AntTweakBar.h>

#include <InputHandler.h>
#include <APP_Controller.h>


using glm::vec3;
using glm::vec4;
using glm::mat4;
using std::cout;

//updated glm/gtx/scalar_multiplication.hpp with fix from https://github.com/g-truc/glm/issues/325


//temp
#include <IntroToOpenGL.h>

int notmain()
{
	IntroOpenGl* intro = new IntroOpenGl();

	intro->Start();

	delete intro;


	return 0;
}

int main()
{
	if (glfwInit() == false)
		return -1;	

	GLFWwindow* window = glfwCreateWindow(1280, 720, "Computer Graphics", nullptr, nullptr);

	glfwMakeContextCurrent(window);

	TwInit(TW_OPENGL_CORE, nullptr);
	TwWindowSize(1280, 720);


	if (ogl_LoadFunctions() == ogl_LOAD_FAILED) {
		glfwDestroyWindow(window);
		glfwTerminate();
		return -3;
	}

	auto major = ogl_GetMajorVersion();
	auto minor = ogl_GetMinorVersion();
	printf("GL: %i.%i\n", major, minor);	
	
	
	//set a pointer to the game camera http://stackoverflow.com/questions/27596861/give-static-function-access-to-data-without-passing-the-data-as-a-parameter	
//	glfwSetWindowUserPointer(window, appPtr->GameCam); //should be set to current app


	APP_Inputhandler* inputHandler = new APP_Inputhandler();		

	//input handler callbacks
	glfwSetMouseButtonCallback(window, inputHandler->OnMouseButton);
	glfwSetCursorPosCallback(window, inputHandler->OnMousePosition);
	glfwSetScrollCallback(window, inputHandler->OnMouseScroll);
	glfwSetKeyCallback(window, inputHandler->OnKey);
	glfwSetCharCallback(window, inputHandler->OnChar);
	glfwSetWindowSizeCallback(window, inputHandler->OnWindowResize);
	glfwSetWindowFocusCallback(window, inputHandler->OnWindowFocus);


	//create app controller
	APP_Control* appCtrl = new APP_Control();
	appCtrl->Start();

	//temp while building new camera
//	IntroOpenGl* intro = new IntroOpenGl();
//	intro->Start();


	bool showMouse = true;
	float clickTime = 0;
	float previousTime = 0.0f;

	while (glfwWindowShouldClose(window) == false &&
		glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS) {
		glClearColor(0.25f, 0.25f, 0.25f, 1);
		glEnable(GL_DEPTH_TEST); // enables the depth buffer
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		float currentTime = (float)glfwGetTime();
		float deltaTime = currentTime - previousTime; // prev of last frame
		previousTime = currentTime;	

		appCtrl->Update(deltaTime);		//main update for apps
		appCtrl->Draw();				//main draw call for apps

		if (APP_Inputhandler::keys[GLFW_KEY_SPACE] && clickTime <= 0)
		{
			clickTime = 0.5f;
			if (showMouse)
				showMouse = false;
			else
				showMouse = true;
		}

		if (clickTime > 0)
			clickTime -= deltaTime;


		if (showMouse)	
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);		
		else	
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); //enable unlimited scrolling - hides the cursor						
		
		//intro->Update(deltaTime);
		//intro->Draw();

		TwDraw();  // draw the tweak bar(s)		

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	//shut down tweak bars
	TwDeleteAllBars();
	TwTerminate();

	//game over, clean up and de-allocate
	appCtrl->Shutdown();
	//intro->Shutdown();
	//delete intro;
	delete appCtrl;
	delete inputHandler;


//	delete GameCam;
	//Gizmos::destroy();
	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}