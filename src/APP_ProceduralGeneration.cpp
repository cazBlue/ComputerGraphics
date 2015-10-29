#include <APP_ProceduralGeneration.h>

APP_Proc_Generation::APP_Proc_Generation()
{

}
APP_Proc_Generation::~APP_Proc_Generation()
{

}

void APP_Proc_Generation::Update(float a_dt)
{
	GameCam->Update(a_dt); //update camera


}

void APP_Proc_Generation::Draw()
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

	Gizmos::draw(GameCam->GetProjectionView());


}

bool APP_Proc_Generation::Start()
{
	Gizmos::create();

	GameCam = new Camera();
	
	//create base grid mesh

	return true; //not being used in this lesson
}

void APP_Proc_Generation::generateMesh(unsigned int rows, unsigned int cols)
{
	//add one to show correct rows/cols
	rows++; 
	cols++;	

	Vertex* aoVertices = new Vertex[rows * cols]; //big ass array to hold all vert positions

	for (unsigned int row = 0; row < rows; ++row)
	{
		for (unsigned int column = 0; column < cols; ++column)
		{			
			aoVertices[row * cols + column].position = vec4((float)column, 0, (float)row, 1); // assign vertex position
			aoVertices[row * cols + column].uv = glm::vec2((float)(column / cols), (float)(row / rows)); //assign UV coords
		}
	}

	// defining index count based off quad count (2 triangles per quad)
	unsigned int indexCount = (rows - 1) * (cols - 1) * 6; //index count is the number of vertices to make triangle NOT the vertices with positions
	unsigned int* auiIndices = new unsigned int[indexCount];
	unsigned int index = 0;
	for (unsigned int r = 0; r < (rows - 1); ++r)
	{
		for (unsigned int c = 0; c < (cols - 1); ++c)
		{
			// triangle 1
			auiIndices[index++] = r * cols + c;
			auiIndices[index++] = (r + 1) * cols + c;
			auiIndices[index++] = (r + 1) * cols + (c + 1);
			// triangle 2
			auiIndices[index++] = r * cols + c;
			auiIndices[index++] = (r + 1) * cols + (c + 1);
			auiIndices[index++] = r * cols + (c + 1);
		}
	}

	createMeshBuffers(auiIndices, aoVertices);

	delete[] auiIndices;
	delete[] aoVertices;
}

void APP_Proc_Generation::createMeshBuffers(unsigned int* auiIndices, Vertex* aoVertices)
{
	//create buffers
}

bool APP_Proc_Generation::Shutdown()
{
	delete GameCam;
	Gizmos::destroy();

	return true; //not being used in this lesson
}