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

using glm::vec3;
using glm::vec4;
using glm::mat4;
using std::cout;

//updated glm/gtx/scalar_multiplication.hpp with fix from https://github.com/g-truc/glm/issues/325


int main()
{
	if (glfwInit() == false)
		return -1;	

	GLFWwindow* window = glfwCreateWindow(1280, 720, "Computer Graphics", nullptr, nullptr);

	glfwMakeContextCurrent(window);

	TwInit(TW_OPENGL_CORE, nullptr);
	TwWindowSize(1280, 720);

	TwBar *myBar;
	myBar = TwNewBar("NameOfMyTweakBar");

	vec4 m_clearColour = vec4(1,1,1,1);

	TwAddVarRW(myBar, "clear colour",
		TW_TYPE_COLOR4F, &m_clearColour[0], "");

	if (ogl_LoadFunctions() == ogl_LOAD_FAILED) {
		glfwDestroyWindow(window);
		glfwTerminate();
		return -3;
	}

	auto major = ogl_GetMajorVersion();
	auto minor = ogl_GetMinorVersion();
	printf("GL: %i.%i\n", major, minor);	
	
	//App *appPtr = new IntroOpenGl();		//#1 & #2 create intro to opengl app
	//App *appPtr = new RenderGeo();		//#2 P1 create planes!
	//App *appPtr = new APP_OBJLoader();	//#3 P2 obj loader
	//App *appPtr = new APP_Texturing();	//#4 texturing!
	//App *appPtr = new APP_LoadFbx();		//#5 FBX loader and lighting!
	//App *appPtr = new APP_AdvTex();		//#6 advanced texturing (normal maps)!
	//App *appPtr = new APP_Animation();	//#7 animation (no lighting)
	//App *appPtr = new APP_Particles();    //#8 CPU billboard particles
	//App *appPtr = new APP_GPUParticles(); //#9 GPU billboard particles
	//App *appPtr = new APP_SpotRotate();     //#-- side step to work out rotation for particles (proof of concept)
	//App *appPtr = new APP_SCENEMANAGE();   //#10 scene management, could do with more work on the quad tree
	//App *appPtr = new APP_RenderTargets();  //#11 render targets
	//App *appPtr = new APP_postProcess();	//#12 post processing
	//App *appPtr = new APP_Shadows();				//#13 shadows
	//App *appPtr = new APP_DeferredRendering();		//#14 deferred rendering pt 1 & 2
	//App *appPtr = new APP_Proc_Generation();		//#15 procedural generation
	//App *appPtr = new APP_PhysicallyBased();		//#16 physically based rendering
	// App *appPtr = new APP_ImageBased();		//#17 image based rendering
	App *appPtr = new APP_GUI();				//#18 GUI

	appPtr->Start();



//	glfwSetMouseButtonCallback(m_window, OnMouseButton);
//	glfwSetCursorPosCallback(m_window, OnMousePosition);
//	glfwSetScrollCallback(m_window, OnMouseScroll);
//	glfwSetKeyCallback(m_window, OnKey);
//	glfwSetCharCallback(m_window, OnChar);
//	glfwSetWindowSizeCallback(m_window, OnWindowResize);



	//enable unlimited scrolling - hides the cursor
//	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	//set a pointer to the game camera http://stackoverflow.com/questions/27596861/give-static-function-access-to-data-without-passing-the-data-as-a-parameter
	//TODO move to key manager when created 
	glfwSetWindowUserPointer(window, appPtr->GameCam); //should be set to current app
//	glfwSetKeyCallback(window, appPtr->GameCam->key_callback);
//	glfwSetCursorPosCallback(window, appPtr->onMouseMove);

	APP_Inputhandler* inputHandler = new APP_Inputhandler();		

	glfwSetMouseButtonCallback(window, inputHandler->OnMouseButton);
	glfwSetCursorPosCallback(window, inputHandler->OnMousePosition);
	glfwSetScrollCallback(window, inputHandler->OnMouseScroll);
	glfwSetKeyCallback(window, inputHandler->OnKey);
	glfwSetCharCallback(window, inputHandler->OnChar);
	glfwSetWindowSizeCallback(window, inputHandler->OnWindowResize);

	
	float previousTime = 0.0f;

	while (glfwWindowShouldClose(window) == false &&
		glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS) {
		glClearColor(0.25f, 0.25f, 0.25f, 1);
		glEnable(GL_DEPTH_TEST); // enables the depth buffer
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		float currentTime = (float)glfwGetTime();
		float deltaTime = currentTime - previousTime; // prev of last frame
		previousTime = currentTime;
		
		appPtr->Update(deltaTime); //main update for current app
		appPtr->Draw();				//main draw call for current app

		TwDraw();  // draw the tweak bar(s)		

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	//shut down tweak bars
	TwDeleteAllBars();
	TwTerminate();

	//game over, clean up and de-allocate
	appPtr->Shutdown();
	delete appPtr;
	delete inputHandler;


//	delete GameCam;
	//Gizmos::destroy();
	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}