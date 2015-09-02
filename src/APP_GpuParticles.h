#ifndef GPUPARTICLES_H
#define GPUPARTICLES_H
#include <Application.h>

class APP_GPUParticles : public App
{
public:
	APP_GPUParticles();
	~APP_GPUParticles();
	void Update(float a_dt);
	void Draw();
	bool Start();
	bool Shutdown();

public:

	unsigned int loadShader(unsigned int type, const char* path);

//	struct posCoord{
//		glm::vec3 position;
//		glm::vec3 texCoord;
//	};

	struct GPUParticle {
		GPUParticle() : lifetime(1), lifespan(0) {}
		glm::vec3 position;
//		glm::vec3 texCord;
		//posCoord posCord;
		glm::vec3 velocity;
		float lifetime;
		float lifespan;

		//unsigned int indexData[6];

	};

	unsigned int m_textureID1;

	void initalise(unsigned int a_maxParticles,
		float a_lifespanMin, float a_lifespanMax,
		float a_velocityMin, float a_velocityMax,
		float a_startSize, float a_endSize,
		const glm::vec4& a_startColour,
		const glm::vec4& a_endColour);

	void draw(float time, const glm::mat4& a_cameraTransform,
		const glm::mat4& a_projectionView);


protected:
	void createBuffers();
	void createUpdateShader();
	void createDrawShader();

	GPUParticle* m_particles;
	unsigned int m_maxParticles;
	glm::vec3 m_position;
	float m_lifespanMin;
	float m_lifespanMax;
	float m_velocityMin;
	float m_velocityMax;
	float m_startSize;
	float m_endSize;
	glm::vec4 m_startColour;
	glm::vec4 m_endColour;
	unsigned int m_activeBuffer;
	unsigned int m_vao[2];
	unsigned int m_vbo[2];
	unsigned int m_drawShader;
	unsigned int m_updateShader;
	float m_lastDrawTime;

	void loadImg(int* a_height, int* a_width, int* a_format, const char* a_path, unsigned int* a_id);
};


#endif GPUPARTICLES_H