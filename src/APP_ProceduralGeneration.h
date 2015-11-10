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
	void ClearMenu();
	void CreateGui();

	std::string LoadShader(const char *a_filePath);

public: //mesh data
	
	enum Offsets
	{
		PositionOffset = 0,
		TexCoord2Offset = PositionOffset + sizeof(glm::vec4),
	};

	struct Vertex {
		vec4 position;
		glm::vec2 uv;
	};	

	void generateMesh();
	void createMeshBuffers(unsigned int* auiIndices, Vertex* aoVertices);

	unsigned int m_VAO, m_VBO, m_IBO, m_program, rows, cols, m_indexCount;

	void createShaders();

public: //noise data

	void createPerlinNoise();
	void createPerlinBuffers();

	float* perlinData;

	unsigned int m_perlin_texture;


};



#endif APP_PROC_GENERATION