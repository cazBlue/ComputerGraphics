#ifndef APP_POSTPROCESS_H
#define APP_POSTPROCESS_H
#include <Application.h>

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

	unsigned int m_fbo, m_fboDepth, m_fboTexture;

	unsigned int m_vao, m_vbo, m_program;
};


#endif APP_POSTPROCESS_H