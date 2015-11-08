#ifndef APP_CONTROLLER_H
#define APP_CONTROLLER_H

#include <Application.h>
#include <list>

using std::list;

class APP_Control
{
public:
	APP_Control();
	~APP_Control();

	void LoadAPP(App* a_app);
	void CheckLoadStatus();
	void CheckNextScene(); //check if next scene has been selected

	void Start();
	void Update(float dt);
	void Draw();

	void Shutdown();
	
public:	
	list<App*> m_apps;
	list<App*> m_appsToLoad;



	App* m_curApp;
	App* m_splashApp;
	App* m_loadingApp;

	bool m_loading;
	bool m_beginLoading; //used to force the loop to run once to display splash screen
};

#endif