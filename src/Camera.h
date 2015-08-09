#ifndef CAMERA_H
#define CAMERA_H

#include <glm/fwd.hpp>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <GLFW\glfw3.h>

class GLFWwindow;

class Camera
{
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
	static void cursor_pos_callback(GLFWwindow* window, double xpos, double ypos);
	static void key_callback(GLFWwindow* window, int key, int scancode, 
		int action, int mods);


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