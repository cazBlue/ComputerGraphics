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
	
	
	bool DoesFileExist(const char *fileName);
	bool HasAnimation = false;

	void writeString(std::ofstream* a_fout, FBXTexture* a_tex);

	void readString(std::ifstream* a_fin, std::string& a_string);

	const char* readImgData(std::ifstream* a_fin);

	struct textureLayout
	{
		std::string		name;
		std::string		path;
		unsigned int	handle;
		const char*		data;
		int				res;
		int				width;
		int				height;
		int				format;
	};

	//set in fbx.h
//	enum Offsets
//	{
//		PositionOffset = 0,
//		ColourOffset = PositionOffset + sizeof(glm::vec4),
//		NormalOffset = ColourOffset + sizeof(glm::vec4),
//		TangentOffset = NormalOffset + sizeof(glm::vec4),
//		BiNormalOffset = TangentOffset + sizeof(glm::vec4),
//		IndicesOffset = BiNormalOffset + sizeof(glm::vec4),
//		WeightsOffset = IndicesOffset + sizeof(glm::vec4),
//		TexCoord1Offset = WeightsOffset + sizeof(glm::vec4),
//		TexCoord2Offset = TexCoord1Offset + sizeof(glm::vec2),
//	};

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

	struct fbxInternals
	{
		textureLayout m_dif, m_normal, m_spec;
		std::vector<unsigned int> m_indices;
		std::vector<vertLayout> verts;

		unsigned int* glData;
	};

	FBXFile* m_fbx;

	void ReadObj(fbxInternals& a_fbxInteral, const char* fileName, const char* path);
	void WriteObj(fbxInternals& a_fbxInteral, const char* fileName, const char* path);

	void createFBX(fbxInternals a_fbxInternal, FBXFile& a_file);
};



#endif FBXIO_H