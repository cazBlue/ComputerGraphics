#ifndef APP_POSTPROCESS_H
#define APP_POSTPROCESS_H
#include <Application.h>
#include <tiny_obj_loader.h>

class APP_postProcess : public App
{
public:
	APP_postProcess();
	~APP_postProcess();
	void Update(float a_dt);
	void Draw();
	bool Start();
	bool Shutdown();
	std::string LoadShader(const char *a_filePath);

public:
	void CreateFrameBuffer();
	void CreateTriangles();

	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;

	unsigned int m_fbo, m_fboDepth, m_fboTexture;

	unsigned int m_vao, m_vbo, m_program;

	void createOpenGLBuffers(std::vector<tinyobj::shape_t>& shapes);

	struct OpenGLInfo
	{
		unsigned int m_VAO;
		unsigned int m_VBO;
		unsigned int m_IBO;
		unsigned int m_index_count;
	};

	std::vector<OpenGLInfo> m_gl_info;
};


#endif APP_POSTPROCESS_H