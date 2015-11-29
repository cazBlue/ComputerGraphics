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

	std::map<std::string, std::string>::iterator iter;

//	for (iter = m_fbx->m_textures.begin(); iter != m_fbx->m_textures.end(); ++iter) {
//	}
//	
//	for (int i = 0; i < m_fbx->m_textures.size(); ++i)
//	{
//		printf(m_fbx->m_textures);
//	}

	//mesh
	//vertex info
	FBXMeshNode* mesh = m_fbx->getMeshByIndex(0);

	FBXMaterial* mat = mesh->m_material;

	FBXTexture* diffuesTex = mat->textures[FBXMaterial::TextureTypes::DiffuseTexture];
	FBXTexture* normalTex = mat->textures[FBXMaterial::TextureTypes::NormalTexture];
	FBXTexture* specTex = mat->textures[FBXMaterial::TextureTypes::SpecularTexture];

	
	std::ofstream fout("FBXData.dat", std::ios::out | std::ios::binary);
	if (fout.good()) 
	{
		//vertex data of fbxVertex
		int vertex_arraySize = mesh->m_vertices.size();		
		fout.write((char*)&vertex_arraySize, sizeof(unsigned int)); //store vertex array size

		//indice data
		int indice_arraySize = mesh->m_indices.size();
		fout.write((char*)&indice_arraySize, sizeof(unsigned int)); //store indice array size

		//write vertex data
		for (int i = 0; i < vertex_arraySize; i++) 
		{
			vertLayout curVert;
			curVert.position = mesh->m_vertices[i].position;
			curVert.indices = mesh->m_vertices[i].indices;
			curVert.normal = mesh->m_vertices[i].normal;
			curVert.binormal = mesh->m_vertices[i].binormal;
			curVert.texCoord1 = mesh->m_vertices[i].texCoord1;
			curVert.weights = mesh->m_vertices[i].weights;

			fout.write((char*)&curVert, sizeof(vertLayout));
		}

		//write indice data
		for (int i = 0; i < indice_arraySize; i++)
		{
			fout.write((char*)&mesh->m_indices[i], sizeof(unsigned int));
		}

		//diffuse data
		FBXTexture* diffuesTex = mat->textures[FBXMaterial::TextureTypes::DiffuseTexture];

		textureLayout diffuseTexLayout;

		//write the image name
		size_t nameCount = strlen(diffuesTex->name.c_str());
		const char* name = diffuesTex->name.c_str();
		fout.write((char*)&nameCount, sizeof(unsigned int)); //write name length
		//http://www.cplusplus.com/forum/general/51954/
		fout.write(name, nameCount + 1); //write path +1 for null character

		//write image data
		std::string str;
		str.append(reinterpret_cast<const char*>(diffuesTex->data));
		//http://stackoverflow.com/questions/658913/c-style-cast-from-unsigned-char-to-const-char

		size_t dataCount = strlen(str.c_str());
		const char* data = str.c_str();
		fout.write((char*)&dataCount, sizeof(unsigned int)); //write path
		fout.write(data, dataCount + 1); //write path +1 for null character

//		diffuseTexLayout.name		= diffuesTex->name;
//		diffuseTexLayout.path		= diffuesTex->path;
//		diffuseTexLayout.handle		= diffuesTex->handle;
//		diffuseTexLayout.data		= diffuesTex->data;
//		diffuseTexLayout.width		= diffuesTex->width;
//		diffuseTexLayout.height		= diffuesTex->height;
//		diffuseTexLayout.format		= diffuesTex->format;
//
//		fout.write((char*)&diffuseTexLayout, sizeof(textureLayout)); //store indice array size

		fout.close();
	}		
	

}

void FBXIO::ReadObj()
{
	std::ifstream fin("FBXData.dat", std::ios::in | std::ios::binary);

	FBXMeshNode mesh;

	vertLayout vert_ptr;

	textureLayout tex_ptr;

	if (fin.good())
	{
		unsigned int vertexSize = 0;
		unsigned int indiceSize = 0;

		unsigned int* ui_ptr = new unsigned int;

		//read vertex size
		if (!fin.eof() && fin.peek() != EOF)
		{
			fin.read((char*)ui_ptr, sizeof(unsigned int));
			vertexSize = (*ui_ptr);
		}

		//read indice size
		if (!fin.eof() && fin.peek() != EOF)
		{
			fin.read((char*)ui_ptr, sizeof(unsigned int));
			indiceSize = (*ui_ptr);
		}

		unsigned int count = 0;

		//read vertex data
		while (!fin.eof() && fin.peek() != EOF && count < vertexSize) 
		{
			fin.read((char*)&vert_ptr, sizeof(vertLayout));			

			FBXVertex curVert;

			curVert.position =	vert_ptr.position;
			curVert.normal =	vert_ptr.normal;
			curVert.tangent =	vert_ptr.tangent;
			curVert.binormal =	vert_ptr.binormal;
			curVert.indices =	vert_ptr.indices;
			curVert.texCoord1 =	vert_ptr.texCoord1;

			mesh.m_vertices.push_back(curVert);	
			
			count++;
		}

		//read indice data
		while (!fin.eof() && fin.peek() != EOF && count < vertexSize + indiceSize) 
		{
			unsigned int indice;

			fin.read((char*)&indice, sizeof(unsigned int));	
			mesh.m_indices.push_back(indice);

			count++;
		}

		unsigned int nameSize = 0;

		//read name size
		if (!fin.eof() && fin.peek() != EOF)
		{
			fin.read((char*)ui_ptr, sizeof(unsigned int));
			nameSize = (*ui_ptr);
		}

		//get the image name
		char* name = new char[nameSize];

		if (!fin.eof() && fin.peek() != EOF)
		{
			fin.read(name, nameSize + 1);			
		}
		//convert image name back to string
		std::string stName = "";
		const char* c = name;
		stName.append(c);

		//read image data size
		unsigned int dataSize = 0;

		//read data size
		if (!fin.eof() && fin.peek() != EOF)
		{
			fin.read((char*)ui_ptr, sizeof(unsigned int));
			dataSize = (*ui_ptr);
		}

		//get the image data
		char* data = new char[dataSize];
		if (!fin.eof() && fin.peek() != EOF)
		{
			fin.read(data, dataSize + 1);
		}

		const char* const_data = data;


//		//read diffuse texture data
//		FBXTexture diffuesTex;
//		if (!fin.eof() && fin.peek() != EOF)
//		{
//			fin.read((char*)&tex_ptr, sizeof(textureLayout));
//			
//			diffuesTex.name		= tex_ptr.name;
//			diffuesTex.path		= tex_ptr.path;
//			diffuesTex.handle	= tex_ptr.handle;
//			diffuesTex.data		= tex_ptr.data;
//			diffuesTex.width	= tex_ptr.width;
//			diffuesTex.height	= tex_ptr.height;
//			diffuesTex.format	= tex_ptr.format;
//			
//			
//
//		}

		


		delete ui_ptr;
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