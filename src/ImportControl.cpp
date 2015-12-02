#include <ImportControl.h>
#include <stb_image.h>
#include <thread>

ImportCTRL::ImportCTRL()
{

}
 

ImportCTRL::~ImportCTRL()
{

}


void ImportCTRL::Start()
{
	m_FBX_soulSpear = new FBXFile();
	m_FBX_anim = new FBXFile();
	m_FBX_bunny = new FBXFile();

	printf("\nloading FBX files\n");

	FBXIO::fbxInternals soulSpear, bunny, anim;

	FBXIO* fbx = new FBXIO();
	FBXIO* fbxTwo = new FBXIO();
	m_OBJ_bunny = new OBJIO();
	std::vector<std::thread> threads;

	//multi threaded loading of obj and fbx files
	//no dependant data to avoid race conditions

	//file must exist for loading to avoid race condition
	//fbx->ReadObj(soulSpear, "soulSpear.bin", "./assets/soulspear/soulspear.fbx");
	//fbxTwo->ReadObj(bunny, "bunny.bin", "./assets/stanford/Bunny.fbx");

	threads.push_back(std::thread(
		// defining a lambda that can access main scope with &
		[&](){
			fbx->ReadObj(soulSpear, "soulSpear.bin", "./assets/soulspear/soulspear.fbx");
			fbx->createFBX(soulSpear, (*m_FBX_soulSpear));
		}
	));

	threads.push_back(std::thread(
		// defining a lambda that can access main scope with &
		[&](){		
		fbxTwo->ReadObj(bunny, "bunny.bin", "./assets/stanford/Bunny.fbx");
		fbxTwo->createFBX(bunny, (*m_FBX_bunny));
	}
	));

	threads.push_back(std::thread(
		// defining a lambda that can access main scope with &
		[&](){
		m_FBX_anim->load("./assets/characters/Pyro/pyro.fbx", m_FBX_anim->UNITS_METER, true, true, true); //no customising of fbx animation, no need!
	}
	));

	threads.push_back(std::thread(
		// defining a lambda that can access main scope with &
		[&](){
		printf("\nloadingOBJ");
		m_OBJ_bunny->ReadObj("./assets/stanford_objs/bunny.obj", "OBJbunny.dat", &objShapes);
	}
	));

	for (auto& thread : threads)
		thread.join();

//	m_FBX_soulSpear->load("./assets/soulspear/soulspear.fbx", m_FBX_soulSpear->UNITS_METER, true, true, true);
	FBXFile* bunTest = new FBXFile(); 
	bunTest->load("./assets/soulspear/soulspear.fbx", m_FBX_bunny->UNITS_METER, true, true, true);
	

	printf("\nFBX files loaded");
	printf("\nOBJ files loaded");

	printf("\nloading FBX opengl buffers");
	createFBXOpenGLBuffers(m_FBX_soulSpear, soulSpear);
	//createFBXOpenGLBuffers(m_FBX_anim); //created in the anim app
	createFBXOpenGLBuffers(m_FBX_bunny, bunny);

	printf("\nloading OBJ opengl buffers");
	createOBJOpenGLBuffers(objShapes);

	printf("\nloading shaders and apps");

	delete fbx;
	m_isLoaded = true;
}

void ImportCTRL::createOBJOpenGLBuffers(std::vector<tinyobj::shape_t>& shapes)
{
	m_gl_info.resize(shapes.size());

	for (unsigned int mesh_index = 0; mesh_index < shapes.size(); ++mesh_index)
	{
		glGenVertexArrays(1, &m_gl_info[mesh_index].m_VAO);
		glGenBuffers(1, &m_gl_info[mesh_index].m_VBO);
		glGenBuffers(1, &m_gl_info[mesh_index].m_IBO);
		glBindVertexArray(m_gl_info[mesh_index].m_VAO);
		unsigned int float_count = shapes[mesh_index].mesh.positions.size();
		float_count += shapes[mesh_index].mesh.normals.size();
		float_count += shapes[mesh_index].mesh.texcoords.size();
		std::vector<float> vertex_data;
		vertex_data.reserve(float_count);
		vertex_data.insert(vertex_data.end(), shapes[mesh_index].mesh.positions.begin(), shapes[mesh_index].mesh.positions.end());
		vertex_data.insert(vertex_data.end(), shapes[mesh_index].mesh.normals.begin(), shapes[mesh_index].mesh.normals.end());
		m_gl_info[mesh_index].m_index_count = shapes[mesh_index].mesh.indices.size();
		glBindBuffer(GL_ARRAY_BUFFER, m_gl_info[mesh_index].m_VBO);
		glBufferData(GL_ARRAY_BUFFER, vertex_data.size() * sizeof(float), vertex_data.data(), GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_gl_info[mesh_index].m_IBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, shapes[mesh_index].mesh.indices.size() * sizeof(unsigned int), shapes[mesh_index].mesh.indices.data(), GL_STATIC_DRAW);
		glEnableVertexAttribArray(0); //position
		glEnableVertexAttribArray(1); //normal data (colour information
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_TRUE, 0, (void*)(sizeof(float)*shapes[mesh_index].mesh.positions.size()));
		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
}

