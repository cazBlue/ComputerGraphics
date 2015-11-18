#ifndef APPPHYSICALBASE_H
#define APPPHYSICALBASE_H
#include <Application.h>
#include <vector>

class FBXFile;

class APP_PhysicallyBased : public App
{
public:
	APP_PhysicallyBased();
	~APP_PhysicallyBased();
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
	unsigned int m_program;

public:
	//interactive elements

	float m_roughness, m_fresnelScale;
};


#endif APPPHYSICALBASE_H