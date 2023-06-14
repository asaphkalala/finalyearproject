#include "Sphere.h"
#include "Window.h"
#include <memory>
#include "Camera.h"
#include "Texture.h"
#include "Cube.h"
#include "Hair.h"
#include "DrawingShader.h"
#include <glm/gtc/matrix_access.hpp>
#include <iostream>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/string_cast.hpp>
#include <array>

template<typename T> using Unique = std::unique_ptr<T>;

int main(void)
{
	Unique<Window> window = std::make_unique<Window>(1440, 810, "Afro Hair Sim", 4);
	glEnable(GL_MULTISAMPLE); // Enable multisampling
	glEnable(GL_DEPTH_TEST); // Enable depth testing
	glDepthFunc(GL_LEQUAL); // Set depth function to less than or equal to (used for skybox)
	glClearColor(0.1f, 0.1f, 0.1f, 1.f); // Set clear color to dark grey

// Set the aspect ratio of the camera to match the window size and set the position of the camera
	PerspCam cam;
	cam.setProjectionAspectRatio(1440.f / 810); 
	cam.setPosition(glm::vec3(-5.f, 3.f, 5.f)); 
	cam.setProjectionViewingAngle(100.f);

	// Light source model
	Unique<Sphere> lightSphere = std::make_unique<Sphere>(10, 5, 0.5f);
	lightSphere->scale(glm::vec3(0.1f));
	lightSphere->translate(glm::vec3(1.f, 2.f, 1.f));
	lightSphere->color = glm::vec3(1.f);

	// Skybox model
	Unique<Cube> skybox = std::make_unique<Cube>(glm::vec3(-1.f), glm::vec3(1.f));
	Texture skyboxCubemap("room.png", GL_TEXTURE_CUBE_MAP, false);

	// Basic hair model
	Unique<Hair> hair = std::make_unique<Hair>(2000, 4.f, 0.f);
	hair->color = glm::vec3(0.45f, 0.18f, 0.012f);

	// Shaders setup 
	DrawingShader basicShader("BasicVertexShader.glsl", "BasicFragmentShader.glsl");
	DrawingShader skyboxShader("SkyboxVertexShader.glsl", "SkyboxFragmentShader.glsl");
	DrawingShader hairShader("HairVertexShader.glsl", "HairGeometryShader.glsl", "HairFragmentShader.glsl");

	// Hair Simulation Parameters
	bool doPhysics = false; //

	enum Control {
		HAIR_FRICTION,
		HAIR_CURLINESS,
		HAIR_STRAND_COUNT,
		VELOCITY_DAMPING
	};

	// Controls which action arrows currently correspond to
	
	Control currentControl = HAIR_FRICTION;

	// This variables are used to track the state of the arrow keys

	glViewport(0, 0, window->getWindowSize().x, window->getWindowSize().y);
	do {
		glDisable(GL_CULL_FACE);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		skyboxShader.use();
		skyboxShader.setMat4("projection", cam.getProjection());
		skyboxShader.setMat4("view", cam.getView());
		skyboxCubemap.activateAndBind(GL_TEXTURE0);
		skybox->draw();

		// Light source model drawing 

		if (doPhysics && glm::abs(window->getTime().deltaTime - window->getTime().lastDeltaTime) < 0.1f)
			hair->applyPhysics(window->getTime().deltaTime, window->getTime().runningTime); // Apply physics to hair

		// Hair model drawing
		basicShader.setVec3("objectColor", glm::vec3(1.f, 0.f, 0.f)); // Set color of ellipsoids to red

		// if the M key is pressed, draw the ellipsoids as wireframes 
		if (window->keyPressed(GLFW_KEY_M)) {
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			for (const auto& s : hair->getEllipsoids()) {
				basicShader.setMat4("model", hair->getTransformMatrix() * s->getTransformMatrix());
				s->draw();
			}
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}
		hair->color = tempColor;
		hairShader.use();
		hairShader.setMat4("projection", cam.getProjection());
		hairShader.setMat4("view", cam.getView());
		hairShader.setMat4("model", hair->getTransformMatrix());
		hairShader.setFloat("curlRadius", hair->getCurlRadius());
		hairShader.setVec3("eyePosition", cam.getPosition());
		hairShader.setUint("particlesPerStrand", hair->particleperStrand());
		hairShader.setVec3("light.position", glm::vec3(glm::column(lightSphere->getTransformMatrix(), 3)));
		hair->updateColorsBasedOnMaterial(hairShader, Entity::Material::HAIR);
		hair->draw();


		float deltaTime = window->getTime().deltaTime; // Get the time between frames

		// Camera movement using space, left shift and WASD keys
		if (window->keyPressed(GLFW_KEY_W))
			cam.moveCamera(Camera::Directions::FORWARD, deltaTime);
		if (window->keyPressed(GLFW_KEY_S))
			cam.moveCamera(Camera::Directions::BACKWARD, deltaTime);
		if (window->keyPressed(GLFW_KEY_A))
			cam.moveCamera(Camera::Directions::LEFT, deltaTime);
		if (window->keyPressed(GLFW_KEY_D))
			cam.moveCamera(Camera::Directions::RIGHT, deltaTime);
		if (window->keyPressed(GLFW_KEY_SPACE))
			cam.moveCamera(Camera::Directions::UP, deltaTime);
		if (window->keyPressed(GLFW_KEY_LEFT_SHIFT))
			cam.moveCamera(Camera::Directions::DOWN, deltaTime);

	// Camera rotation using mouse
		if (window->mouseButtonPressed(GLFW_MOUSE_BUTTON_RIGHT))
			cam.rotateCamera(window->offCursing());

		for (int i = 0; i <= 4; ++i) // Check which control key is being pressed between 1 and 4
		{
			if (window->keyTapped(i + GLFW_KEY_0)) // If the key is being tapped, set the current control to the num value
			{
				control = (Control)i;
				std::cout << "Currently controlling: ";
				switch (i)
				{
					case 1:
						std::cout << "Hair friction" << std::endl;
						break;
					case 2:
						std::cout << "Hair curliness" << std::endl;
						break;
					case 3:
						std::cout << "Hair strand count" << std::endl;
						break;
					case 4:
						std::cout << "Velocity damping" << std::endl;
						break;
				}
				break;
			}
		}
		switch (control) /* Check which control is currently being used and editing the corresponding parameter. 
		Use the up and down arrow keys to change the value and increments by 0.01 for friction and damping
		and 1 for curliness and strand count*/
		{
			case HAIR_FRICTION:
				if (doPhysics && window->keyTapped(GLFW_KEY_UP))
					hair->setFrictionFactor(hair->getFrictionFactor() + 0.01f);
				else if (doPhysics && window->keyTapped(GLFW_KEY_DOWN))
					hair->setFrictionFactor(hair->getFrictionFactor() - 0.01f);
				break;

			case HAIR_CURLINESS:
				if (window->keyTapped(GLFW_KEY_UP))
					hair->increaseCurlRadius();
				else if (window->keyTapped(GLFW_KEY_DOWN))
					hair->decreaseCurlRadius();
				break;

			case HAIR_STRAND_COUNT:
				if (window->keyTapped(GLFW_KEY_UP))
					hair->increaseStrandCount();
				else if (window->keyTapped(GLFW_KEY_DOWN))
					hair->decreaseStrandCount();
				break;

			case VELOCITY_DAMPING:
				if (window->keyTapped(GLFW_KEY_UP))
					hair->increaseVelocityDamping();
				else if (window->keyTapped(GLFW_KEY_DOWN))
					hair->decreaseVelocityDamping();
				break;
		}

		if (window->keyTapped(GLFW_KEY_ENTER))
			doPhysics = !doPhysics;

		window->onUpdate();
	} while (!window->windowClose());

	return 0;
}
