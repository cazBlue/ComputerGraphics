#include <APP_Splash.h>

#include <stb_image.h>
#include <fstream>
#include <string>
#include <iostream>
#include <stdio.h>

using std::string;

//static variables
string APP_Splash::nextScene = string("");

APP_Splash::APP_Splash()
{

}

APP_Splash::~APP_Splash()
{

}

void APP_Splash::Update(float a_dt)
{
	GameCam->Update(a_dt); //update camera

	if (m_guiDirty)
		CreateGui();
}

void APP_Splash::Draw()
{			
	// draw out full-screen quad
	glUseProgram(m_programBackBuffer);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_splashImg); //bind the splash image
	int loc = glGetUniformLocation(m_programBackBuffer, "diffuse");
	glUniform1i(loc, 0);


	glBindVertexArray(m_vao);
	glDrawArrays(GL_TRIANGLES, 0, 6);
}

bool APP_Splash::Start()
{	
	GameCam = new Camera();

	m_appName = "splash screen";	
	m_bar = TwNewBar("Select an app");

	//load image
	int imageWidth = 0, imageHeight = 0, imageFormat = 0; //throw away variables
	m_splashImg = 0;

	loadImg(&imageHeight, &imageWidth, &imageFormat, "./assets/textures/splash.tga", &m_splashImg);

	CreateFullSreenQuad();

	createBackBufferBuffers();
	

	isLoaded = true;
	m_guiDirty = true;

	return true; //not being used in this lesson
}

void APP_Splash::SetDirtyGui()
{
	m_guiDirty = true;
}

void APP_Splash::SetAppList(list<App*>* a_apps)
{
	m_apps = a_apps;
}

void APP_Splash::CreateGui()
{
	TwDeleteBar(m_bar); //reset the gui
	m_bar = TwNewBar("Select an app");
	
	int counter = 0;
	list <App *>::iterator iter;
	for (iter = m_apps->begin(); iter != m_apps->end(); iter++)
	{
		if ((*iter)->m_appName != m_appName) //don't show the splash screen
		{
			string name = "Info." + std::to_string(counter);
			string label = " label='";
			if ((*iter)->isLoaded)
				label += std::to_string(counter) + ") " + (*iter)->m_appName + "'";
			else
				label += std::to_string(counter) + ") LOADING -- " + (*iter)->m_appName + "'";


			TwAddButton(m_bar, name.c_str(), Callback, (*iter), label.c_str());

			counter++;
		}
	}

	m_guiDirty = false;
}

void TW_CALL APP_Splash::Callback(void *clientData)
{
	// do something
	nextScene = static_cast<App*>(clientData)->m_appName;
}

void APP_Splash::loadImg(int* a_height, int* a_width, int* a_format, const char* a_path, unsigned int* a_id)
{
	unsigned char* data = stbi_load(a_path, a_width, a_height, a_format, STBI_rgb); //request no alpha

	glGenTextures(1, a_id);
	glBindTexture(GL_TEXTURE_2D, (*a_id));
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, (*a_width), (*a_height), 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	stbi_image_free(data); //unload the image data
}

void APP_Splash::createBackBufferBuffers()
{
	//load shaders
	// create shaders
	const char* vsSource = nullptr;
	std::string vsResult = LoadShader("./assets/shaders/splash/SplashVert.vert");
	vsSource = vsResult.c_str();

	const char* fsSource = nullptr;
	std::string fsResult = LoadShader("./assets/shaders/splash/SplashFrag.frag");
	fsSource = fsResult.c_str();

	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

	glShaderSource(vertexShader, 1, (const char**)&vsSource, 0);
	glCompileShader(vertexShader);
	glShaderSource(fragmentShader, 1, (const char**)&fsSource, 0);
	glCompileShader(fragmentShader);

	m_programBackBuffer = glCreateProgram();
	glAttachShader(m_programBackBuffer, vertexShader);
	glAttachShader(m_programBackBuffer, fragmentShader);
	glLinkProgram(m_programBackBuffer);
}

bool APP_Splash::Shutdown()
{
	delete GameCam;	

	return true; //not being used in this lesson
}

void APP_Splash::CreateFullSreenQuad()
{
	// fullscreen quad
	glm::vec2 halfTexel = 1.0f / glm::vec2(1280, 720) * 0.5f; //half texel


	//not using an indice object, single quad is simple
	float vertexData[] = {
		-1, 1, 0, 1,		halfTexel.x, halfTexel.y,
		1, 1, 0, 1,			1 - halfTexel.x, halfTexel.y,
		-1, -1, 0, 1,		halfTexel.x, 1 - halfTexel.y,
		
		1, -1, 0, 1,		1 - halfTexel.x, 1 - halfTexel.y,
		1, 1, 0, 1,			1 - halfTexel.x, halfTexel.y,
		-1, -1, 0, 1,		halfTexel.x, 1 - halfTexel.y,
	};

	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);
	
	glGenBuffers(1, &m_vbo);	
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);	
	glBufferData(GL_ARRAY_BUFFER, sizeof(float)* 6 * 6, vertexData, GL_STATIC_DRAW);
	
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(float)* 6, 0);
	
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float)* 6, ((char*)0) + 16);
	
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

std::string APP_Splash::LoadShader(const char *a_filePath)
{
	std::string strShaderCode; //file info holder
	//open shader file
	std::ifstream shaderStream(a_filePath);
	//if that worked ok, load file line by line

	if (shaderStream.is_open())
	{
		std::string Line = "";
		while (std::getline(shaderStream, Line))
		{
			strShaderCode += "\n" + Line;
		}
		shaderStream.close();
	}
	return strShaderCode; //not in use
}