void ImportCTRL::createFBXOpenGLBuffers(FBXFile* fbx, FBXIO::fbxInternals a_interal)
{
	unsigned int meshCount = fbx->getMeshCount();
	unsigned int materialCount = fbx->getMaterialCount();
	// create the GL VAO/VBO/IBO data for each mesh	
	for (unsigned int i = 0; i < meshCount; ++i)
	{
		FBXMeshNode* mesh = fbx->getMeshByIndex(i);

//		FBXMaterial* mat = mesh->m_material;

//		FBXTexture* diffuesTex = mat->textures[FBXMaterial::TextureTypes::DiffuseTexture];
//		FBXTexture* normalTex = mat->textures[FBXMaterial::TextureTypes::NormalTexture];
//		FBXTexture* specTex = mat->textures[FBXMaterial::TextureTypes::SpecularTexture];

		// storage for the opengl data in 4 unsigned int (includes diffuse texID (3) and normal texID (4))
		unsigned int* glData = new unsigned int[6];

		glGenVertexArrays(1, &glData[0]);
		glBindVertexArray(glData[0]);
		glGenBuffers(1, &glData[1]);
		glGenBuffers(1, &glData[2]);
		glBindBuffer(GL_ARRAY_BUFFER, glData[1]);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, glData[2]);
		glBufferData(GL_ARRAY_BUFFER, mesh->m_vertices.size() * sizeof(FBXVertex), mesh->m_vertices.data(), GL_STATIC_DRAW);
//		glBufferData(GL_ARRAY_BUFFER, a_interal.verts.size() * sizeof(FBXIO::vertLayout), a_interal.verts.data(), GL_STATIC_DRAW);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh->m_indices.size() * sizeof(unsigned int), mesh->m_indices.data(), GL_STATIC_DRAW);
		glEnableVertexAttribArray(0); // position
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(FBXVertex), 0);
		glEnableVertexAttribArray(1); // normal
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_TRUE, sizeof(FBXVertex), ((char*)0) + FBXVertex::NormalOffset);
		glEnableVertexAttribArray(2); // colour / tex cords
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_TRUE, sizeof(FBXVertex), ((char*)0) + FBXVertex::TexCoord1Offset);
		glEnableVertexAttribArray(3); // tangent coords / tex cords
		glVertexAttribPointer(3, 4, GL_FLOAT, GL_TRUE, sizeof(FBXVertex), ((char*)0) + FBXVertex::TangentOffset);
		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		if (a_interal.m_dif.data != nullptr)
		{
			//gen diffuse texture id and bind texture to buffer 
			glActiveTexture(GL_TEXTURE0); //texture are we binding to
			glGenTextures(1, &glData[3]);
			glBindTexture(GL_TEXTURE_2D, glData[3]);
//			glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
			
			unsigned char* data = stbi_load(a_interal.m_dif.path.c_str(), &a_interal.m_dif.width, &a_interal.m_dif.height, &a_interal.m_dif.format, STBI_rgb); //request no alpha
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, a_interal.m_dif.res, a_interal.m_dif.res, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			stbi_image_free(data); //unload the image data
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		}

		
		if (a_interal.m_normal.data != nullptr)
		{
			glActiveTexture(GL_TEXTURE0 + 1); //texture are we binding to
			glGenTextures(1, &glData[4]);
			glBindTexture(GL_TEXTURE_2D, glData[4]);			
			unsigned char* data = stbi_load(a_interal.m_normal.path.c_str(), &a_interal.m_dif.width, &a_interal.m_normal.height, &a_interal.m_normal.format, STBI_rgb); //request no alpha
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, a_interal.m_normal.res, a_interal.m_normal.res, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			stbi_image_free(data); //unload the image data
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		}

		if (a_interal.m_spec.data != nullptr)
		{
			glActiveTexture(GL_TEXTURE0 + 2); //texture are we binding to
			glGenTextures(1, &glData[5]);
			glBindTexture(GL_TEXTURE_2D, glData[5]);
			unsigned char* data = stbi_load(a_interal.m_spec.path.c_str(), &a_interal.m_spec.width, &a_interal.m_spec.height, &a_interal.m_normal.format, STBI_rgb); //request no alpha
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, a_interal.m_spec.res, a_interal.m_spec.res, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			stbi_image_free(data); //unload the image data
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		}


		mesh->m_userData = glData;
	}
}




void ImportCTRL::Update()
{

}

void ImportCTRL::Shutdown()
{
	delete m_FBX_soulSpear 	;
	delete m_FBX_anim		;
	delete m_FBX_bunny		;
}