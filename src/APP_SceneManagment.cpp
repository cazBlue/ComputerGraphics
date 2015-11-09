#include <APP_SceneManagment.h>

#include <iostream>
#include <fstream>

class QuadZone
{
public:
	unsigned int gridSize;
	
	void createChildren(unsigned int depth)
	{
		//recusively build children layers
		for (unsigned int i = 0; i < depth; ++i)
		{
			createChildren(depth - 1);
		}
	}

	QuadZone* parent;
	std::list <QuadZone *> children;
};

class QuadTree 
{
public:
	unsigned int gridDepth = 2;
	unsigned int gridSize = 128; //divided by 4 each time
	glm::vec2 centre = glm::vec2(0, 0);
	std::list <QuadZone *> firstLayer; //first layer of the quad tree

	void createGrid()
	{
		//create a grid with

		for (unsigned int i = 0; i < gridDepth; ++i)
		{
			QuadZone* newZone = new QuadZone();
			newZone->gridSize = gridSize;
			newZone->parent = nullptr;
			newZone->createChildren(gridDepth);
			
			firstLayer.push_back(newZone);
		}

	}

	void RenderGrid()
	{

	}

	void locateMatch() //finds which grid the camera is in and renders an object in it
	{

		//if match is found render an object
	}

	void renderObj() //used is a match is found an places an object in the middle
	{

	}
};

class AABB {
public: 
	AABB() { reset(); } 
	~AABB() {}
	
	void reset() 
	{ 
		min.x = min.y = min.z = 1e37f; 
		max.x = max.y = max.z = -1e37f; 
	} 
	
	void fit(const std::vector<glm::vec3>& points) 
	{ 
		for (auto& p : points) 
		{ 
			if (p.x < min.x) min.x = p.x; 
			if (p.y < min.y) min.y = p.y; 
			if (p.z < min.z) min.z = p.z; 
			if (p.x > max.x) max.x = p.x; 
			if (p.y > max.y) max.y = p.y; 
			if (p.z > max.z) max.z = p.z; 
		} 
	}
	
	glm::vec3 min, max;
};

void BoundingSphere::fit(const std::vector<glm::vec3>& points) {
	glm::vec3 min(1e37f), max(-1e37f);
	for (auto& p : points) 
	{ 
		if (p.x < min.x) min.x = p.x; 
		if (p.y < min.y) min.y = p.y; 
		if (p.z < min.z) min.z = p.z; 
		if (p.x > max.x) max.x = p.x; 
		if (p.y > max.y) max.y = p.y; 
		if (p.z > max.z) max.z = p.z; 
	}
	
	centre = (min + max) * 0.5f; 
	radius = glm::distance(min, centre);
}


APP_SCENEMANAGE::APP_SCENEMANAGE()
{

}
APP_SCENEMANAGE::~APP_SCENEMANAGE()
{
	delete quadTree;
}

void APP_SCENEMANAGE::Update(float a_dt)
{
	GameCam->Update(a_dt); //update camera


}

void APP_SCENEMANAGE::Draw()
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

	BoundingSphere sphere; 
	AABB boundingBox;

//	boundingBox.fit(vertex_data);

//	Gizmos::addAABB(vec3(0, 0, 0), vec3(10, 10, 10), vec4(0, 0, 0, 1), nullptr);

//	boundingBox.fit(Gizmos::);

	vec3 movingPoint = vec3(0, cosf((float)glfwGetTime()) + 1, 0); 
	sphere.centre = movingPoint;
	sphere.radius = 0.5f;
	vec4 plane(0, 1, 0, -1);
	float d = glm::dot(vec3(plane), sphere.centre) + plane.w;
	
	Gizmos::addAABB(movingPoint, vec3(0.5f, 0.5f, 0.5f), vec4(0, 0, 1, 1), nullptr);
	
	//comparing a square AABB shape of center 0 to .5 giving total size of 1 square
	//against a plane

	//AABB/plane collision
