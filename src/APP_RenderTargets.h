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
	unsigned int m_fboDepth, m_program;
	
	struct plane
	{
		unsigned int m_vao, m_vbo, m_ibo;
	};

	plane m_planes[4];
	unsigned int m_fboTexture[4];
	unsigned int m_fbo[4];

	void createFBO();
	void createFBOBuffers(unsigned int bufNum);
	void createGLplane(unsigned int bufNum);
	void createGLShaderProgram();
};


#endif APP_RENDERTARGETS_H