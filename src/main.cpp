#include <gl_core_4_4.h>
#include <GLFW/glfw3.h>
#include <aie/Gizmos.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <iostream>
#include <Camera.h>
#include <Application.h>
#include <IntroToOpenGL.h>
#include <RenderingGeo.h>
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

	if (ogl_LoadFunctions() == ogl_LOAD_FAILED) {
		glfwDestroyWindow(window);
		glfwTerminate();
		return -3;
	}

	auto major = ogl_GetMajorVersion();
	auto minor = ogl_GetMinorVersion();
	printf("GL: %i.%i\n", major, minor);	
	
	//App *appPtr = new IntroOpenGl(); //create into to opengl app
	App *appPtr = new RenderGeo(); //create into to opengl app

	appPtr->Start();

	//enable unlimited scrolling - hides the cursor
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	//set a pointer to the game camera http://stackoverflow.com/questions/27596861/give-static-function-access-to-data-without-passing-the-data-as-a-parameter
	//TODO move to key manager when created 
	glfwSetWindowUserPointer(window, appPtr->GameCam); 
	glfwSetKeyCallback(window, appPtr->GameCam->key_callback);
	glfwSetCursorPosCallback(window, appPtr->GameCam->cursor_pos_callback);
	
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

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	//game over, clean up and de-allocate
	appPtr->Shutdown();
	delete appPtr;
//	delete GameCam;
	//Gizmos::destroy();
	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}