#include <iostream>
#include "Window.h"
#include <glm/common.hpp>

Window::Window(uint32_t windowWidth, uint32_t windowHeight, const char* windowName, int sampleCount)
{
	GLFWwindow* window = nullptr;

	// Initialising GLFW
	if (!glfwInit())
		std::cerr << "Failed to initialize GLFW!" << std::endl;

	glfwWindowHint(GLFW_SAMPLES, sampleCount);
	// Create a windowed mode window and its OpenGL context 
	window = glfwCreateWindow(windowWidth, windowHeight, windowName, NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		std::cerr << "Failed to create window!" << std::endl;
	}

	// Make the window's context current 
	glfwMakeContextCurrent(window);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	glfwSetWindowUserPointer(window, this);
	glfwSetWindowSizeCallback(window, windowResizeCallback);
	glfwSetWindowUserPointer(window, &resized);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cerr << "Failed to initialize GLAD" << std::endl;
	}

	this->windowHandle = window;

	onUpdate();
}

Window::~Window() // This function called at the end of the program
{
	glfwTerminate();
}

void Window::onUpdate() // This function called at the beginning of each frame
{
	resized = false;
	// Cursor update
	xLast = xCursor;
	lastY = yCursor;
	glfwGetCursorPos(windowHandle, &xCursor, &yCursor); // Location of cursor using screen coordinates
	for (auto& pair : keyStates)
	{
		pair.second = keyPressed(pair.first); // Updating key states
	}
	// Updating keys
	glfwPollEvents();
	glfwSwapBuffers(windowHandle);

}

// Returns true if the key is being pressed at the moment
bool Window::keyPressed(int key) const
{
	return bool(glfwGetKey(windowHandle, key)); //
}

bool Window::keyTapped(int key) const // Returns true if the key is being pressed at the moment and was not pressed in the previous frame
{
	if (keyStates.find(key) != keyStates.end())
	{
		return (keyStates[key] == false && keyPressed(key) == true);
	}
	else
	{
		keyStates.insert({ key, false });
	}
	return false;
}

bool Window::mouseButtonPressed(int key) const
{
	return bool(glfwGetMouseButton(windowHandle, key));
}

glm::ivec2 Window::getWindowSize() const
{
	glm::ivec2 winSize;
	glfwGetWindowSize(windowHandle, &winSize.x, &winSize.y);
	return winSize;
}

const glm::vec2 Window::cursorPostition() const
{
	return glm::vec2(xCursor, yCursor);
}

glm::vec2 Window::offCursing() const
{
	glm::vec2 cursorOff;
	cursorOff.x = (float)xCursor - (float)xLast;
	cursorOff.y = (float)lastY - (float)yCursor;
	return cursorOff;
}

void Window::windowResizeCallback(GLFWwindow* window, int w, int h)
{
	*(static_cast<bool*>(glfwGetWindowUserPointer(window))) = true;
}
