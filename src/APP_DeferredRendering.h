#ifndef APP_DEFERREDRENDERING_H
#define APP_DEFERREDRENDERING_H
#include <Application.h>

//#include <tiny_obj_loader.h>
#include <vector>

class FBXFile;

class APP_DeferredRendering : public App
{
public:
	APP_DeferredRendering();
	~APP_DeferredRendering();
	void Update(float a_dt);
	void Draw();
	bool Start();
	bool Shutdown();

public:

	//creaters
	void createGpassBuffer();
	void createLightBuffer();
	void createCompositeBuffer();

	void createScene(); //loads fbx and creates needed buffers
	void createFullSreenQuad();
	void createTargetBuffers(); //also loads obj
//	void createOpenGLBuffers(std::vector<tinyobj::shape_t>& shapes);
	


	//pass vars
	unsigned int m_gpassFBO, m_albedoTexture, m_positionTexture, 
		m_normalTexture, m_gpassDepth;

	//light buffers
	unsigned int m_lightFBO, m_lightTexture;
	unsigned int m_fullScreenQuad_vao, m_fullScreenQuad_vbo;
	
	//obj	
//	std::vector<tinyobj::shape_t> shapes;
//	std::vector<tinyobj::material_t> materials;

	//fbx
	FBXFile* m_fbx;
	void createOpenGLBuffers(FBXFile* fbx);
	void cleanupOpenGLBuffers(FBXFile* fbx);
	void loadImg(int* a_height, int* a_width, int* a_format, const char* a_path, unsigned int* a_id);
	

	unsigned int m_programGBuffer; //object scene
	unsigned int m_programDirectionalLight; 
	unsigned int m_programComposite; 


	void drawDirectionalLight(const glm::vec3& direction, const glm::vec3& diffuse);


	//bounding cube and point lights
	void createBoundingCube();

	unsigned int m_cubeVBO, m_cubeVAO, m_cubeIBO, m_pointLightShader;

	void drawPointLight(const glm::vec3& position, float radius, const glm::vec3& diffuse);



	

//	struct OpenGLInfo
//	{
//		unsigned int m_VAO;
//		unsigned int m_VBO;
//		unsigned int m_IBO;
//		unsigned int m_index_count;
//	};
//
//	std::vector<OpenGLInfo> m_gl_info;


	std::string LoadShader(const char *a_filePath);
};

#endif APP_DEFERREDRENDERING_H