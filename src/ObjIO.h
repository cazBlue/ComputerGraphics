#ifndef OBJIO_H
#define OBJIO_H

#include <tiny_obj_loader.h>
#include <iostream>
#include <fstream>

class OBJIO
{
public:

	void WriteObj(const char* basePath, const char* fileName, std::vector<tinyobj::shape_t>* shapes);
	void ReadObj(const char* basePath, const char* fileName, std::vector<tinyobj::shape_t>* shapes);
	bool DoesFileExist(const char *fileName);

};



#endif OBJIO_H