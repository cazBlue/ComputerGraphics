#include <FBXIO.h>

#include <iostream>
#include <fstream>
#include <memory>

#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include <stb_image.h>


FBXIO::FBXIO()
{

}

FBXIO::~FBXIO()
{
	delete m_fbx;
}

void FBXIO::WriteObj()
{	

	m_fbx = new FBXFile();


	std::string strShaderCode; //file info holder --TODO create array of file names
	std::ifstream shaderStream("./assets/fbxToLoad.txt");
	//if that worked ok, load file line by line

	if (shaderStream.is_open())
	{
		std::string Line = "";
		while (std::getline(shaderStream, Line))
		{
			//strShaderCode += "\n" + Line;
			strShaderCode += Line;
		}
		shaderStream.close();
	}

	const char* path = strShaderCode.c_str();

	m_fbx->load(path, m_fbx->UNITS_METER, true, true, true);



	//mesh
	//vertex info
	FBXMeshNode* mesh = m_fbx->getMeshByIndex(0);

	 

	FBXVertex vert = mesh->m_vertices[0];	

	std::ofstream fout("FBXData.dat", std::ios::out | std::ios::binary);
	if (fout.good()) 
	{
		//write position data
		int arraySize = mesh->m_vertices.size();
		//int arraySize = sizeof(mesh->m_vertices) / sizeof(FBXVertex);
		for (int i = 0; i < arraySize; i++) {

			vertLayout curVert;
			curVert.position = mesh->m_vertices[i].position;
			curVert.indices = mesh->m_vertices[i].indices;
			curVert.normal = mesh->m_vertices[i].normal;
			curVert.binormal = mesh->m_vertices[i].binormal;
			curVert.texCoord1 = mesh->m_vertices[i].texCoord1;
			curVert.weights = mesh->m_vertices[i].weights;

			fout.write((char*)&curVert, sizeof(FBXVertex));
		}

		fout.close();
	}		
	

}

void FBXIO::ReadObj()
{
	std::ifstream fin("FBXData.dat", std::ios::in | std::ios::binary);

	FBXMeshNode mesh;

	vertLayout vert_ptr;

	if (fin.good())
	{
		while (!fin.eof() && fin.peek() != EOF) {
			fin.read((char*)&vert_ptr, sizeof(vertLayout));			

			FBXVertex curVert;

			curVert.position =	vert_ptr.position;
			curVert.normal =	vert_ptr.normal;
			curVert.tangent =	vert_ptr.tangent;
			curVert.binormal =	vert_ptr.binormal;
			curVert.indices =	vert_ptr.indices;
			curVert.texCoord1 =	vert_ptr.texCoord1;

			mesh.m_vertices.push_back(curVert);	
			
		}

		fin.close();
	}

	FBXFile fbx;

//	fbx.m_meshes.push_back(&mesh);
	
	
}

bool FBXIO::DoesFileExist(const char *fileName)
{
	//http://stackoverflow.com/questions/12774207/fastest-way-to-check-if-a-file-exist-using-standard-c-c11-c
	std::ifstream infile(fileName);
	return infile.good();	
}