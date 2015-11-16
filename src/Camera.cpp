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
	m_pos = vec3(0, 0, 10);	
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


	//centre the mouse
	m_lastX = APP_Inputhandler::lastMousePos.x;
	m_lastY = APP_Inputhandler::lastMousePos.y;	
}


void Camera::ManualSnap()
{
//	glm::vec3 front;
//	front.x = cos(glm::radians(m_pitch)) * cos(glm::radians(m_yaw));
//	front.y = sin(glm::radians(m_pitch));
//	front.z = cos(glm::radians(m_pitch)) * sin(glm::radians(m_yaw));
//	m_front = glm::normalize(front);

	m_view = glm::lookAt(m_pos, m_pos + m_front, m_up);
}

void Camera::SetPosition(glm::vec3 a_position)
{
	m_pos = a_position;

}

void Camera::SetFront(glm::vec3 a_front)
{
	m_front = a_front;

}

void Camera::SetPitchYaw(float a_pitch, float a_yaw)
{
	m_pitch = a_pitch;
	m_yaw = a_yaw;
}

glm::mat4 Camera::GetProjectionView()
{
	return m_projectionTransform * m_view;// * glm::inverse(world);
	

//	return m_projectionTransform * m_view;
}

void Camera::Update(float a_dt)
{
	//from opengl camera tut
	// http://learnopengl.com/#!Getting-started/Camera

	m_view = glm::lookAt(m_pos, m_pos + m_front, m_up);

	Do_movement(a_dt);
	Do_Mouse(a_dt);
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
	{
		m_pos += cameraSpeed * m_front;
		//m_worldTransform *= glm::translate(cameraSpeed * m_front);
	}
	if (keys[GLFW_KEY_S])
	{
		m_pos -= cameraSpeed * m_front;
		//m_worldTransform *= glm::translate(-(cameraSpeed * m_front));
	}
	if (keys[GLFW_KEY_A])
	{
		m_pos -= glm::normalize(glm::cross(m_front, m_up)) * cameraSpeed;
		//m_worldTransform *= glm::translate(-(glm::normalize(glm::cross(m_front, m_up)) * cameraSpeed));
	}
	if (keys[GLFW_KEY_D])
	{
		m_pos += glm::normalize(glm::cross(m_front, m_up)) * cameraSpeed;
		//m_worldTransform *= glm::translate((glm::normalize(glm::cross(m_front, m_up)) * cameraSpeed));
	}
}

glm::mat4 Camera::GetWorldTransform()
{
//	glm::vec4 pos =  m_worldTransform[3];
	//return m_worldTransform;

	return glm::inverse(m_view);
}

glm::mat4 Camera::GetView()
{
	return m_view;
}