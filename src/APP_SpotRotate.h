#ifndef APP_SPOTROTATE_H
#define APP_SPOTROTATE_H
#include <Application.h>

class APP_SpotRotate : public App
{
public:
	//inherited functions
	APP_SpotRotate();
	~APP_SpotRotate();
	void Update(float a_dt);
	void Draw();
	bool Start();
	bool Shutdown();
	std::string LoadShader(const char *a_filePath);

public:
	void generateGrid(unsigned int rows, unsigned int cols);
	
	void generatePlane();
	// our vertex and index buffers
	unsigned int m_VAO;
	unsigned int m_VBO;
	unsigned int m_IBO;

	unsigned int m_programID;

	float m_time;
};


#endif APP_SPOTROTATE_H