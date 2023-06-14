#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/vec3.hpp>
#include <glm/vec2.hpp>
#include <glm/vec4.hpp>
#include <unordered_map>


// This class is a wrapper around GLFWwindow and provides some useful functions for input handling
class Window {
public:
	Window(uint32_t windowWidth = 1024, uint32_t windowHeight = 768, const char* windowName = "MyApp", int sampleCount = 1); 
	~Window();
	void onUpdate(); // updating window

	// Getters and setters for the cursor position, cursor offset, window size, key states, mouse button states, and whether the window should close
	glm::vec2 offCursing() const; 
	const glm::vec2 cursorPostition() const; 
	glm::ivec2 getWindowSize() const;
	bool keyPressed(int key) const;
	bool keyTapped(int key) const;
	bool mouseButtonPressed(int key) const;
	bool windowClose() const { return glfwWindowShouldClose(windowHandle); }
	const bool isResized() const { return resized; }
 
private:
// Callback functions for keyboard and mouse input
	GLFWwindow* windowHandle = nullptr; 
	double xCursor{ 512.f }, yCursor{ 384.f }; //
	double xLast{ 0.f }, lastY{ 0.f };
	static void windowResizeCallback(GLFWwindow* window, int w, int h);
	bool resized = false; 
	mutable std::unordered_map<int, bool> keyStates;
};