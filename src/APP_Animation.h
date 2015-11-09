#ifndef APPANIMATION_H
#define APPANIMATION_H
#include <Application.h>
#include <vector>

class FBXFile;
class FBXTexture;

class APP_Animation : public App
{
public:
	APP_Animation();
	~APP_Animation();
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

	//	std::string LoadShader(const char *a_filePath);

	//	unsigned int m_programID;

	//	unsigned int m_textureID1, m_textureID2, m_textureID3;

	//	unsigned int m_vbo, m_vao, m_ibo;

	void loadImg(FBXTexture* a_tex);

	//	void createOpenGLBuffers();

	//	struct OpenGLInfo
	//	{
	//		unsigned int m_VAO;
	//		unsigned int m_VBO;
	//		unsigned int m_IBO;
	//		unsigned int m_index_count;
	//	};
	//
	//	std::vector<OpenGLInfo> m_gl_info;

	//	FBXFile* fbxFile;

	void createOpenGLBuffers(FBXFile* fbx);
	void cleanupOpenGLBuffers(FBXFile* fbx);
	FBXFile* m_fbx;
	unsigned int m_program;

	float m_timer;
};


#endif APPANIMATION_H