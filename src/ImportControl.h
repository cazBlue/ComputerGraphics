#ifndef IMPORTCTRL_H
#define IMPORTCTRL_H

#include <gl_core_4_4.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <string>


#include <iostream>
#include <fstream>

#include <stb_image.h>
#include <FBXFile.h>

#include <FBXIO.h>
#include <ObjIO.h>



class ImportCTRL
{
public:
	//main functions
	ImportCTRL();
	~ImportCTRL();
	void Start();
	void Shutdown();
	void Update();

	//flags
	bool m_isLoaded = false;

public:
	//file imports
	OBJIO* m_OBJ_bunny;
	std::vector<tinyobj::shape_t> objShapes;

	FBXFile* m_FBX_bunny;
	FBXFile* m_FBX_soulSpear;
	FBXFile* m_FBX_anim;


public:
	//opengl buffers

	struct OpenGLInfo
	{
		unsigned int m_VAO;
		unsigned int m_VBO;
		unsigned int m_IBO;
		unsigned int m_index_count;
	};

	std::vector<OpenGLInfo> m_gl_info;



private:	
	//create opengl buffers
	void createFBXOpenGLBuffers(FBXFile* fbx, FBXIO::fbxInternals a_interal);
	void cleanupOpenGLBuffers(FBXFile* fbx);
	void createOBJOpenGLBuffers(std::vector<tinyobj::shape_t>& shapes);


};



#endif IMPORTCTRL_H