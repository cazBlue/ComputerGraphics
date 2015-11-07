#include <Camera.h>
#include <iostream>
#include <InputHandler.h>

using glm::vec3;
using glm::vec4;
using glm::mat4;
using std::cout;

Camera::~Camera()
{
	//destroy pointers etc
}

Camera::Camera()
{
	m_viewTransform = glm::lookAt(vec3(10, 10, 10), vec3(0), vec3(0, 1, 0));
	m_projectionTransform = glm::perspective(glm::pi<float>() * 0.25f,
		16 / 9.f, 0.1f, 1000.f);

	m_worldTransform = glm::inverse(m_viewTransform);

	m_moveSpeed = 100;
	m_lastMousePosX = 0;
	m_lastMousePosY = 0;
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
	return m_projectionTransform * m_viewTransform;
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
	key_update();
	mouseUpdate();

	HandleInput(a_dt); //check if the camera should be moved	

	//re-align the camera
	UpdateProjectionViewTransform();
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