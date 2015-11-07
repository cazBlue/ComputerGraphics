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

	void Start();
	void Update(float dt);
	void Draw();

	void Shutdown();
	
public:	
	list<App*> apps;

	App* curApp;
	App* splashApp;

	bool m_loading;

};

#endif