//#include <pch.h>
#include "WindowObject.h"

#include <Core/InputSystem.h>
#include <Core/WindowManager.h>

WindowObject::WindowObject() {
	Init("Engine", glm::ivec2(840, 480), glm::ivec2(300, 300), true);
}

void WindowObject::Init(char* name, glm::ivec2 resolution, glm::ivec2 position, bool reshapable)
{
	this->name = name;
	this->resolution = resolution;
	this->position = position;
	this->reshapable = reshapable;

	aspectRatio = float(resolution.x) / resolution.y;
	center = resolution / 2;
}

glm::ivec2 WindowObject::GetCenter() {
	return center;
}

void WindowObject::FullScreen() {
	GLFWmonitor *monitor = glfwGetPrimaryMonitor();
	const GLFWvidmode *videoDisplay = glfwGetVideoMode(monitor);
	window = glfwCreateWindow(videoDisplay->width, videoDisplay->height, name, monitor, NULL);
	glfwMakeContextCurrent(window);
	SetSize(videoDisplay->width, videoDisplay->height);
	SetWindowCallbacks();
}

void WindowObject::WindowMode() {
	window = glfwCreateWindow(resolution.x, resolution.y, name, NULL, NULL);
	glfwMakeContextCurrent(window);
	glfwSetWindowPos(window, (int)position.x, (int)position.y);
	SetSize(resolution.x, resolution.y);
	SetWindowCallbacks();
}

void WindowObject::SetWindowCallbacks() {
	glfwSetWindowSizeCallback(window, WindowManager::OnResize);
	glfwSetKeyCallback(window, InputSystem::KeyCallback);
	glfwSetCursorPosCallback(window, InputSystem::CursorMove);

	/* Force Vertical Sync */
//	wglSwapIntervalEXT(1);
}

void WindowObject::SetSize(int width, int height) {
	resolution = glm::ivec2(width, height);
	center = resolution / 2;
	aspectRatio = float(width) / height;
	glViewport(0, 0, width, height);
//	ClipPointer(true);
}

// Clip user cursor inside the Window
// ClipCursor function use from Window.h
void WindowObject::ClipPointer(bool state) {
	int clippingEdge = 5;
	if (state == false) {
		// ClipCursor(0);
		return;
	}

	int posX, posY;
	glfwGetWindowPos(window, &posX, &posY);
	WindowRECT.left = posX + clippingEdge;
	WindowRECT.top = posY + clippingEdge;
	WindowRECT.bottom = WindowRECT.top + resolution.y - 2 * clippingEdge;
	WindowRECT.right = WindowRECT.left + resolution.x - 2 * clippingEdge;

	// ClipCursor(&WindowRECT);
}