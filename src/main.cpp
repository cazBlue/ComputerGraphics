#include <gl_core_4_4.h>
#include <GLFW/glfw3.h>
#include <aie/Gizmos.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <iostream>
#include <AntTweakBar.h>
#include <Camera.h>
#include <Application.h>
#include <IntroToOpenGL.h>
#include <RenderingGeo.h>
#include <APP_ObjLoader.h>
#include <App_LoadingTextures.h>
#include <APP_LoadFbx.h>
#include <APP_AdvTexturing.h>
#include <APP_Animation.h>
#include <APP_Particles.h>
#include <APP_GpuParticles.h>
#include <APP_SpotRotate.h>
#include <APP_SceneManagment.h>
#include <APP_RenderTargets.h>
#include <APP_PostProcessing.h>
#include <APP_Shadows.h>
#include <APP_DeferredRendering.h>
#include <APP_ProceduralGeneration.h>
#include <APP_PhysicallyBased.h>
#include <APP_ImageBased.h>
#include <APP_GUI.h>
#include <InputHandler.h>
#include <APP_Splash.h>
#include <APP_Controller.h>

#include <ObjIO.h>
#include <FBXIO.h>

using glm::vec3;
using glm::vec4;
using glm::mat4;
using std::cout;

//updated glm/gtx/scalar_multiplication.hpp with fix from https://github.com/g-truc/glm/issues/325


int notmain()
{

	//FBXIO* fbxio = new FBXIO();
	//fbxio->WriteObj();
	//
	//delete fbxio;

	//OBJIO* objio = new OBJIO();	
	//objio->ReadObj();
	//delete objio;
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
	
	



	//enable unlimited scrolling - hides the cursor
//	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

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


	//create app controller
	APP_Control* appCtrl = new APP_Control();
	appCtrl->Start();


	
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

		TwDraw();  // draw the tweak bar(s)		

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	//shut down tweak bars
	TwDeleteAllBars();
	TwTerminate();

	//game over, clean up and de-allocate
	appCtrl->Shutdown();
	delete appCtrl;
	delete inputHandler;


//	delete GameCam;
	//Gizmos::destroy();
	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}