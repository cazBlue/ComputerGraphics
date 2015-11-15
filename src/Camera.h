#ifndef CAMERA_H
#define CAMERA_H

#include <glm/fwd.hpp>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <GLFW\glfw3.h>

struct GLFWwindow;

using glm::mat4;
using glm::vec3;

class Camera
{
public:
	//new from the camera tutorial
	vec3 m_pos, m_target, m_direction, m_up, m_right, m_front;
	mat4 m_view;

	void Do_movement(float a_dt);
	void Do_Mouse(float a_dt);

	float m_lastX, m_lastY, m_yaw, m_pitch;

	bool m_firstMouse = true;
	bool m_firstRun = true;

public:
	//variables
	glm::mat4 m_worldTransform;
	glm::mat4 m_viewTransform;
	glm::mat4 m_projectionTransform;
	glm::mat4 m_projectionViewTransform;

	float m_moveSpeed, m_lastMousePosX, m_lastMousePosY;

	bool m_moveLeft, m_moveRight, m_moveForward, m_moveBackward; //key input
	bool m_rotateLeft, m_rotateRight, m_rotateUp, m_rotateDown, m_tiltleft, m_tiltRight;

public:
	//functions
	Camera(glm::vec3 a_lookAt, glm::vec3 a_centre, glm::vec3 a_up, 
		float a_nearClip, float a_farClip, float a_FOV, float a_aspectRatio, float a_moveSpeed);
	Camera();
	~Camera();
	
	void Update(float a_dt);	
	void UpdateProjectionViewTransform();

	//glfw key and mouse callbacks
	void mouseUpdate();
	void key_update();

	void HandleInput(float a_dt);
	//setters
	void SetPerspective(float a_FOV, float a_aspectRatio,
		float a_near, float a_far);
	void SetLookAt(glm::vec3 a_from, glm::vec3 a_to, glm::vec3 a_up);
	void SetPosition(glm::vec3 a_position);
	
	//getters
	glm::mat4 GetWorldTransform();
	glm::mat4 GetView();
	glm::mat4 GetProjectionView();
};
#endif CAMERA_H