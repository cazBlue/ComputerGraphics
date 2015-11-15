#include <Camera.h>
#include <iostream>
#include <InputHandler.h>

using glm::vec3;
using glm::vec4;
using glm::mat4;
using std::cout;

//rebuild camera using: 
//	http://learnopengl.com/#!Getting-started/Camera

Camera::~Camera()
{
	//destroy pointers etc
}

Camera::Camera()
{
//	m_viewTransform = glm::lookAt(vec3(10, 10, 10), vec3(0), vec3(0, 1, 0));
//	m_projectionTransform = glm::perspective(glm::pi<float>() * 0.25f,
//		16 / 9.f, 0.1f, 1000.f);
//
//	m_worldTransform = glm::inverse(m_viewTransform);
//
//	m_moveSpeed = 100;
//	m_lastMousePosX = 0;
//	m_lastMousePosY = 0;

	//set the projection matrix
	m_projectionTransform = glm::perspective(glm::pi<float>() * 0.25f,
		16 / 9.f, 0.1f, 1000.f);

	//set initial position
	m_pos = vec3(0, 0, 2);	
	//set the initial front vector
	m_front = vec3(0,0,-1);

	//point the camera at 0,0,0 (world origin)
	m_target = glm::vec3(0.0f, 0.0f, 0.0f);
	m_direction = glm::normalize(m_pos - m_target);

	//get the camera right
	vec3 world_up = glm::vec3(0.0f, 1.0f, 0.0f);
	m_right = glm::normalize(glm::cross(world_up, m_direction));

	//get the camera up
	m_up = glm::cross(m_direction, m_right);

	//actual starting place - needs to be set after axes are formed
	m_pos = vec3(-6, 6, 10);	
	m_front = vec3(.55, -.3, -.7);

	//set initial pitch and yaw
	m_pitch = -25;
	m_yaw = -60;

	glm::vec3 front;
	front.x = cos(glm::radians(m_pitch)) * cos(glm::radians(m_yaw));
	front.y = sin(glm::radians(m_pitch));
	front.z = cos(glm::radians(m_pitch)) * sin(glm::radians(m_yaw));
	m_front = glm::normalize(front);

	m_view = glm::lookAt(m_pos, m_pos + m_front, m_up);
	m_worldTransform = glm::inverse(m_view);

	//centre the mouse
	m_lastX = APP_Inputhandler::lastMousePos.x;
	m_lastY = APP_Inputhandler::lastMousePos.y;

	m_worldTransform = glm::inverse(m_viewTransform);
}

void Camera::SetPosition(glm::vec3 a_position)
{
	//set the camera position
	m_worldTransform *= glm::translate(a_position);

	UpdateProjectionViewTransform(); //make sure things stay alinged
}

void Camera::SetPerspective(float a_FOV, float a_aspectRatio,
	float a_near, float a_far)
{
	//m_viewTransform = glm::lookAt(a_lookAt, a_centre, a_up);
	m_projectionTransform = glm::perspective(a_FOV,
		a_aspectRatio, a_near, a_far);

	m_worldTransform = glm::inverse(m_viewTransform);
}

Camera::Camera(glm::vec3 a_lookAt, glm::vec3 a_centre, glm::vec3 a_up, 
	float a_nearClip, float a_farClip, float a_FOV, float a_aspectRatio, float a_moveSpeed)
{
	m_viewTransform = glm::lookAt(a_lookAt, a_centre, a_up);
	m_projectionTransform = glm::perspective(a_FOV,
		a_aspectRatio, a_nearClip, a_farClip);

	m_worldTransform = glm::inverse(m_viewTransform);

	m_moveSpeed = a_moveSpeed;
}

void Camera::SetLookAt(glm::vec3 a_from, glm::vec3 a_to, glm::vec3 a_up)
{
	m_viewTransform = glm::lookAt(a_from, a_to, a_up);
	m_worldTransform = glm::inverse(m_viewTransform);
}

//call after updating the world transform to align the view transform
void Camera::UpdateProjectionViewTransform()
{	
	m_viewTransform = glm::inverse(m_worldTransform);
}

glm::mat4 Camera::GetProjectionView()
{

	mat4 world  = mat4(
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		m_pos.x + m_front.x, m_pos.y + m_front.y, m_pos.z + m_front.z, 1);

	//glm::mat4 mvp = model * view * proj;


	return m_projectionTransform * m_viewTransform;// * glm::inverse(world);
	

//	return m_projectionTransform * m_view;
}

void Camera::mouseUpdate()
{	
	float width = 1280; //TODO pass from main game - not dynamic!
	float height = 720; 

	float xpos = APP_Inputhandler::lastMousePos.x;
	float ypos = APP_Inputhandler::lastMousePos.y;

	float sensativity = .05f;

	//work out if the mouse moved
	if (m_lastMousePosX != xpos)
	{
		if (xpos < m_lastMousePosX)
			m_rotateLeft = true;
		else
			m_rotateRight = true;

		m_lastMousePosX = (float)xpos; //set to last position
	}

	if (m_lastMousePosY != ypos)
	{
		if (ypos < m_lastMousePosY)
			m_rotateUp = true;
		else
			m_rotateDown = true;

		m_lastMousePosY = (float)ypos; //set to last position
	}
}

