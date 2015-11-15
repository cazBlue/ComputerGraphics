#include <InputHandler.h>

//init static variables
glm::vec2 APP_Inputhandler::lastMousePos = glm::vec2();
int APP_Inputhandler::lastKey = 0;
int APP_Inputhandler::lastKeyAction = 0;
bool APP_Inputhandler::keys[1024];
bool APP_Inputhandler::focused = false;


void APP_Inputhandler::OnWindowFocus(GLFWwindow*, int f)
{
	if (f)
		APP_Inputhandler::focused = true;
	else
		APP_Inputhandler::focused = false;
}
	

void APP_Inputhandler::OnMouseButton(GLFWwindow*, int b, int a, int m)
{
	TwEventMouseButtonGLFW(b, a);
}

void APP_Inputhandler::OnMousePosition(GLFWwindow*, double x, double y) {
	TwEventMousePosGLFW((int)x, (int)y);
	lastMousePos = glm::vec2((float)x, (float)y);
}

void APP_Inputhandler::OnMouseScroll(GLFWwindow*, double x, double y) {
	TwEventMouseWheelGLFW((int)y);
}

void APP_Inputhandler::OnKey(GLFWwindow*, int k, int s, int a, int m) {
	TwEventKeyGLFW(k, a);

	if (a == GLFW_PRESS)
		keys[k] = true;
	else if (a == GLFW_RELEASE)
		keys[k] = false;
	
	lastKey = k;
	lastKeyAction = a;
}

void APP_Inputhandler::OnChar(GLFWwindow*, unsigned int c) {
	TwEventCharGLFW(c, GLFW_PRESS);
}

void APP_Inputhandler::OnWindowResize(GLFWwindow*, int w, int h) {
	TwWindowSize(w, h);
	glViewport(0, 0, w, h);
}