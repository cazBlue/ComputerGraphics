#include <APP_ObjLoader.h>
#include <iostream>
#include <fstream>

APP_OBJLoader::APP_OBJLoader()
{

}
APP_OBJLoader::~APP_OBJLoader()
{

}

void APP_OBJLoader::createOpenGLBuffers(std::vector<tinyobj::shape_t>& shapes)
{
//	m_gl_info.resize(shapes.size());
//
//	for (unsigned int mesh_index = 0; mesh_index < shapes.size(); ++mesh_index)
//	{
//		glGenVertexArrays(1, &m_gl_info[mesh_index].m_VAO);
//		glGenBuffers(1, &m_gl_info[mesh_index].m_VBO);
//		glGenBuffers(1, &m_gl_info[mesh_index].m_IBO);
//		glBindVertexArray(m_gl_info[mesh_index].m_VAO);
//		unsigned int float_count = shapes[mesh_index].mesh.positions.size();
//		float_count += shapes[mesh_index].mesh.normals.size();
//		float_count += shapes[mesh_index].mesh.texcoords.size();
//		std::vector<float> vertex_data;
//		vertex_data.reserve(float_count);
//		vertex_data.insert(vertex_data.end(), shapes[mesh_index].mesh.positions.begin(), shapes[mesh_index].mesh.positions.end());
//		vertex_data.insert(vertex_data.end(), shapes[mesh_index].mesh.normals.begin(), shapes[mesh_index].mesh.normals.end());
//		m_gl_info[mesh_index].m_index_count = shapes[mesh_index].mesh.indices.size();
//		glBindBuffer(GL_ARRAY_BUFFER, m_gl_info[mesh_index].m_VBO);
//		glBufferData(GL_ARRAY_BUFFER, vertex_data.size() * sizeof(float), vertex_data.data(), GL_STATIC_DRAW);
//		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_gl_info[mesh_index].m_IBO);
//		glBufferData(GL_ELEMENT_ARRAY_BUFFER, shapes[mesh_index].mesh.indices.size() * sizeof(unsigned int), shapes[mesh_index].mesh.indices.data(), GL_STATIC_DRAW);
//		glEnableVertexAttribArray(0); //position
//		glEnableVertexAttribArray(1); //normal data (colour information
//		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
//		glVertexAttribPointer(1, 3, GL_FLOAT, GL_TRUE, 0, (void*)(sizeof(float)*shapes[mesh_index].mesh.positions.size()));
//		glBindVertexArray(0);
//		glBindBuffer(GL_ARRAY_BUFFER, 0);
//		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
//	}
}

void APP_OBJLoader::Update(float a_dt)
{
	GameCam->Update(a_dt); //update camera
}

void APP_OBJLoader::Draw()
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

	drawObj();

	Gizmos::draw(GameCam->GetProjectionView());
}

std::string APP_OBJLoader::LoadShader(const char *a_filePath)
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

	return strShaderCode;
}

void APP_OBJLoader::createShaders()
{
	// create shaders
	const char* vsSource = nullptr;
	std::string vsResult = LoadShader("./assets/shaders/VertexShader.glsl");
	vsSource = vsResult.c_str();

	const char* fsSource = nullptr;
	std::string fsResult = LoadShader("./assets/shaders/FragShader.glsl");
	fsSource = fsResult.c_str();

	int success = GL_FALSE;
	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

	//glBindAttributeLocation

	glShaderSource(vertexShader, 1, (const char**)&vsSource, 0);
	glCompileShader(vertexShader);
	glShaderSource(fragmentShader, 1, (const char**)&fsSource, 0);
	glCompileShader(fragmentShader);
	m_programID = glCreateProgram();
	glAttachShader(m_programID, vertexShader);
	glAttachShader(m_programID, fragmentShader);
	glLinkProgram(m_programID);

	glGetProgramiv(m_programID, GL_LINK_STATUS, &success);
	if (success == GL_FALSE) {
		int infoLogLength = 0;
		glGetProgramiv(m_programID, GL_INFO_LOG_LENGTH, &infoLogLength);
		char* infoLog = new char[infoLogLength];
		glGetProgramInfoLog(m_programID, infoLogLength, 0, infoLog);
		printf("Error: Failed to link shader program!\n");
		printf("%s\n", infoLog);
		delete[] infoLog;
	}
	glDeleteShader(fragmentShader);
	glDeleteShader(vertexShader);
}

void APP_OBJLoader::drawObj()
{
	glUseProgram(m_programID);
	int view_proj_uniform = glGetUniformLocation(m_programID, "ProjectionView");
	glUniformMatrix4fv(view_proj_uniform, 1, GL_FALSE, glm::value_ptr(GameCam->GetProjectionView()));
	for (unsigned int i = 0; i < importCtrl->m_gl_info.size(); ++i)
	{
		glBindVertexArray(importCtrl->m_gl_info[i].m_VAO);
		glDrawElements(GL_TRIANGLES, importCtrl->m_gl_info[i].m_index_count, GL_UNSIGNED_INT, 0);
	}
}

bool APP_OBJLoader::Start()
{
	m_appName = "OBJ loader";

	Gizmos::create();
	GameCam = new Camera();	
		
	//std::string err = tinyobj::LoadObj(shapes, materials, "./assets/stanford_objs/bunny.obj");	
	//loads obj from file and creates a binary version, loads from that if it exists

//	objCtrl = new OBJIO();
//
//	objCtrl->ReadObj("./assets/stanford_objs/bunny.obj", "OBJbunny.dat", &shapes);
	

	

	createShaders(); //created the program and loads shaders

	//createOpenGLBuffers(shapes);

	isLoaded = true;

	return true; //not being used in this lesson
}

void APP_OBJLoader::ClearMenu()
{
	TwDeleteBar(m_bar); //reset the gui
}

void APP_OBJLoader::CreateGui()
{
	m_bar = TwNewBar("ObjLoader");

	TwDefine(" ObjLoader position='10 10' "); // move bar to position (10, 10)
	TwDefine(" ObjLoader size='430 320' "); // resize bar	
	TwDefine(" ObjLoader color='128 128 128' alpha=32 ");   // semi-transparent blue bar
	TwDefine(" ObjLoader resizable=false "); // mybar cannot be resized


	TwAddButton(m_bar, "label_01", NULL, NULL, "label='loads obj and shows normals'"); //show as label		
	TwAddButton(m_bar, "mainMenu", Callback, this, "label='main menu'"); //show as button	

	//resets the camera when app re-opens
	GameCam->SetPosition(vec3(-9.8, 3.65, 13.46));
	GameCam->SetFront(vec3(.483, .0558, -.87));
	GameCam->SetPitchYaw(3.2, -61);

	GameCam->SetMouseSnapToCurrent();
	GameCam->ManualSnap();
}

bool APP_OBJLoader::Shutdown()
{
	delete GameCam;	
	Gizmos::destroy();

	return true; //not being used in this lesson
}