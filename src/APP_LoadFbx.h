#ifndef APPLOADFBX_H
#define APPLOADFBX_H
#include <Application.h>
#include <vector>

class FBXFile;

class APP_LoadFbx : public App
{
public:
	APP_LoadFbx();
	~APP_LoadFbx();
	void Update(float a_dt);
	void Draw();
	bool Start();
	bool Shutdown();

public:
	void createShaders();
	void drawObj();

	std::string LoadShader(const char *a_filePath);

//	std::string LoadShader(const char *a_filePath);

//	unsigned int m_programID;

//	unsigned int m_textureID1, m_textureID2, m_textureID3;

//	unsigned int m_vbo, m_vao, m_ibo;

	void loadImg(int* a_height, int* a_width, int* a_format, const char* a_path, unsigned int* a_id);

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
};


#endif APPLOADFBX_H