#pragma once
#include <glm/glm.hpp>
class FreeCamera
{

private:
	glm::vec3 cameraPos, cameraUp;
	glm::vec3 dir;
	float pitch, yaw;
	float fov;
	bool changedView, changedProjection, vpUpdated;
	glm::mat4 view, projection, vp;
public:
	float maxPitch = 90.0f, minPitch = -90.0f, maxFov = 45.0f, minFov = 1.0f;

	FreeCamera(const glm::vec3 &cameraPos, float yaw = -90.0f, float pitch = 0.0f, float fov = 45.0f);

	const glm::vec3 &getCameraPos();
	const glm::vec3 &getCameraDir();
	const glm::vec3 &getCameraRight();
	const glm::mat4 &getViewMatrix();
	const glm::mat4 &getProjectionMatrix();
	const glm::mat4 &getVPMatrix();
	const float getFOV();
	const float getPitch();
	const float getYaw();


	void setCameraPos(const glm::vec3 &camPos);
	void setPitch(float pitch);
	void setYaw(float yaw);
	void setFOV(float fov);

	void validateMaxMins();
	void updateCameraDir(bool forced = false);
	const glm::mat4 &updateMatrices(bool forced = false);
	const bool isVPUpdated();
	void clearFlags();
};