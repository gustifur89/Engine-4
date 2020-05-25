#include "GameObject.h"

// ============================== GameObject ===========================

float GameObject::stopSpeed = 0.01;

GameObject::GameObject(std::shared_ptr<Mesh> mesh, std::shared_ptr<Shader> shader) : mesh(mesh), shader(shader)
{
	visible = true;
	persistentVisible = false;
	collider = NULL;
	elasticity = 0.0;
	mass = 0.0;
	invMass = 0.0;
	velocity = glm::vec3(0, 0, 0);
	staticFriction = 0.4;
	dynamicFriction = 0.2;
	friction = 0.0;
	collisionReactEnabled = true;
	physiceEnabled = true;
	gravityAffected = true;
	neverDisable = false;
	noClip = false;
}

void GameObject::renderFunc(std::shared_ptr<Camera> camera, glm::mat4 parentTransform)
{
	//no nothing
}

void GameObject::cleanUp(std::shared_ptr<GameObject> object, bool removeChildren)
{
	for (int i = 0; i < object->children.size(); i++)
	{
		bool removeThis = object->children[i]->toRemove;
		cleanUp(object->children[i], removeThis || removeChildren);

		if (object->children[i]->toRemove || removeChildren)
		{
			object->children.erase(object->children.begin() + i);
			i--;
		}
	}

}

void GameObject::setMass(float mass)
{
	this->mass = mass;
	invMass = (mass > 0.0) ? 1.0 / mass : 0.0;
}

void GameObject::setElasticity(float elasticity)
{
	this->elasticity = elasticity;
}

void GameObject::render(std::shared_ptr<Camera> camera, glm::mat4 parentTransform)
{
	if (!visible) return;

	if (this->persistentVisible)
	{
		this->renderFunc(camera, parentTransform);
	}
	else if (mesh)
	{
		//if (true || camera->isBoxInView(mesh->bounds, parentTransform * this->transform.getTransformMatrix()))
		if (camera->isBoxInView(mesh->bounds, parentTransform * this->transform.getTransformMatrix()))
		{
			this->renderFunc(camera, parentTransform);
		}
	}

	for (int i = 0; i < children.size(); i++)
	{
		children[i]->render(camera, parentTransform * this->transform.getTransformMatrix());
	}
}

void GameObject::addChild(std::shared_ptr<GameObject> gameObject)
{
	children.push_back(gameObject);
}

// == Physics

void GameObject::applyImpulse(glm::vec3 impulse, glm::vec3 position)
{
	if (glm::any(glm::isnan(impulse))) return;
	this->impulse += impulse;

	/*
	glm::vec2 dif = position - pos;

	glm::vec3 angImpul = glm::cross(glm::vec3(dif, 0), glm::vec3(impulse, 0));
	this->applyAngularImpulse(angImpul.z);
	*/
}

void GameObject::applyForce(glm::vec3 force, glm::vec3 position)
{
	if (glm::any(glm::isnan(force))) return;
	this->force += force;
	/*
	glm::vec2 dif = position - pos;

	glm::vec3 tor = glm::cross(glm::vec3(dif, 0), glm::vec3(force, 0));
	this->applyTorque(tor.z);
	*/
}


// ============================== GameObjectColor ===========================

GameObjectColor::GameObjectColor(std::shared_ptr<ColorMesh> mesh, std::shared_ptr<ColorShader> shader) : shader(shader)
{
	colorMatrix = glm::mat4(1.0);
	this->mesh = mesh;
	shininess = 0.0;
}

void GameObjectColor::setFillColor(int r, int g, int b)
{
	colorMatrix = glm::mat4(0.0);
	colorMatrix[3][0] = r / 255.f;
	colorMatrix[3][1] = g / 255.f;
	colorMatrix[3][2] = b / 255.f;
	colorMatrix[3][3] = 1.0f;
}

void GameObjectColor::setShininess(float shininess)
{
	this->shininess = shininess;
}

void GameObjectColor::renderFunc(std::shared_ptr<Camera> camera, glm::mat4 parentTransform)
{
	glm::mat4 MMatrix = parentTransform * transform.getTransformMatrix();
	glm::mat4 MVMatrix = camera->getTransformMatrix() * MMatrix;
	glm::mat4 MVPmatrix = camera->getProjectionMatrix() * MVMatrix;
	glm::mat4 depMVPmatrix = camera->getDepthProjectionMatrix() * MVMatrix;
	glm::mat4 NMmatrix = glm::transpose(glm::inverse(MVMatrix));

	if (shader && mesh)
	{
		shader->useShader();

		/*
		if (isColliding)
		{
			glm::mat4 nColorMatrix = glm::mat4(0.0);
			nColorMatrix[3][0] = 255 / 255.f;
			nColorMatrix[3][1] = 0 / 255.f;
			nColorMatrix[3][2] = 0 / 255.f;
			nColorMatrix[3][3] = 1.0f;

			shader->setMatrixes(MVPmatrix, MVMatrix, NMmatrix, shininess, nColorMatrix);
		}
		else
		{
			shader->setMatrixes(MVPmatrix, MVMatrix, NMmatrix, shininess, colorMatrix);
		}
		//*/

		shader->setMatrixes(MVPmatrix, MVMatrix, NMmatrix, MMatrix, depMVPmatrix, shininess, colorMatrix);
		mesh->render();
	}
}

// ============================== GameObjectTexture ===========================

GameObjectTexture::GameObjectTexture(std::shared_ptr<TextureMesh> mesh, std::shared_ptr<TextureShader> shader) : shader(shader)
{
	colorMatrix = glm::mat4(1.0);
	this->mesh = mesh;
	shininess = 0.0;
}

void GameObjectTexture::setFillColor(int r, int g, int b)
{
	colorMatrix = glm::mat4(0.0);
	colorMatrix[3][0] = r / 255.f;
	colorMatrix[3][1] = g / 255.f;
	colorMatrix[3][2] = b / 255.f;
	colorMatrix[3][3] = 1.0f;
}

void GameObjectTexture::setShininess(float shininess)
{
	this->shininess = shininess;
}

void GameObjectTexture::renderFunc(std::shared_ptr<Camera> camera, glm::mat4 parentTransform)
{
	glm::mat4 MMatrix = parentTransform * transform.getTransformMatrix();
	glm::mat4 MVMatrix = camera->getTransformMatrix() * MMatrix;
	glm::mat4 MVPmatrix = camera->getProjectionMatrix() * MVMatrix;
	glm::mat4 depMVPmatrix = camera->getDepthProjectionMatrix() * MVMatrix;
//	glm::mat4 NMmatrix = glm::transpose(glm::inverse(MVMatrix));
	glm::mat4 NMmatrix = glm::transpose(glm::inverse(MMatrix));

	if (shader && mesh && texture)
	{
		shader->useShader();
		shader->setTexture(texture);
		shader->setMatrixes(MVPmatrix, MVMatrix, NMmatrix, MMatrix, depMVPmatrix, shininess, colorMatrix);
		mesh->render();
	}
}

