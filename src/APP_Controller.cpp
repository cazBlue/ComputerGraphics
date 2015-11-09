#include <APP_Controller.h>
#include <iostream>

//apps includes
#include <IntroToOpenGL.h>
#include <RenderingGeo.h>
#include <APP_ObjLoader.h>
#include <App_LoadingTextures.h>
#include <APP_LoadFbx.h>
#include <APP_AdvTexturing.h>
#include <APP_Animation.h>
#include <APP_Particles.h>
#include <APP_GpuParticles.h>
#include <APP_SpotRotate.h>
#include <APP_SceneManagment.h>
#include <APP_RenderTargets.h>
#include <APP_PostProcessing.h>
#include <APP_Shadows.h>
#include <APP_DeferredRendering.h>
#include <APP_ProceduralGeneration.h>
#include <APP_PhysicallyBased.h>
#include <APP_ImageBased.h>
#include <APP_GUI.h>
#include <APP_Splash.h>


//App *appPtr = new IntroOpenGl();		//#1 & #2 create intro to opengl app
//App *appPtr = new RenderGeo();		//#2 P1 create planes!
//App *appPtr = new APP_OBJLoader();	//#3 P2 obj loader
//App *appPtr = new APP_Texturing();	//#4 texturing!
//App *appPtr = new APP_LoadFbx();		//#5 FBX loader and lighting!
//App *appPtr = new APP_AdvTex();		//#6 advanced texturing (normal maps)!
//App *appPtr = new APP_Animation();	//#7 animation (no lighting)
//App *appPtr = new APP_Particles();    //#8 CPU billboard particles
//App *appPtr = new APP_GPUParticles(); //#9 GPU billboard particles
//App *appPtr = new APP_SpotRotate();     //#-- side step to work out rotation for particles (proof of concept)
//App *appPtr = new APP_SCENEMANAGE();   //#10 scene management, could do with more work on the quad tree
//App *appPtr = new APP_RenderTargets();  //#11 render targets
//App *appPtr = new APP_postProcess();	//#12 post processing
//App *appPtr = new APP_Shadows();				//#13 shadows
//App *appPtr = new APP_DeferredRendering();		//#14 deferred rendering pt 1 & 2
//App *appPtr = new APP_Proc_Generation();		//#15 procedural generation
//App *appPtr = new APP_PhysicallyBased();		//#16 physically based rendering
//App *appPtr = new APP_ImageBased();		//#17 image based rendering
//App *appPtr = new APP_GUI();				//#18 GUI
//App *appPtr = new APP_Splash();				//#18 splash screen
//
//appPtr->Start();



APP_Control::APP_Control()
{
	m_loading = true;
	m_beginLoading = false;

	m_splashApp = new APP_Splash();
	m_splashApp->Start(); //init the splash screen app

	m_curApp = m_splashApp;	
}

APP_Control::~APP_Control()
{
	
}

void APP_Control::Start()
{
	//push all apps into memory for menu display

	m_apps.push_front(new APP_AdvTex());		//#6 advanced texturing (normal maps)!
	m_apps.push_front(new APP_LoadFbx());		//#5 FBX loader and lighting!
	m_apps.push_front(new APP_Texturing());	//#4 texturing!
	//m_apps.push_front(new APP_OBJLoader()); //#3 P2 obj loader //take out while debugging
	m_apps.push_front(new RenderGeo());		//#2 P1 create planes!
	m_apps.push_front(new IntroOpenGl());	//#1 & #2 create intro to opengl app

	//make the splash screen aware of the apps list
	static_cast<APP_Splash*>(m_splashApp)->SetAppList(&m_apps);

	//set the loading list
	m_appsToLoad = m_apps;

	m_loadingApp = m_apps.front();

	m_apps.push_front(m_splashApp); //skips adding the splash app to the loading list
}

void APP_Control::LoadAPP(App* a_app)
{		
	
}

void APP_Control::Update(float dt)
{
	m_curApp->Update(dt);

	if (m_loading && m_beginLoading)
		CheckLoadStatus();
	else
		CheckNextScene();

	m_beginLoading = true;
}

void APP_Control::CheckNextScene()
{
	if (App::nextScene != "")
	{
		list <App *>::iterator iter;
		for (iter = m_apps.begin(); iter != m_apps.end(); iter++)
		{
			if (App::nextScene == m_curApp->m_appName)
			{								
				m_curApp = m_splashApp;
				m_curApp->CreateGui();

				App::nextScene = ""; //reset the next scene flag
			}

			if ((*iter)->m_appName == App::nextScene)
			{
				//a scene button has been pressed

				//clear the menu
				m_curApp->ClearMenu();
				m_curApp = (*iter);
				m_curApp->CreateGui();

				App::nextScene = ""; //reset the next scene flag
			}
		}
	}
}

void APP_Control::CheckLoadStatus()
{
	//only load 1 app per update

	bool stillLoading = true;
	bool loadingApp = false;

	if (m_loadingApp->isLoaded)
	{		
		if (m_appsToLoad.size() > 0)
		{
			m_loadingApp = m_appsToLoad.front();
			m_appsToLoad.pop_front();
		}
		else
			m_loading = false; //done loading apps

		static_cast<APP_Splash*>(m_splashApp)->SetDirtyGui();
	}
	else
		m_loadingApp->Start();
}


void APP_Control::Draw()
{
	m_curApp->Draw();
}

void APP_Control::Shutdown()
{

}