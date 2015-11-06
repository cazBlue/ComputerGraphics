#ifndef APPIMAGEBASE_H
#define APPIMAGEBASE_H
#include <Application.h>
#include <vector>

class FBXFile;

class APP_ImageBased : public App
{
public:
	APP_ImageBased();
	~APP_ImageBased();
	void Update(float a_dt);
	void Draw();
	bool Start();
	bool Shutdown();

public:
	void createShaders();
	void drawObj();

	void loadImg(int* a_height, int* a_width, int* a_format, const char* a_path, unsigned int* a_id);

	std::string LoadShader(const char *a_filePath);


	void createOpenGLBuffers(FBXFile* fbx);
	void cleanupOpenGLBuffers(FBXFile* fbx);
	FBXFile* m_fbx;
	unsigned int m_program;

public:
	unsigned int m_textureID1;

};


#endif APPIMAGEBASE_H