#ifndef APP_RENDERTARGETS_H
#define APP_RENDERTARGETS_H
#include <Application.h>

class APP_RenderTargets : public App
{
public:
	APP_RenderTargets();
	~APP_RenderTargets();
	void Update(float a_dt);
	void Draw();
	bool Start();
	bool Shutdown();

public:
	unsigned int m_fbo, m_fboTexture, m_fboDepth, 
		m_vao, m_vbo, m_ibo, m_program;
	
	void createGLplane();
	void createGLShaderProgram();
};


#endif APP_RENDERTARGETS_H