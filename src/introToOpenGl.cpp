#include <IntroToOpenGL.h>

IntroOpenGl::IntroOpenGl()
{

}
IntroOpenGl::~IntroOpenGl()
{

}

void IntroOpenGl::Update(float a_dt)
{
	GameCam->Update(a_dt); //update camera

	rot += .0001;

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

bool IntroOpenGl::Start()
{
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

	return true; //not being used in this lesson
}

bool IntroOpenGl::Shutdown()
{
	delete GameCam;
	Gizmos::destroy();

	return true; //not being used in this lesson
}