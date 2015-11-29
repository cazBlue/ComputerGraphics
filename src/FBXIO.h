#ifndef FBXIO_H
#define FBXIO_H

#include "glm/glm.hpp"
#include <FBXFile.h>

class FBXIO
{
public:

	//embeds images into single binary file allow fast load with no image loading needed

	FBXIO();
	~FBXIO();

	void WriteObj();
	void ReadObj();
	bool DoesFileExist(const char *fileName);
	
	struct textureLayout
	{
		std::string		name;
		std::string		path;
		unsigned int	handle;
		unsigned char*	data;
		int				width;
		int				height;
		int				format;
	};

	struct vertLayout
	{
		glm::vec4	position;		
		glm::vec4	normal;
		glm::vec4	tangent;
		glm::vec4	binormal;
		glm::vec4	indices;
		glm::vec4	weights;
		glm::vec2	texCoord1;		
	};

	FBXFile* m_fbx;

};



#endif FBXIO_H