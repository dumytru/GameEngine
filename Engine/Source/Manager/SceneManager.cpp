//#include <pch.h>
#include "SceneManager.h"

#include <iostream>

#ifdef PHYSICS_ENGINE
#include <Component/Physics.h>
#endif
#include <Component/Transform.h>

#include <Core/Camera/Camera.h>
#include <Core/GameObject.h>

#include <Lighting/PointLight.h>

#include <Manager/Manager.h>
#include <Manager/ResourceManager.h>
#include <Manager/DebugInfo.h>
#include <Manager/EventSystem.h>

SceneManager::SceneManager() {
}

SceneManager::~SceneManager() {
}

void SceneManager::LoadScene(const char *fileName) {
	Manager::Debug->InitManager("Scene");
	sceneFile = fileName;

	frustumObjects.clear();
	activeObjects.clear();
	lights.clear();
	toRemove.clear();

	// Load document
	pugi::xml_document &doc = *pugi::LoadXML(fileName);

	// TODO
	// Load different type of objects based on container
	// examples - lights, coins, etc
	// const char *container = child.name();

	// --- Load GameObjects --- //
	const char *refID;
	pugi::xml_node transformInfo;
	pugi::xml_node objects = doc.child("objects");

	for (pugi::xml_node obj: objects.children()) {
		refID	= obj.child_value("ref");
		transformInfo	= obj.child("transform");

		GameObject *GO = Manager::Resource->GetGameObject(refID);
		Manager::Resource->SetTransform(transformInfo, *GO->transform);
		GO->SetupAABB();
		AddObject(GO);
	}

	// --- Load Lights --- //
	pugi::xml_node pointLights = doc.child("lights");

	for (pugi::xml_node light: pointLights.children()) {
		transformInfo = light.child("transform");

		auto *L = new PointLight();
		Manager::Resource->SetTransform(transformInfo, *L->transform);

		this->lights.push_back(L);
	}

	// --- Load orbs --- //
	Transform *T = new Transform();
	pugi::xml_node orbs = doc.child("orbs");
	for (pugi::xml_node orb: orbs.children()) {
		Manager::Resource->SetTransform(orb, *T);
		Manager::Event->EmitSync(EventType::CREATE_ORB, (Object*)T);
	}
}

// TODO - bug clear Physics World... just loaded objects
void SceneManager::ReloadScene() {
	LoadScene(sceneFile);
}

void SceneManager::Update() {
	bool shouldAdd = !toAdd.empty();
	bool shouldRemove = !toRemove.empty();

	if (shouldAdd || shouldRemove) {
		if (shouldAdd) {
			for (auto obj: toAdd) {
				activeObjects.push_back(obj);
			}
			toAdd.clear();
		}

		if (shouldRemove) {
			for (auto obj: toRemove) {
				auto elem = find(activeObjects.begin(), activeObjects.end(), obj);
				activeObjects.remove(obj);
			}
			toRemove.clear();
		}
	}
}

void SceneManager::AddObject(GameObject *obj) {
	toAdd.push_back(obj);
#ifdef PHYSICS_ENGINE
	if (obj->physics)
		obj->physics->AddToWorld();
#endif
}

void SceneManager::RemoveObject(GameObject *obj) {
	toRemove.push_back(obj);
#ifdef PHYSICS_ENGINE
	if (obj->physics)
		obj->physics->RemoveFromWorld();
#endif
}

void SceneManager::FrustumCulling(Camera *camera) {
	frustumObjects.clear();
	for (auto obj: activeObjects) {
		if (camera->ColidesWith(obj))
			frustumObjects.push_back(obj);
	}
}
