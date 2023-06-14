#include "Camera.h"
#include <iostream>
#include <glm/gtx/string_cast.hpp>


#define SPACE_MOVE (moveLength * deltaTime)
#define FULL_CIRCLE (2 * glm::pi<float>())

// Constructor for Camera class that sets the camera's position, center, and up vector
Camera::Camera()
{
	recalculateEulerAngles();
}

 // Getters and setters for the camera's position, center, up, projection near plane, and projection far plane
void Camera::recalculateEulerAngles()
{
	wDirection = glm::normalize(center - position);
	center = position + wDirection;
	uDirection = glm::cross(wDirection, lookUp);
	vDirection = glm::cross(uDirection, wDirection);

	pitch = glm::asin(glm::dot(glm::vec3(0.f, 1.f, 0.f), wDirection));
	oscillation = glm::atan(glm::dot(glm::vec3(0.f, 0.f, -1.f), wDirection) / glm::dot(glm::vec3(1.f, 0.f, 0.f), wDirection));

	if (wDirection.z < 0.f) 
	{
		oscillation = FULL_CIRCLE - oscillation;
	}

	view = glm::lookAt(position, center, lookUp);
	viewChanged = true;
}

void Camera::setPosition(const glm::vec3& position)
{
	this->position = position;
	center = position + wDirection;
	view = glm::lookAt(position, center, lookUp);
	viewChanged = true;
}

void Camera::setCenter(const glm::vec3& center)
{
	this->center = center;
	recalculateEulerAngles();
}

void Camera::setUp(const glm::vec3& up)
{
	lookUp = up;
	recalculateEulerAngles();
}

void Camera::setProjectionNearPlane(float near)
{
	nearPlane = near;
	project();
}

void Camera::setProjectionFarPlane(float far)
{
	farPlane = far;
	project();
}

void Camera::rotateCamera(const glm::vec2& offCurs)
{
	float offMouseX = offCurs.x;
	float offMouseY = offCurs.y;

	oscillation += offMouseX * mouseSensitivity;
	if (oscillation < 0.f)
		oscillation += FULL_CIRCLE;
	else if (oscillation > FULL_CIRCLE)
		oscillation -= FULL_CIRCLE;

	if (pitch + (offMouseY * mouseSensitivity) < glm::pi<float>() / 2 && pitch + (offMouseY * mouseSensitivity) > -glm::pi<float>() / 2)
		pitch += offMouseY * mouseSensitivity;

	wDirection.x = glm::cos(oscillation) * glm::cos(pitch);
	wDirection.y = glm::sin(pitch);
	wDirection.z = glm::sin(oscillation) * glm::cos(pitch);
	wDirection = glm::normalize(wDirection);
	center = position + wDirection;
	uDirection = glm::cross(wDirection, lookUp);
	vDirection = glm::cross(uDirection, wDirection);

	view = glm::lookAt(position, center, lookUp);
	viewChanged = true;
}

void Camera::setMouseSensitivity(float sensitivity)
{
	mouseSensitivity = sensitivity;
	recalculateEulerAngles();
}

void Camera::setMoveSensitivity(float sensitivity)
{
	moveLength = sensitivity;
	recalculateEulerAngles();
}

// 3d camera view
void Camera::moveCamera(Directions direction, float deltaTime)
{
	switch (direction)
	{
		case Directions::FORWARD:
			position += wDirection * SPACE_MOVE;
			center += wDirection * SPACE_MOVE;
			break;

		case Directions::BACKWARD:
			position -= wDirection * SPACE_MOVE;
			center -= wDirection * SPACE_MOVE;
			break;

		case Directions::LEFT:
			position -= uDirection * SPACE_MOVE;
			center -= uDirection * SPACE_MOVE;
			break;

		case Directions::RIGHT:
			position += uDirection * SPACE_MOVE;
			center += uDirection * SPACE_MOVE;
			break;

		case Directions::UP:
			position += lookUp * SPACE_MOVE;
			center += lookUp * SPACE_MOVE;
			break;

		case Directions::DOWN:
			position -= lookUp * SPACE_MOVE;
			center -= lookUp * SPACE_MOVE;
			break;
	}

	view = glm::lookAt(position, center, lookUp);
	viewChanged = true;
}

// 2d camera view
PerspCam::PerspCam()
{
	project();
}

void PerspCam::setProjectionViewingAngle(float angle)
{
	viewingAngle = angle;
	project();
}
void PerspCam::setProjectionAspectRatio(float ratio)
{
	aspectRatio = ratio;
	project();
}

void PerspCam::project()
{
	projection = glm::perspective(glm::radians(viewingAngle), aspectRatio, nearPlane, farPlane);
	projectionChanged = true;
}


// TODO: add orthopedic camera view
OrthoCam::OrthoCam()
{
	project();
}

void OrthoCam::setProjectionLeftPlane(float left)
{
	leftPlane = left;
	project();
}

void OrthoCam::setProjectionRightPlane(float right)
{
	rightPlane = right;
	project();
}

void OrthoCam::setProjectionTopPlane(float top)
{
	topPlane = top;
	project();
}

void OrthoCam::setProjectionBottomPlane(float bottom)
{
	bottomPlane = bottom;
	project();
}

void OrthoCam::project()
{
	projection = glm::ortho(leftPlane, rightPlane, nearPlane, farPlane);
	projectionChanged = true;
}