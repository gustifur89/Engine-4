#pragma once
#include "Header.h"
#include "GameObject.h"

class ParticleSystem : public GameObject
{
protected:
	struct Particle
	{
		Particle() {};

		Particle(glm::vec3 pos_, float size_, glm::vec3 vel_, glm::vec3 acc_, glm::vec3 col_, float life_) : pos(pos_), size(size_), vel(vel_), acc(acc_), col(col_), life(life_) {};

		glm::vec3 pos;
		glm::vec3 vel;
		glm::vec3 acc;
		glm::vec3 col;
		float life;
		float size;
	};
	
public:
	std::shared_ptr<ColorParticleShader> shader;
	glm::mat4 colorMatrix;
	static std::vector<GLfloat> g_vertex_buffer_data;
	static float aspectRatio;
	void setAspectRatio(float aspectRatio);

	GLuint particleVAO;
	GLuint particleVertexBufferID;
	GLuint particlePositionBufferID;
	GLuint particleColorBufferID;

	int maxParticles;
	int particleCount;

	std::vector<Particle> particles;
	std::vector<GLfloat> particlePos;
	std::vector<GLfloat> particleCol;

	ParticleSystem();

	void updateParticles(float dt);
	void spawnParticle(glm::vec3 pos, float size, glm::vec3 color, glm::vec3 vel, glm::vec3 acc, float life);
	void renderFunc(std::shared_ptr<Camera> camera, glm::mat4 parentTransform);
};

