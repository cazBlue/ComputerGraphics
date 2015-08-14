#ifndef APPLOADFBX_H
#define APPLOADFBX_H
#include <Application.h>

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

	unsigned int m_programID;

	unsigned int m_textureID1, m_textureID2, m_textureID3;

	unsigned int m_vbo, m_vao, m_ibo;

	void loadImg(int* a_height, int* a_width, int* a_format, const char* a_path, unsigned int* a_id);

	//FBXFile* fbxFile;
};


#endif APPLOADFBX_H