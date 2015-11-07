#ifndef APP_SPLASH_H
#define APP_SPLASH_H
#include <Application.h>
#include <AntTweakBar.h>
#include <list>

using std::list;

class APP_Splash : public App
{
public:
	APP_Splash();
	~APP_Splash();
	void Update(float a_dt);
	void Draw();
	bool Start();
	bool Shutdown();
	std::string LoadShader(const char *a_filePath);

public:
	void CreateFrameBuffer();
	void CreateFullSreenQuad();

	void loadImg(int* a_height, int* a_width, int* a_format, const char* a_path, unsigned int* a_id);

	unsigned int m_vao, m_vbo, m_programBackBuffer, m_splashImg;
	
	void createBackBufferBuffers();
	void CreateGui(); 
	static void TW_CALL Callback(void *clientData);
	static std::string nextScene;
	void SetAppList(list<App*>* apps);

	list<App*>* m_apps; //pointer to main list of apps

	TwBar* m_bar;
	
	void SetDirtyGui();
	bool m_guiDirty; //marks the gui for re-generating

};


#endif APP_SPLASH_H