//	vec4 planeColour(1, 1, 0, 1); //set the plane to yellow
//	if (fb_xmin > ob_xmax || fb_xmax < ob_xmin ||
//		fb_ymin > ob_ymax || fb_ymax < ob_ymin ||
//		fb_zmin > ob_zmax || fb_zmax < ob_zmin)
//
//		return (OUTSIDE);
//
//	else if (fb_xmin < ob_xmin && fb_xmax > ob_xmax &&
//		fb_ymin < ob_ymin && fb_ymax > ob_ymax &&
//		fb_zmin < ob_zmin && fb_zmax > ob_zmax)
//		return (INSIDE);
//	else
//		return(INTERSECT);

	//sphere/plane collision
	vec4 planeColour(1, 1, 0, 1); //set the plane to yellow
	if (d > sphere.radius) 
		planeColour = vec4(0, 1, 0, 1); //set the plane to green
	else if (d < -sphere.radius) 
		planeColour = vec4(1, 0, 0, 1); // set the plane to red
	Gizmos::addTri(vec3(4, 1, 4), vec3(-4, 1, -4), vec3(-4, 1, 4), planeColour); Gizmos::addTri(vec3(4, 1, 4), vec3(4, 1, -4), vec3(-4, 1, -4), planeColour);

	vec4 planes[6]; 
	getFrustumPlanes(GameCam->GetProjectionView(), planes);

	for (int i = 0; i < 6; i++) {
		float d = glm::dot(vec3(planes[i]), sphere.centre) + planes[i].w;
		if (d < -sphere.radius) 
		{ 
			//printf("Behind, don't render it!\n"); 
			break; 
		}
		else if (d < sphere.radius) { 
			//printf("Touching, we still need to render it!\n");
			Gizmos::addSphere(sphere.centre, sphere.radius, 8, 8, vec4(1, 0, 1, 1));
		}
		else 
		{ 
			//printf("Front, fully visible so render it!\n"); 
			Gizmos::addSphere(sphere.centre, sphere.radius, 8, 8, vec4(1, 0, 1, 1));
		}	
	}

	Gizmos::draw(GameCam->GetProjectionView());
}

void APP_SCENEMANAGE::ClearMenu()
{
	TwDeleteBar(m_bar); //reset the gui
}

void APP_SCENEMANAGE::CreateGui()
{
	m_bar = TwNewBar("SceneManagment");

	TwDefine(" SceneManagment position='10 10' "); // move bar to position (10, 10)
	TwDefine(" SceneManagment size='430 320' "); // resize bar	
	TwDefine(" SceneManagment color='128 128 128' alpha=32 ");   // semi-transparent blue bar
	TwDefine(" SceneManagment resizable=false "); // mybar cannot be resized


	TwAddButton(m_bar, "label_01", NULL, NULL, "label='Scene managment - green when in front of plane'"); //show as label		
	TwAddButton(m_bar, "label_02", NULL, NULL, "label='yellow when intersecting and red when behind'"); //show as label		
//	TwAddButton(m_bar, "label_03", NULL, NULL, "label='Scene managment - green when in front of plane'"); //show as label		
	TwAddButton(m_bar, "mainMenu", Callback, this, "label='main menu'"); //show as button				
}

bool APP_SCENEMANAGE::Start()
{
	m_appName = "Scene Managment";

	Gizmos::create();

	GameCam = new Camera();

//	quadTree = new QuadTree;
//
//	quadTree->createGrid();

//	printf("loading object, this can take a while!");
//	std::string err = tinyobj::LoadObj(shapes, materials, "./assets/stanford_objs/bunny.obj");
//
//	printf("creating shaders");
//	createShaders(); //created the program and loads shaders
//
//	printf("creating buffers");
//	createOpenGLBuffers(shapes);

	isLoaded = true;

	return true; //not being used in this lesson
}

bool APP_SCENEMANAGE::Shutdown()
{
	delete GameCam;
	Gizmos::destroy();

	return true; //not being used in this lesson
}

