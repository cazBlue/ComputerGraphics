#include <Application.h>


//void App::onMouseMove(GLFWwindow* window, double xpos, double ypos)
//{	
//	void *data = glfwGetWindowUserPointer(window);
//	Camera *camera = static_cast<Camera *>(data);
//
//	float width = 1280; //TODO pass from main game - not dynamic!
//	float height = 720;
//
//	//create a dead zone in the centre of the screen
//	//if (xpos )
//	//cout << "xpos: " << xpos << " ypos: " << ypos << std::endl;
//
//	float sensativity = .05f;
//
//	//work out if the mouse moved
//	if (camera->m_lastMousePosX != xpos)
//	{
//		if (xpos < camera->m_lastMousePosX)
//			camera->m_rotateLeft = true;
//		else
//			camera->m_rotateRight = true;
//
//		camera->m_lastMousePosX = (float)xpos; //set to last position
//	}
//
//	if (camera->m_lastMousePosY != ypos)
//	{
//		if (ypos < camera->m_lastMousePosY)
//			camera->m_rotateUp = true;
//		else
//			camera->m_rotateDown = true;
//
//		camera->m_lastMousePosY = (float)ypos; //set to last position
//	}
//}
//
//void App::OnKey(GLFWwindow* window, int key, int scancode,
//	int action, int mods)
//{	
//	Camera *camera = GameCam;
//
//	if (key == GLFW_KEY_D && action == GLFW_REPEAT || action == GLFW_PRESS)
//		camera->m_moveRight = true;
//
//	if (key == GLFW_KEY_A && action == GLFW_REPEAT || action == GLFW_PRESS)
//		camera->m_moveLeft = true;
//
//	if (key == GLFW_KEY_W && action == GLFW_REPEAT || action == GLFW_PRESS)
//		camera->m_moveForward = true;
//
//	if (key == GLFW_KEY_S && action == GLFW_REPEAT || action == GLFW_PRESS)
//		camera->m_moveBackward = true;
//
//	if (key == GLFW_KEY_Q && action == GLFW_REPEAT || action == GLFW_PRESS)
//		camera->m_tiltleft = true;
//
//	if (key == GLFW_KEY_E && action == GLFW_REPEAT || action == GLFW_PRESS)
//		camera->m_tiltRight = true;
//}