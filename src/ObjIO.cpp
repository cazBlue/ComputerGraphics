#include <ObjIO.h>
#include <tiny_obj_loader.h>

#include <iostream>
#include <fstream>

#include <glm/glm.hpp>
#include <glm/ext.hpp>

using glm::vec3;


void OBJIO::WriteObj() {
	
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	std::string err = tinyobj::LoadObj(shapes, materials, "./assets/stanford_objs/bunny.obj");

	std::vector<float> pos = shapes[0].mesh.positions;
	std::vector<float> normals = shapes[0].mesh.normals;
	std::vector<unsigned int> indices = shapes[0].mesh.indices;
	
	std::ofstream fout("data.dat", std::ios::out | std::ios::binary);
	if (fout.good()) {
		//write array sizes
		//writepos size
		//write normal size
		//write indice size

		//write position data
		int arraySize = pos.size();
		for (int i = 0; i < arraySize; i++) {
			fout.write((char*)&pos[i], sizeof(float));
		}

		//write normal data
		arraySize = normals.size();
		for (int i = 0; i < arraySize; i++) {
			fout.write((char*)&normals[i], sizeof(float));
		}

		//write indice data
		arraySize = indices.size();
		for (int i = 0; i < arraySize; i++) {
			fout.write((char*)&indices[i], sizeof(unsigned int));
		}

		fout.close();
	}
}

bool OBJIO::DoesFileExist(const char *fileName)
{
	//http://stackoverflow.com/questions/12774207/fastest-way-to-check-if-a-file-exist-using-standard-c-c11-c
	std::ifstream infile(fileName);
	return infile.good();
}

void OBJIO::ReadObj() {	
	//vec3* myVec = new vec3();

	float* ptr = new float;

	std::vector<float>	pos;
	std::vector<float>	normals;
	std::vector<int>	indices;

	if (DoesFileExist("data.dat"))
	{
		//for checking inputs
		std::vector<tinyobj::shape_t> shapes;
		std::vector<tinyobj::material_t> materials;
		std::string err = tinyobj::LoadObj(shapes, materials, "./assets/stanford_objs/bunny.obj");

		std::ifstream fin("data.dat", std::ios::in | std::ios::binary);

		int count = 0;

		if (fin.good()) {
			// read until we get to the end of file

			//read pos size

			//read normal size

			//read indice size


			while (!fin.eof() && fin.peek() != EOF) {
				fin.read((char*)ptr, sizeof(float));

				pos.push_back((*ptr)); //copy the value into the vector				
				count++;
			}



			fin.close();
		}
	}
	else
		WriteObj();

	delete ptr;
}