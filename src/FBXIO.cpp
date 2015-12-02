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

void FBXIO::WriteObj(fbxInternals& a_fbxInteral, const char* fileName, const char* path)
{	

	m_fbx = new FBXFile();

	m_fbx->load(path, m_fbx->UNITS_METER, true, true, true);

	std::map<std::string, std::string>::iterator iter;

	//mesh
	//vertex info
	FBXMeshNode* mesh = m_fbx->getMeshByIndex(0);

	FBXMaterial* mat = mesh->m_material;

	FBXTexture* diffuesTex = mat->textures[FBXMaterial::TextureTypes::DiffuseTexture];
	FBXTexture* normalTex = mat->textures[FBXMaterial::TextureTypes::NormalTexture];
	FBXTexture* specTex = mat->textures[FBXMaterial::TextureTypes::SpecularTexture];

	
	std::ofstream fout(fileName, std::ios::out | std::ios::binary);
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
			curVert.texCoord2 = mesh->m_vertices[i].texCoord2;
			curVert.colour = mesh->m_vertices[i].colour;
			curVert.tangent = mesh->m_vertices[i].tangent;


			fout.write((char*)&curVert, sizeof(vertLayout));
		}

		//write indice data
		for (int i = 0; i < indice_arraySize; i++)
		{
			fout.write((char*)&mesh->m_indices[i], sizeof(unsigned int));
		}

		//write path, name and data for needed maps

		FBXTexture* tex = mat->textures[FBXMaterial::TextureTypes::DiffuseTexture];
		if (tex)
			writeString(&fout, mat->textures[FBXMaterial::TextureTypes::DiffuseTexture]);	//diffuse data		
		
		tex = mat->textures[FBXMaterial::TextureTypes::NormalTexture];
		if (tex)
			writeString(&fout, mat->textures[FBXMaterial::TextureTypes::NormalTexture]);	//normal data
		
		tex = mat->textures[FBXMaterial::TextureTypes::SpecularTexture];
		if (tex)
		writeString(&fout, mat->textures[FBXMaterial::TextureTypes::SpecularTexture]);	//spec data

		if (HasAnimation)
			return; //write animation data out

		fout.close();
	}			
}

void FBXIO::ReadObj(fbxInternals& a_fbxInteral, const char* fileName, const char* path)
{
	m_fbx = new FBXFile();

	//write the obj if it doesn't exist, it is then read back in.. pretty slow!
	if (!DoesFileExist(fileName))
	{
		WriteObj(a_fbxInteral, fileName, path);
	}

	std::ifstream fin(fileName, std::ios::in | std::ios::binary);

	fbxInternals fbxObj;

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

			curVert.position = vert_ptr.position;
			curVert.indices = vert_ptr.indices;
			curVert.normal = vert_ptr.normal;
			curVert.tangent = vert_ptr.tangent;
			curVert.binormal = vert_ptr.binormal;			
			curVert.texCoord1 = vert_ptr.texCoord1;
			curVert.texCoord2 = vert_ptr.texCoord2;
			curVert.tangent = vert_ptr.tangent;
			curVert.colour = vert_ptr.colour;

			//			mesh.m_vertices.push_back(curVert);	
			fbxObj.verts.push_back(vert_ptr);

			count++;
		}

		//read indice data
		while (!fin.eof() && fin.peek() != EOF && count < vertexSize + indiceSize)
		{
			unsigned int indice;

			fin.read((char*)&indice, sizeof(unsigned int));
			//			mesh.m_indices.push_back(indice);
			fbxObj.m_indices.push_back(indice);

			count++;
		}

		textureLayout diffuseMap;

		readString(&fin, diffuseMap.name);
		readString(&fin, diffuseMap.path);
		//read res
		if (!fin.eof() && fin.peek() != EOF)
		{
			fin.read((char*)ui_ptr, sizeof(unsigned int));
			diffuseMap.res = (*ui_ptr);
		}
		//diffuseMap.data = readImgData(&fin);

		textureLayout normalMap;

		readString(&fin, normalMap.name);
		readString(&fin, normalMap.path);
		//read res
		if (!fin.eof() && fin.peek() != EOF)
		{
			fin.read((char*)ui_ptr, sizeof(unsigned int));
			normalMap.res = (*ui_ptr);
		}
		//normalMap.data = readImgData(&fin);

		textureLayout specMap;

		readString(&fin, specMap.name);
		readString(&fin, specMap.path);
		//read res
		if (!fin.eof() && fin.peek() != EOF)
		{
			fin.read((char*)ui_ptr, sizeof(unsigned int));
			specMap.res = (*ui_ptr);
		}
		//specMap.data = readImgData(&fin);


		if (HasAnimation)
		{
			return; //read animation data in
		}

		fbxObj.m_dif = diffuseMap;
		fbxObj.m_normal = normalMap;
		fbxObj.m_spec = specMap;


		delete ui_ptr;
		fin.close();
	}

	FBXFile fbx;

	FBXMeshNode* mesh = new FBXMeshNode();



	a_fbxInteral = fbxObj; //set 

	//fbx.m_meshes.push_back();	
}