void APP_SCENEMANAGE::getFrustumPlanes(const glm::mat4& transform, glm::vec4* planes)
{ 
	// right side
	planes[0] = vec4( transform[0][3] - transform[1][0], 
					transform[1][3] - transform[1][0], 
					transform[2][3] - transform[2][0], 
					transform[3][3] - transform[3][0]);
	// left side 
	planes[1] = vec4( transform[0][3] + transform[0][0], 
					transform[1][3] + transform[1][0], 
					transform[2][3] + transform[2][0], 
					transform[3][3] + transform[3][0]);

	// top 
	planes[2] = vec4( transform[0][3] - transform[0][1], 
					transform[1][3] - transform[1][1], 
					transform[2][3] - transform[2][1], 
					transform[3][3] - transform[3][1]);

	// bottom 
	planes[3] = vec4( transform[0][3] + transform[0][1], 
					transform[1][3] + transform[1][1], 
					transform[2][3] + transform[2][1], 
					transform[3][3] + transform[3][1]); 
	
	// far 
	planes[4] = vec4( transform[0][3] - transform[0][2], 
				transform[1][3] - transform[1][2], 
				transform[2][3] - transform[2][2], 
				transform[3][3] - transform[3][2]); 
	
	// near 
	planes[5] = vec4( transform[0][3] + transform[0][2], 
				transform[1][3] + transform[1][2], 
				transform[2][3] + transform[2][2], 
				transform[3][3] + transform[3][2]);


	for (int i = 0; i < 6; i++) 
		planes[i] = glm::normalize(planes[i]);
}

void APP_SCENEMANAGE::createOpenGLBuffers(std::vector<tinyobj::shape_t>& shapes)
{
	m_gl_info.resize(shapes.size());

	for (unsigned int mesh_index = 0; mesh_index < shapes.size(); ++mesh_index)
	{
		glGenVertexArrays(1, &m_gl_info[mesh_index].m_VAO);
		glGenBuffers(1, &m_gl_info[mesh_index].m_VBO);
		glGenBuffers(1, &m_gl_info[mesh_index].m_IBO);
		glBindVertexArray(m_gl_info[mesh_index].m_VAO);
		unsigned int float_count = shapes[mesh_index].mesh.positions.size();
		float_count += shapes[mesh_index].mesh.normals.size();
		float_count += shapes[mesh_index].mesh.texcoords.size();
		std::vector<float> vertex_data;
		vertex_data.reserve(float_count);
		vertex_data.insert(vertex_data.end(), shapes[mesh_index].mesh.positions.begin(), shapes[mesh_index].mesh.positions.end());
		vertex_data.insert(vertex_data.end(), shapes[mesh_index].mesh.normals.begin(), shapes[mesh_index].mesh.normals.end());
		m_gl_info[mesh_index].m_index_count = shapes[mesh_index].mesh.indices.size();
		glBindBuffer(GL_ARRAY_BUFFER, m_gl_info[mesh_index].m_VBO);
		glBufferData(GL_ARRAY_BUFFER, vertex_data.size() * sizeof(float), vertex_data.data(), GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_gl_info[mesh_index].m_IBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, shapes[mesh_index].mesh.indices.size() * sizeof(unsigned int), shapes[mesh_index].mesh.indices.data(), GL_STATIC_DRAW);
		glEnableVertexAttribArray(0); //position
		glEnableVertexAttribArray(1); //normal data (colour information
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_TRUE, 0, (void*)(sizeof(float)*shapes[mesh_index].mesh.positions.size()));
		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
}

std::string APP_SCENEMANAGE::LoadShader(const char *a_filePath)
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

void APP_SCENEMANAGE::drawObj()
{
	glUseProgram(m_programID);
	int view_proj_uniform = glGetUniformLocation(m_programID, "ProjectionView");
	glUniformMatrix4fv(view_proj_uniform, 1, GL_FALSE, glm::value_ptr(GameCam->GetProjectionView()));
	for (unsigned int i = 0; i < m_gl_info.size(); ++i)
	{
		glBindVertexArray(m_gl_info[i].m_VAO);
		glDrawElements(GL_TRIANGLES, m_gl_info[i].m_index_count, GL_UNSIGNED_INT, 0);
	}
}

void APP_SCENEMANAGE::createShaders()
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