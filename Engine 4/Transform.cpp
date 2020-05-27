#include "Transform.h"

Transform::Transform()
{
	position = glm::vec3(0.0);	
	setRotation(0, 0, 0);
	scale = glm::vec3(1.0);
}

Transform::Transform(glm::vec3 pos, glm::vec3 rot, glm::vec3 scale)
{
	this->position = pos;
	setRotation(rot);
	this->scale = scale;
}

// == Setter

void Transform::setPosition(float x, float y, float z)
{
	position.x = x;
	position.y = y;
	position.z = z;
}

void Transform::setPosition(glm::vec3 position)
{
	this->position = position;
}

void Transform::setPosition(Transform& transform)
{
	position = transform.position;
}

void Transform::setRotation(double angleX, double angleY, double angleZ)
{
	setRotation(glm::vec3(angleX, angleY, angleZ));
}

void Transform::setRotation(glm::vec3 eulerAngles)
{
	rotation = glm::quat(glm::radians(eulerAngles));
}

void Transform::setRotation(Transform& transform)
{
	this->rotation = transform.rotation;
}

void Transform::setRotation(glm::quat quat)
{
	rotation = quat;
}

void Transform::setDirection(glm::vec3 direction)
{
	rotation = LookAt(glm::vec3(0), direction);
}

void Transform::setScale(float scaleX, float scaleY, float scaleZ)
{
	scale.x = scaleX;
	scale.y = scaleY;
	scale.z = scaleZ;
}

void Transform::setScale(glm::vec3 scale)
{
	this->scale = scale;
}

void Transform::setScale(Transform& transform)
{
	scale = transform.scale;
}

// == Getters

glm::vec3 Transform::getPosition()
{
	return position;
}

glm::vec3 Transform::getRotation()
{
	glm::vec3 rots = glm::degrees(glm::eulerAngles(rotation));
	if (std::fabs(rots.z) >= 90) {
		rots.x += 180.f;
		rots.y = 180.f - rots.y;
		rots.z += 180.f;
	}
	return rots;// glm::degrees(glm::eulerAngles(rotation));
}

glm::quat Transform::getRotationQuat()
{
	return rotation;
}

glm::vec3 Transform::getScale()
{
	return scale;
}

// == Functions

glm::mat4 Transform::getRotationMatrix(glm::vec3 angles)
{
	return glm::toMat4(glm::quat(glm::radians(angles)));
}

glm::mat4 Transform::getRotationMatrix(glm::quat rotQuat)
{
	return glm::toMat4(rotQuat);
}

glm::vec3 Transform::getTransformedZ()
{
	glm::mat4 rotationMatrix = getRotationMatrix(rotation);
	glm::vec4 zAxis(0, 0, 1, 0);
	
	return rotationMatrix * zAxis;
}

glm::vec3 Transform::getTransformedZ(glm::vec3 angles)
{
	glm::mat4 rotationMatrix = getRotationMatrix(angles);
	glm::vec4 zAxis(0, 0, 1, 0);

	return rotationMatrix * zAxis;
}

glm::vec3 Transform::getTransformedZ(glm::quat quat)
{
	glm::mat4 rotationMatrix = getRotationMatrix(quat);
	glm::vec4 zAxis(0, 0, 1, 0);

	return rotationMatrix * zAxis;
}

glm::vec3 Transform::getTransformedX()
{
	glm::mat4 rotationMatrix = getRotationMatrix(rotation);
	glm::vec4 xAxis(1, 0, 0, 0);

	return rotationMatrix * xAxis;
}

glm::vec3 Transform::getTransformedX(glm::vec3 angles)
{
	glm::mat4 rotationMatrix = getRotationMatrix(angles);
	glm::vec4 xAxis(1, 0, 0, 0);

	return rotationMatrix * xAxis;
}

glm::vec3 Transform::getTransformedX(glm::quat quat)
{
	glm::mat4 rotationMatrix = getRotationMatrix(quat);
	glm::vec4 xAxis(1, 0, 0, 0);

	return rotationMatrix * xAxis;
}

glm::vec3 Transform::getTransformedY()
{
	glm::mat4 rotationMatrix = getRotationMatrix(rotation);
	glm::vec4 yAxis(0, 1, 0, 0);

	return rotationMatrix * yAxis;
}

glm::vec3 Transform::getTransformedY(glm::vec3 angles)
{
	glm::mat4 rotationMatrix = getRotationMatrix(angles);
	glm::vec4 yAxis(0, 1, 0, 0);

	return rotationMatrix * yAxis;
}

glm::vec3 Transform::getTransformedY(glm::quat quat)
{
	glm::mat4 rotationMatrix = getRotationMatrix(quat);
	glm::vec4 yAxis(0, 1, 0, 0);

	return rotationMatrix * yAxis;
}

glm::mat4 Transform::getTransformMatrix()
{
	//glm::quat rotationQuat;
	//rotationQuat = glm::quat(glm::vec3(angleX * TO_RAD, angleY * TO_RAD, angleZ * TO_RAD));

	glm::mat4 translation = glm::translate(position);// glm::translate(glm::mat4(1.0), position);

	glm::mat4 rotationMatrix = getRotationMatrix(rotation);

	glm::mat4 scaleMatrix = glm::scale(scale);

	return translation * rotationMatrix * scaleMatrix;
}

glm::mat4 Transform::getRotationMatrix()
{
	return getRotationMatrix(rotation);
}

glm::mat4 Transform::getScaleMatrix()
{
	return glm::scale(scale);
}

glm::mat4 Transform::getTranslationMatrix()
{
	return glm::translate(glm::mat4(1.0), position);
}

//Appliers

void Transform::rotate(Transform& transform)
{
	rotate(transform.rotation);
}

void Transform::rotate(glm::vec3 eulerAngles)
{
	rotate(glm::quat(glm::radians(eulerAngles)));
}

void Transform::rotate(glm::quat rotQuat)
{
	//rotation = rotQuat * rotation * glm::inverse(rotQuat);
	this->rotation = rotation * rotQuat;// this->rotation* rotQuat;
}

void Transform::translate(glm::vec3 translation)
{
	position += translation;
}

void Transform::translate(float x, float y, float z)
{
	position.x += x;
	position.y += y;
	position.z += z;
}

void Transform::translate(Transform& transform)
{
	position += transform.position;
}

void Transform::applyScale(glm::vec3 scale)
{
	this->scale *= scale;
}

void Transform::applyScale(float x, float y, float z)
{
	this->scale.x *= x;
	this->scale.y *= y;
	this->scale.z *= z;
}

void Transform::applyScale(Transform& transform)
{
	this->scale *= transform.scale;
}

