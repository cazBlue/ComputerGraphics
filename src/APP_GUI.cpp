#include <APP_GUI.h>

APP_GUI::APP_GUI()
{

}
APP_GUI::~APP_GUI()
{

}

void APP_GUI::Update(float a_dt)
{
	//GameCam->Update(a_dt); //update camera

	rot += .0001f;

	parentTrans = glm::rotate(rot, vec3(0, 1, 0));

	parentTrans *= glm::translate(vec3(0, 2, 0));

}

void APP_GUI::Draw()
{
	Gizmos::clear();
	Gizmos::addTransform(glm::mat4(1));

	vec4 white(1);
	vec4 black(0, 0, 0, 1);

	//	draws the home grid
	for (int i = 0; i < 21; ++i) {
		Gizmos::addLine(vec3(-10 + i, 0, 10),
			vec3(-10 + i, 0, -10),
			i == 10 ? white : black);
		Gizmos::addLine(vec3(10, 0, -10 + i),
			vec3(-10, 0, -10 + i),
			i == 10 ? white : black);
	}

	glm::mat4 worldPos = parentTrans * sphereTrans;
	vec4 pos = worldPos[3];
	Gizmos::addSphere(pos.xyz(), 3.f, 12, 12, m_sphereColour, &worldPos);

	Gizmos::draw(GameCam->GetProjectionView());	
}

void APP_GUI::ClearMenu()
{
	TwDeleteBar(m_bar); //reset the gui	
}

void APP_GUI::CreateGui()
{
	m_bar = TwNewBar("GUI");

	TwDefine(" GUI position='10 10' "); // move bar to position (10, 10)
	TwDefine(" GUI size='250 250' "); // resize bar	
	TwDefine(" GUI color='128 128 128' alpha=32 ");   // semi-transparent blue bar
	TwDefine(" GUI resizable=false "); // mybar cannot be resized

	TwAddButton(m_bar, "label_01", NULL, NULL, "label='The original implementation of the antTweakBar for this project'"); //show as label		
	
	TwAddButton(m_bar, "mainMenu", Callback, this, "label='main menu'"); //show as button				

	m_sphereColour = vec4(.5, .5, .5, 1);

	TwAddVarRW(m_bar, "sphere colour",
		TW_TYPE_COLOR4F, &m_sphereColour[0], "");
}


bool APP_GUI::Start()
{
	m_appName = "GUI";

	Gizmos::create();

	GameCam = new Camera();
	parentTrans = mat4(
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1);

	rot = 0.0f;

	sphereTrans = mat4(
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		10, 0, 0, 1);



	isLoaded = true;

	return true; //not being used in this lesson
}

bool APP_GUI::Shutdown()
{
	delete GameCam;
	Gizmos::destroy();

	return true; //not being used in this lesson
}