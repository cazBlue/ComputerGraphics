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
	unsigned int posSize = pos.size();
	
	std::vector<float> normals = shapes[0].mesh.normals;
	unsigned int normalsSize = normals.size();
	
	std::vector<unsigned int> indices = shapes[0].mesh.indices;
	unsigned int indicesSize = indices.size();
	
	std::ofstream fout("data.dat", std::ios::out | std::ios::binary);
	if (fout.good()) {
		//write array sizes
		//writepos size
		fout.write((char*)&posSize, sizeof(unsigned int));
		//write normal size
		fout.write((char*)&normalsSize, sizeof(unsigned int));
		//write indice size
		fout.write((char*)&indicesSize, sizeof(unsigned int));

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

	float* f_ptr = new float;
	unsigned int* ui_ptr = new unsigned int;

	std::vector<float>	pos;
	unsigned int posSize = 0;
	std::vector<float>	normals;
	unsigned int normalSize = 0;
	std::vector<int>	indices;
	unsigned int indiceSize = 0;

	if (DoesFileExist("data.dat"))
	{
		//for checking inputs
//		std::vector<tinyobj::shape_t> shapes;
//		std::vector<tinyobj::material_t> materials;
//		std::string err = tinyobj::LoadObj(shapes, materials, "./assets/stanford_objs/bunny.obj");

		std::ifstream fin("data.dat", std::ios::in | std::ios::binary);

		

		if (fin.good()) {
			// read until we get to the end of file
			//read pos size
			if (!fin.eof() && fin.peek() != EOF)
			{
				fin.read((char*)ui_ptr, sizeof(unsigned int));
				posSize = (*ui_ptr);
			}

			//read normal size
			if (!fin.eof() && fin.peek() != EOF)
			{
				fin.read((char*)ui_ptr, sizeof(unsigned int));
				normalSize = (*ui_ptr);
			}

			//read indice size
			if (!fin.eof() && fin.peek() != EOF)
			{
				fin.read((char*)ui_ptr, sizeof(unsigned int));
				indiceSize = (*ui_ptr);
			}

			unsigned int count = 0;

			//pull pos data
			while (!fin.eof() && fin.peek() != EOF && count < posSize) {
				fin.read((char*)f_ptr, sizeof(float));

				pos.push_back((*f_ptr)); //copy the value into the float list
				count++;
			}

			//pull normal data
			while (!fin.eof() && fin.peek() != EOF && count < (posSize + normalSize)) {
				fin.read((char*)f_ptr, sizeof(float));

				normals.push_back((*f_ptr)); //copy the value into the float list
				count++;
			}

			//pull indice data
			while (!fin.eof() && fin.peek() != EOF && count < (posSize + normalSize + indiceSize)) {
				fin.read((char*)ui_ptr, sizeof(unsigned int));

				indices.push_back((*ui_ptr)); //copy the value into the float list
				count++;
			}

			fin.close();
		}
	}
	else
		WriteObj();

	delete f_ptr;
	delete ui_ptr;
}