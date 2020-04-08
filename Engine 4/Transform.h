#pragma once
#include "Headers.h"
class Transform
{
private:
	static glm::quat LookAt(glm::vec3 sourcePoint, glm::vec3 destPoint)
	{
		glm::vec3 forwardVector = glm::normalize(destPoint - sourcePoint);

		glm::vec3 rotAxis = glm::cross(glm::vec3(0,0,1), forwardVector);
		float dot = glm::dot(glm::vec3(0, 0, 1), forwardVector);

		glm::quat q;
		q.x = rotAxis.x;
		q.y = rotAxis.y;
		q.z = rotAxis.z;
		q.w = dot + 1;

		return glm::normalize(q);
	}

protected:
	glm::vec3 position;
	glm::quat rotation;
	glm::vec3 scale;

public:
	Transform();
	Transform(glm::vec3 pos, glm::vec3 rot, glm::vec3 scale);

	//Getters and setters
	void setPosition(float x, float y, float z);
	void setPosition(glm::vec3 position);
	void setPosition(Transform& transform);

	void setRotation(double angleX, double angleY, double angleZ);
	void setRotation(glm::vec3 eulerAngles);
	void setRotation(Transform& transform);
	void setDirection(glm::vec3 direction);

	void setScale(float scaleX, float scaleY, float scaleZ);
	void setScale(glm::vec3 scale);
	void setScale(Transform& transform);

	glm::vec3 getPosition();
	glm::vec3 getRotation();
	glm::vec3 getScale();
	
	//Other getter
	glm::mat4 getTransformMatrix();
	glm::mat4 getRotationMatrix();
	glm::mat4 getScaleMatrix();
	glm::mat4 getTranslationMatrix();

	glm::vec3 getTransformedX();
	glm::vec3 getTransformedY();
	glm::vec3 getTransformedZ();

	static glm::mat4 getRotationMatrix(glm::vec3 angles);
	static glm::mat4 getRotationMatrix(glm::quat rotQuat);
	static glm::vec3 getTransformedX(glm::vec3 angles);
	static glm::vec3 getTransformedY(glm::vec3 angles);
	static glm::vec3 getTransformedZ(glm::vec3 angles);

	//appliers
	inline Transform operator+(const Transform& other)
	{
		Transform out;
		out.position = this->position + other.position;
		out.rotation = this->rotation + other.rotation;
		out.scale = this->scale + other.scale;
		return out;
	}

	void rotate(Transform& transform);
	void rotate(glm::vec3 eulerAngles);
	void rotate(glm::quat rotQuat);

	void translate(glm::vec3 translation);
	void translate(float x, float y, float z);
	void translate(Transform& transform);

	void applyScale(glm::vec3 scale);
	void applyScale(float x, float y, float z);
	void applyScale(Transform& transform);
};

