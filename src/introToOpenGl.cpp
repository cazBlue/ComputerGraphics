#include <IntroToOpenGL.h>

IntroOpenGl::IntroOpenGl()
{
	m_appName = "intro to openGL";
	isLoaded = false;
}
IntroOpenGl::~IntroOpenGl()
{

}

void IntroOpenGl::Update(float a_dt)
{
	GameCam->Update(a_dt); //update camera

	rot += .0001f;

	parentTrans = glm::rotate(rot, vec3(0, 1, 0));

	parentTrans *= glm::translate(vec3(0, 2, 0));

}

void IntroOpenGl::Draw()
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
	Gizmos::addSphere(pos.xyz(), 3.f, 12, 12, white, &worldPos);

	Gizmos::draw(GameCam->GetProjectionView());
}

void IntroOpenGl::ClearMenu()
{
	TwDeleteBar(m_bar); //reset the gui
}

void IntroOpenGl::CreateGui()
{	
	m_bar = TwNewBar("IntroToOpenGL");

	TwDefine(" IntroToOpenGL position='10 10' "); // move bar to position (10, 10)
	TwDefine(" IntroToOpenGL size='430 320' "); // resize bar	
	TwDefine(" IntroToOpenGL color='128 128 128' alpha=32 ");   // semi-transparent blue bar
	TwDefine(" IntroToOpenGL resizable=false "); // mybar cannot be resized


	TwAddButton(m_bar, "label_01", NULL, NULL, "label='simply opengl scene, nothing much happening'"); //show as label		
	TwAddButton(m_bar, "mainMenu", Callback, this, "label='main menu'"); //show as button				
}

bool IntroOpenGl::Start()
{
	Gizmos::create();

	m_appName = "intro to openGL";

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

bool IntroOpenGl::Shutdown()
{
	delete GameCam;
	Gizmos::destroy();

	return true; //not being used in this lesson
}