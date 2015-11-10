#ifndef APP_SHADOWS_H
#define APP_SHADOWS_H
#include <Application.h>
#include <vector>

class FBXFile;

class APP_Shadows : public App
{
public:
	APP_Shadows();
	~APP_Shadows();
	void Update(float a_dt);
	void Draw();
	bool Start();
	bool Shutdown();
	void ClearMenu();
	void CreateGui();

public:
	void createShaders();
	void drawObj();

	std::string LoadShader(const char *a_filePath);

	void loadImg(int* a_height, int* a_width, int* a_format, const char* a_path, unsigned int* a_id);

	void createOpenGLBuffers(FBXFile* fbx);
	void cleanupOpenGLBuffers(FBXFile* fbx);
	FBXFile* m_fbx;

	unsigned int m_shadowGenProgram, m_useShadowProgram;

	unsigned int m_fbo, m_fboDepth;

	void createLightProgram();

	void createShadowMapBuffers();

	glm::mat4 m_lightMatrix;
	glm::vec3 m_lightDirection;

	void createShadowGenProgram();
	void generatePlane();

	unsigned int m_plane_vbo, m_plane_ibo, m_plane_vao;

	float m_time;
};


#endif APP_SHADOWS_H