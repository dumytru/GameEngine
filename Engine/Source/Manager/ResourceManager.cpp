//#include <pch.h>
#include "ResourceManager.h"

#include <include/glm.h>
#include <include/glm_utils.h>
#include <include/util.h>
#include <sstream>

#include <Manager/Manager.h>
#include <Manager/SceneManager.h>
#include <Manager/ShaderManager.h>
#include <Manager/DebugInfo.h>

#include <Core/GameObject.h>
#include <Component/Mesh.h>
#include <Component/Transform.h>
#include <Component/Renderer.h>

#ifdef PHYSICS_ENGINE
#include <include/havok.h>
#include <Component/Physics.h>
#endif

ResourceManager::ResourceManager() {
}

ResourceManager::~ResourceManager() {
	for (auto obj : _objects)
		SAFE_FREE(obj.second);
	for (auto obj : meshes)
		SAFE_FREE(obj.second);
}

void ResourceManager::Load(const char* file) {
	Manager::Debug->InitManager("Resources");

	// Load document
	const pugi::xml_document &doc = *(pugi::LoadXML(file));

	// Load Meshes
	const char* meshName;
	pugi::xml_node materialXML;
	pugi::xml_node meshesXML = doc.child("meshes");

	for (pugi::xml_node mesh: meshesXML.children()) {
		string fileLocation = mesh.child_value("path");
		fileLocation += '\\';
		fileLocation += mesh.child_value("file");
		meshName = mesh.child_value("name");
		materialXML = mesh.child("material");
		auto quad = mesh.attribute("quad");

		Mesh *M = new Mesh();
		if (quad)			M->SetGlPrimitive(GL_QUADS);
		if (materialXML) 	M->UseMaterials(false);
		if (!M->LoadMesh(fileLocation)) {
			SAFE_FREE(M);
			continue;
		}
		meshes[meshName] = M;
	}

	// --- Load GameObjects --- //
	const char *objName;
	pugi::xml_node shaderInfo;
	pugi::xml_node physicsInfo;
	pugi::xml_node transformInfo;
	pugi::xml_node renderingInfo;
	pugi::xml_node objects = doc.child("objects");

	for (pugi::xml_node obj: objects.children()) {
		objName			= obj.child_value("name");
		meshName		= obj.child_value("mesh");
		shaderInfo		= obj.child("shader");
		physicsInfo		= obj.child("physics");
		transformInfo	= obj.child("transform");
		renderingInfo	= obj.child("shadow");

		GameObject *GO = new GameObject(objName);
		SetTransform(transformInfo, *GO->transform);
		if (meshName) {
			GO->mesh = meshes[meshName];
			GO->SetupAABB();
		}

		if (shaderInfo) {
			GO->UseShader(Manager::Shader->GetShader(shaderInfo.text().get()));
		}

#ifdef PHYSICS_ENGINE
		if (physicsInfo) {
			GO->physics = new Physics(GO);
			GO->physics->LoadHavokFile(physicsInfo.child_value("file"));
		}
#endif

		if (renderingInfo) {
			GO->renderer->SetCastShadow(true);
		}

		_objects[objName] = GO;
	}
}

GameObject* ResourceManager::GetGameObject(const char *name) {
	if (_objects[name])
		return new GameObject(*_objects[name]);
	return nullptr;
}

void ResourceManager::SetTransform(pugi::xml_node node, Transform &T) {
	if (!node)
		return;

	pugi::xml_node prop;

	prop = node.child("position");
	if (prop)
		T.position = glm::ExtractVector<glm::vec3>(prop.text().get());

	prop = node.child("rotation");
	if (prop) {
		glm::vec3 rotation = glm::ExtractVector<glm::vec3>(prop.text().get());
		T.SetRotation(rotation);
	}

	prop = node.child("scale");
	if (prop)
		T.scale = glm::ExtractVector<glm::vec3>(prop.text().get());

	T.Update();
}