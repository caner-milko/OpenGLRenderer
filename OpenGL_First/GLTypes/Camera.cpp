#include <GLTypes/Camera.h>
#include <glm/gtc/matrix_transform.hpp>

FreeCamera::FreeCamera(const glm::vec3 &cameraPos, float yaw, float pitch, float fov) : cameraPos(cameraPos), cameraUp(0.0f, 1.0f, 0.0f), yaw(yaw), pitch(pitch), fov(fov)
{
	updateMatrices(true);
}

const glm::vec3 &FreeCamera::getCameraPos()
{
	return cameraPos;
}
const glm::vec3 &FreeCamera::getCameraDir()
{
	updateCameraDir();
	return dir;
}

const glm::vec3 &FreeCamera::getCameraRight()
{
	return glm::normalize(glm::cross(getCameraDir(), cameraUp));
}
const glm::mat4 &FreeCamera::getViewMatrix()
{
	updateMatrices();
	return view;
}
const glm::mat4 &FreeCamera::getProjectionMatrix()
{
	updateMatrices();
	return projection;
}
const glm::mat4 &FreeCamera::getVPMatrix()
{
	updateMatrices();
	return vp;
}
const float FreeCamera::getFOV()
{
	return fov;
}
const float FreeCamera::getPitch()
{
	return pitch;
}
const float FreeCamera::getYaw()
{
	return yaw;
}


void FreeCamera::setCameraPos(const glm::vec3 &camPos)
{
	if(camPos == cameraPos)
	{
		return;
	}
	cameraPos = camPos;
	changedView = true;
}
void FreeCamera::setPitch(float pitch)
{
	if(pitch > maxPitch || pitch < minPitch)
	{
		pitch = glm::clamp(pitch, minPitch, maxPitch);
	}
	if(this->pitch == pitch)
		return;
	this->pitch = pitch;

	changedView = true;
}
void FreeCamera::setYaw(float yaw)
{
	if(this->yaw == yaw)
		return;
	this->yaw = yaw;
	changedView = true;
}
void FreeCamera::setFOV(float fov)
{
	if(fov > maxFov || fov < minFov)
	{
		fov = glm::clamp(fov, minFov, maxFov);
	}
	if(this->fov == fov)
		return;
	this->fov = fov;

	changedProjection = true;
}

void FreeCamera::validateMaxMins()
{
	if(pitch > maxPitch || pitch < minPitch)
	{
		pitch = glm::clamp(pitch, minPitch, maxPitch);
		changedView = true;
	}
	if(fov > maxFov || fov < minFov)
	{
		fov = glm::clamp(fov, minFov, maxFov);
		changedProjection = true;
	}
}
void FreeCamera::updateCameraDir(bool forced)
{
	if(changedView || forced)
	{
		dir.x = glm::cos(glm::radians(yaw)) * glm::cos(glm::radians(pitch));
		dir.y = glm::sin(glm::radians(pitch));
		dir.z = glm::sin(glm::radians(yaw)) * glm::cos(glm::radians(pitch));
	}
}
const glm::mat4 &FreeCamera::updateMatrices(bool forced)
{
	updateCameraDir(forced);
	if(changedView || forced)
	{
		view = glm::lookAt(cameraPos, cameraPos + dir, cameraUp);
	}
	if(changedProjection || forced)
	{
		projection = glm::perspective(glm::radians(fov), 800.0f / 600.0f, 0.1f, 100.0f);
	}
	if(changedView || changedProjection || forced)
	{
		vp = projection * view;
		vpUpdated = true;
	}
	changedProjection = false;
	changedView = false;
	return vp;
}

const bool FreeCamera::isVPUpdated()
{
	return vpUpdated;
}

void FreeCamera::clearFlags()
{
	vpUpdated = false;
	changedProjection = false;
	changedView = false;
}