const char* FBXIO::readImgData(std::ifstream* a_fin)
{
	//read image data size
	unsigned int dataSize = 0;
	unsigned int* ui_ptr = new unsigned int;

	//read data size
	if (!a_fin->eof() && a_fin->peek() != EOF)
	{
		a_fin->read((char*)ui_ptr, sizeof(unsigned int));
		dataSize = (*ui_ptr);
	}

	//get the image data
	char* data = new char[dataSize];
	if (!a_fin->eof() && a_fin->peek() != EOF)
	{
		a_fin->read(data, dataSize + 1);
	}

	delete ui_ptr;

	return data;
}

void FBXIO::readString(std::ifstream* a_fin, std::string& a_string)
{
	unsigned int size = 0;
	unsigned int* ui_ptr = new unsigned int;

	//read name size
	if (!a_fin->eof() && a_fin->peek() != EOF)
	{
		a_fin->read((char*)ui_ptr, sizeof(unsigned int));
		size = (*ui_ptr);
	}

	//get the image name
	char* rString = new char[size];

	if (!a_fin->eof() && a_fin->peek() != EOF)
	{
		a_fin->read(rString, size + 1);
	}
	//convert image name back to string
	std::string stName = "";
	const char* c = rString;
	stName.append(c);

	a_string = stName;

	delete ui_ptr;
}


void FBXIO::writeString(std::ofstream* a_fout, FBXTexture* a_tex)
{
	//write the image name
	size_t count = strlen(a_tex->name.c_str());
	const char* name = a_tex->name.c_str();
	a_fout->write((char*)&count, sizeof(unsigned int)); //write name length
	//http://www.cplusplus.com/forum/general/51954/
	a_fout->write(name, count + 1); //write path +1 for null character

	//write the image path
	count = strlen(a_tex->path.c_str());
	const char* path = a_tex->path.c_str();
	a_fout->write((char*)&count, sizeof(unsigned int)); //write name length
	//http://www.cplusplus.com/forum/general/51954/
	a_fout->write(path, count + 1); //write path +1 for null character

	//write image res (only dealing with square textures)
	a_fout->write((char*)&a_tex->width, sizeof(unsigned int)); //write res

//	//write image data
//	std::string str;
//	str.append(reinterpret_cast<const char*>(a_tex->data));
//	//http://stackoverflow.com/questions/658913/c-style-cast-from-unsigned-char-to-const-char

//	size_t dataCount = strlen(str.c_str());
//	const char* data = str.c_str();
//	a_fout->write((char*)&dataCount, sizeof(unsigned int)); //write path
//	a_fout->write(data, dataCount + 1); //write path +1 for null character
}

bool FBXIO::DoesFileExist(const char *fileName)
{
	//http://stackoverflow.com/questions/12774207/fastest-way-to-check-if-a-file-exist-using-standard-c-c11-c
	std::ifstream infile(fileName);
	return infile.good();	
}

void FBXIO::createFBX(fbxInternals a_fbxInternal, FBXFile& a_file)
{
	//FBXFile* fbxFile = new FBXFile();
	FBXMeshNode* mesh = new FBXMeshNode;
	FBXTexture* diffuse = new FBXTexture();
	FBXTexture* spec = new FBXTexture();
	FBXTexture* normal = new FBXTexture();

	mesh->m_indices = a_fbxInternal.m_indices;

	for (int i = 0; i < a_fbxInternal.verts.size(); ++i)
	{
		FBXVertex curVert;

		curVert.position =	a_fbxInternal.verts[i].position;
		curVert.normal =	a_fbxInternal.verts[i].normal;
		curVert.tangent =	a_fbxInternal.verts[i].tangent;
		curVert.binormal =	a_fbxInternal.verts[i].binormal;
		curVert.indices =	a_fbxInternal.verts[i].indices;
		curVert.texCoord1 = a_fbxInternal.verts[i].texCoord1;

		mesh->m_vertices.push_back(curVert);
	}

	a_file.m_meshes.push_back(mesh);

	diffuse->data = (unsigned char*)a_fbxInternal.m_dif.data;
	diffuse->name =		a_fbxInternal.m_dif.name;
	diffuse->height =	a_fbxInternal.m_dif.res;
	diffuse->width = 	a_fbxInternal.m_dif.res;
	diffuse->format =	-1;
	diffuse->path =		a_fbxInternal.m_dif.path;

	normal->data = (unsigned char*)a_fbxInternal.m_normal.data;
	normal->name = a_fbxInternal.m_normal.name;
	normal->height = a_fbxInternal.m_normal.res;
	normal->width = a_fbxInternal.m_normal.res;
	normal->format = -1;
	normal->path = a_fbxInternal.m_normal.path;

	spec->data = (unsigned char*)a_fbxInternal.m_spec.data;
	spec->name = a_fbxInternal.m_spec.name;
	spec->height = a_fbxInternal.m_spec.res;
	spec->width = a_fbxInternal.m_spec.res;
	spec->format = -1;
	spec->path = a_fbxInternal.m_spec.path;

	std::map<std::string, FBXTexture*>::iterator it = a_file.m_textures.begin();
	a_file.m_textures.insert(it, std::pair<std::string, FBXTexture*>(a_fbxInternal.m_dif.path, diffuse));
	a_file.m_textures.insert(it, std::pair<std::string, FBXTexture*>(a_fbxInternal.m_normal.path, normal));
	a_file.m_textures.insert(it, std::pair<std::string, FBXTexture*>(a_fbxInternal.m_spec.path, spec));

	//a_file = (*fbxFile);

	
}