void Camera::key_update()
{
	int key = APP_Inputhandler::lastKey;
	int action = APP_Inputhandler::lastKeyAction;	

//	void *data = glfwGetWindowUserPointer(window);
//	Camera *camera = static_cast<Camera *>(data);

	if (key == GLFW_KEY_D && action == GLFW_REPEAT || action == GLFW_PRESS)
		m_moveRight = true;

	if (key == GLFW_KEY_A && action == GLFW_REPEAT || action == GLFW_PRESS)
		m_moveLeft = true;

	if (key == GLFW_KEY_W && action == GLFW_REPEAT || action == GLFW_PRESS)
		m_moveForward = true;

	if (key == GLFW_KEY_S && action == GLFW_REPEAT || action == GLFW_PRESS)
		m_moveBackward = true;

	if (key == GLFW_KEY_Q && action == GLFW_REPEAT || action == GLFW_PRESS)
		m_tiltleft = true;

	if (key == GLFW_KEY_E && action == GLFW_REPEAT || action == GLFW_PRESS)
		m_tiltRight = true;
}

void Camera::HandleInput(float a_dt)
{
	if (m_moveLeft)
	{
		m_worldTransform *= glm::translate(vec3(-m_moveSpeed * a_dt, 0, 0));
		m_moveLeft = false;
	}

	if (m_moveRight)
	{
		m_worldTransform *= glm::translate(vec3(m_moveSpeed * a_dt, 0, 0));
		m_moveRight = false;
	}

	if (m_moveForward)
	{
		m_worldTransform *= glm::translate(vec3(0, 0, -m_moveSpeed * a_dt));
		m_moveForward = false;
	}

	if (m_moveBackward)
	{
		m_worldTransform *= glm::translate(vec3(0, 0, m_moveSpeed * a_dt));
		m_moveBackward = false;
	}

	if (m_rotateLeft)
	{
		m_worldTransform *= glm::rotate(.01f, vec3(0, 1, 0));
		m_rotateLeft = false;
	}

	if (m_rotateRight)
	{
		m_worldTransform *= glm::rotate(-0.01f, vec3(0, 1, 0));
		m_rotateRight = false;
	}

	if (m_rotateUp)
	{
		m_worldTransform *= glm::rotate(.01f, vec3(1, 0, 0));
		m_rotateUp = false;
	}

	if (m_rotateDown)
	{
		m_worldTransform *= glm::rotate(-0.01f, vec3(1, 0, 0));
		m_rotateDown = false;
	}

	if (m_tiltleft)
	{
		m_worldTransform *= glm::rotate(0.01f, vec3(0, 0, 1));
		m_tiltleft = false;
	}

	if (m_tiltRight)
	{
		m_worldTransform *= glm::rotate(-0.01f, vec3(0, 0, 1));
		m_tiltRight = false;
	}
}

void Camera::Update(float a_dt)
{
	//from opengl camera tut
	// http://learnopengl.com/#!Getting-started/Camera
	
	m_view = glm::lookAt(m_pos, m_pos + m_front, m_up);

	m_viewTransform = m_view;

//	m_worldTransform = glm::inverse(m_viewTransform);

	Do_movement(a_dt);
	Do_Mouse(a_dt);

	

//	key_update();
//	mouseUpdate();
//
//	HandleInput(a_dt); //check if the camera should be moved	
//
//	//re-align the camera
//	UpdateProjectionViewTransform();
}


void Camera::Do_Mouse(float a_dt)
{	

	if (m_firstRun)
	{
		m_lastX = APP_Inputhandler::lastMousePos.x;
		m_lastY = APP_Inputhandler::lastMousePos.y;

		m_firstRun = false;
	}

	if (m_firstMouse && APP_Inputhandler::focused)
	{
		m_lastX = APP_Inputhandler::lastMousePos.x;
		m_lastY = APP_Inputhandler::lastMousePos.y;

		m_firstMouse = false;
	}

	GLfloat xoffset = APP_Inputhandler::lastMousePos.x - m_lastX;
	GLfloat yoffset = m_lastY - APP_Inputhandler::lastMousePos.y; // Reversed since y-coordinates range from bottom to top
	
	m_lastX = APP_Inputhandler::lastMousePos.x;
	m_lastY = APP_Inputhandler::lastMousePos.y;
	
	GLfloat sensitivity = .05f;
	xoffset *= sensitivity;
	yoffset *= sensitivity;
	
	m_yaw		+= xoffset;
	m_pitch		+= yoffset;

	//std::cout << "offset: " << xoffset << std::endl;

	if (m_pitch > 89.0f)
		m_pitch = 89.0f;
	if (m_pitch < -89.0f)
		m_pitch = -89.0f;

	glm::vec3 front;
	front.x = cos(glm::radians(m_pitch)) * cos(glm::radians(m_yaw));
	front.y = sin(glm::radians(m_pitch));
	front.z = cos(glm::radians(m_pitch)) * sin(glm::radians(m_yaw));
	m_front = glm::normalize(front);
}

void Camera::Do_movement(float a_dt)
{
	int key = APP_Inputhandler::lastKey;
	int action = APP_Inputhandler::lastKeyAction;
	bool* keys = APP_Inputhandler::keys;

	// Camera controls
	GLfloat cameraSpeed = 5.0f * a_dt;
	if (keys[GLFW_KEY_W])
		m_pos += cameraSpeed * m_front;
	if (keys[GLFW_KEY_S])
		m_pos -= cameraSpeed * m_front;
	if (keys[GLFW_KEY_A])
		m_pos -= glm::normalize(glm::cross(m_front, m_up)) * cameraSpeed;
	if (keys[GLFW_KEY_D])
		m_pos += glm::normalize(glm::cross(m_front, m_up)) * cameraSpeed;
}

glm::mat4 Camera::GetWorldTransform()
{
//	glm::vec4 pos =  m_worldTransform[3];
	return m_worldTransform;
}

glm::mat4 Camera::GetView()
{
	return m_viewTransform;
}