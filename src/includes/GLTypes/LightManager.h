#pragma once
#include <GLTypes/Light.h>
#include <iostream>
#include <string>
#include <unordered_map>
class ILightType
{
public:
	std::vector<Shader *> *const litShaders;
	const std::string typeName;
	uint32_t maxLightCount;
	ILightType(const std::string &typeName, const uint32_t maxLightCount, std::vector<Shader *> *const litShaders);
	virtual void updateTransforms() = 0;
	virtual void free() = 0;
};

template<typename Type>
class LightType : public ILightType
{
public:
	static LightType<Type> *singleton;
	std::unordered_map<uint32_t, Type *> lights;
	LightType(const std::string &typeName, const uint32_t maxLightCount, std::vector<Shader *> *const litShaders);
	Type *addLight();
	void updateTransforms() override;
	void free() override;
};

class LightManager
{
private:
	std::unordered_map<std::string, ILightType *> lightTypes;
public:
	std::vector<Shader *> litShaders;
	template<typename Type>
	LightType<Type> *getLightType(const std::string &lightName, uint32_t maxLightCount)
	{
		if(LightType<Type>::singleton != nullptr)
		{
			return LightType<Type>::singleton;
		}
		if(maxLightCount == 0)
		{
			std::cout << "Max Light Count can't be 0, initialize lightType " << lightName << " before adding light." << std::endl;
			return nullptr;
		}
		LightType<Type> *lightType = new LightType<Type>(lightName, maxLightCount, &litShaders);
		lightTypes[lightName] = lightType;
		return lightType;
	}
	template<typename Type>
	Type *addLight(const std::string &lightType)
	{
		LightType<Type> *type = getLightType<Type>(lightType, 0);
		return type->addLight();
	}
	void updateLights();
	~LightManager();
};

