#include <APP_Particles_emitter.h>



APP_particleEmitter::~APP_particleEmitter() {
	delete[] m_particles;
	delete[] m_vertexData;
}









