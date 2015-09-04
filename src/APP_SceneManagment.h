#ifndef APP_SCENEMANAGMENT_H
#define APP_SCENEMANAGMENT_H
#include <Application.h>
#include <vector>
#include <tiny_obj_loader.h>
#include <list>

class QuadTree;

class APP_SCENEMANAGE : public App
{
public:
	APP_SCENEMANAGE();
	~APP_SCENEMANAGE();
	void Update(float a_dt);
	void Draw();
	bool Start();
	bool Shutdown();

public:

	QuadTree* quadTree;

	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;

	void getFrustumPlanes(const glm::mat4& transform, glm::vec4* planes);
	void createOpenGLBuffers(std::vector<tinyobj::shape_t>& shapes);
	void drawObj();
	void createShaders();
	std::string LoadShader(const char *a_filePath);

	unsigned int m_programID;

	struct OpenGLInfo
	{
		unsigned int m_VAO;
		unsigned int m_VBO;
		unsigned int m_IBO;
		unsigned int m_index_count;
	};

	std::vector<OpenGLInfo> m_gl_info;
};

class BoundingSphere
{
public: 
		BoundingSphere() : centre(0), radius(0) { } ~BoundingSphere() {}
		
		void fit(const std::vector<glm::vec3>& points);
		
		glm::vec3 centre; 
		float radius;
};


#endif APP_SCENEMANAGMENT_H