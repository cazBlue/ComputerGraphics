#ifndef RENDERGEO_H
#define RENDERGEO_H
#include <Application.h>

class RenderGeo : public App
{
public:
	//inherited functions
	RenderGeo();
	~RenderGeo();
	void Update(float a_dt);
	void Draw();
	bool Start();
	bool Shutdown();
	std::string LoadShader(const char *a_filePath);

public:
	void generateGrid(unsigned int rows, unsigned int cols);

	// our vertex and index buffers
	unsigned int m_VAO;
	unsigned int m_VBO;
	unsigned int m_IBO;

	unsigned int m_programID;

};


#endif RENDERGEO_H