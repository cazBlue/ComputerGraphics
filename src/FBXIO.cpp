#include <FBXIO.h>

#include <iostream>
#include <fstream>

#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include <stb_image.h>
#include <FBXFile.h>

void FBXIO::WriteObj()
{
	FBXFile* fbx;

	fbx = new FBXFile();

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

	fbx->load(path, fbx->UNITS_METER, true, true, true);


	//mesh


	std::ofstream fout("FBXData.dat", std::ios::out | std::ios::binary);
	if (fout.good()) 
	{

	}

}

void FBXIO::ReadObj()
{

}

bool FBXIO::DoesFileExist(const char *fileName)
{
	//http://stackoverflow.com/questions/12774207/fastest-way-to-check-if-a-file-exist-using-standard-c-c11-c
	std::ifstream infile(fileName);
	return infile.good();	
}