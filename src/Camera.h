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
	glm::mat4 m_projectionTransform;	

public:
	//functions
	Camera();
	~Camera();
	
	void Update(float a_dt);	


	//glfw key and mouse callbacks

	//setters
	void SetFront(glm::vec3 a_front);
	void SetPosition(glm::vec3 a_position);
	void SetPitchYaw(float a_pitch, float a_yaw);
	void ManualSnap();
	void SetMouseSnapToCurrent();
	void SetToDefault();

	//getters
	glm::mat4 GetWorldTransform();
	glm::mat4 GetView();
	glm::mat4 GetProjectionView();
};
#endif CAMERA_H