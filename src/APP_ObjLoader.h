#ifndef APPOBJLOADER_H
#define APPOBJLOADER_H
#include <Application.h>
#include <tiny_obj_loader.h>

class APP_OBJLoader : public App
{
public:
	APP_OBJLoader();
	~APP_OBJLoader();
	void Update(float a_dt);
	void Draw();
	bool Start();
	bool Shutdown();

public:
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;

	void createOpenGLBuffers(std::vector<tinyobj::shape_t>& shapes);
	
	void createShaders();
	void drawObj();
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


#endif APPOBJLOADER_H