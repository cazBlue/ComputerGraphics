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


	splashApp = new APP_Splash();

	splashApp->Start(); //init the splash screen app

	curApp = splashApp;

	apps.push_front(splashApp);
}

APP_Control::~APP_Control()
{
	
}

void APP_Control::Start()
{
	//begin loading all apps
	LoadAPP(new IntroOpenGl());

	//make the splash screen aware of the apps list
	static_cast<APP_Splash*>(splashApp)->SetAppList(&apps);
}

void APP_Control::LoadAPP(App* a_app)
{	
	a_app->Start();
	apps.push_front(a_app);
}

void APP_Control::Update(float dt)
{
	if (m_loading)
		CheckLoadStatus();
	else
//		CheckNextScene();

	curApp->Update(dt);	
}

void APP_Control::CheckLoadStatus()
{
	bool stillLoading = true;

	list <App *>::iterator iter;
	for (iter = apps.begin(); iter != apps.end(); iter++)
	{
		if ((*iter)->isLoaded)
		{
			//mark as loaded
			std::cout << "loaded: " << (*iter)->m_appName << std::endl;
			stillLoading = false;
			
			static_cast<APP_Splash*>(splashApp)->SetDirtyGui();
		}
	}	

	if (!stillLoading)
		m_loading = false;
}


void APP_Control::Draw()
{
	curApp->Draw();
}

void APP_Control::Shutdown()
{

}