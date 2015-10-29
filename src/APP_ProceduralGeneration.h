#ifndef APP_PROC_GENERATION
#define APP_PROC_GENERATION
#include <Application.h>


class APP_Proc_Generation : public App
{
public:
	APP_Proc_Generation();
	~APP_Proc_Generation();


	void Update(float a_dt);
	void Draw();
	bool Start();
	bool Shutdown();

public:
	
	struct Vertex {
		vec4 position;
		glm::vec2 uv;
	};	

	void generateMesh(unsigned int rows, unsigned int cols);
	void createMeshBuffers(unsigned int* auiIndices, Vertex* aoVertices);

	unsigned int m_VAO, m_VBO, m_IBO, m_program;

};



#endif APP_PROC_GENERATION