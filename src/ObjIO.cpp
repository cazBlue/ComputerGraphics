#include <ObjIO.h>

#include <iostream>
#include <fstream>

#include <glm/glm.hpp>
#include <glm/ext.hpp>

using glm::vec3;


void OBJIO::WriteObj() {


	vec3 myVecArray[] = { { 0, 1, 2 }, { 1, 2, 3 }, { 4, 5, 6 }, { 32, 1, 98 } };
	int arraySize = sizeof(myVecArray) / sizeof(vec3);
	//fill in the array here
	std::ofstream fout("data.dat", std::ios::out | std::ios::binary);
	if (fout.good()) {
		for (int i = 0; i < arraySize; i++) {
			fout.write((char*)&myVecArray[i], sizeof(vec3));
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
	vec3* myVec = new vec3();
	if (DoesFileExist("data.dat"))
	{

		std::ifstream fin("data.dat", std::ios::in | std::ios::binary);

		int count = 0;

		if (fin.good()) {
			// read until we get to the end of file
			while (!fin.eof() && fin.peek() != EOF) {
				fin.read((char*)&myVec[count], sizeof(vec3));
				std::cout << myVec[count].x << " " << myVec[count].y << " " << myVec[count].z << std::endl;
				count++;
			}
			fin.close();
		}
	}
	else
		WriteObj();
}