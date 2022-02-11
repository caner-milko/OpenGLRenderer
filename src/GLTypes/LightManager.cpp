#include <GLTypes/LightManager.h>

ILightType::ILightType(const std::string &typeName, const uint32_t maxLightCount, std::vector<Shader *> *const litShaders) : typeName(typeName), maxLightCount(maxLightCount), litShaders(litShaders)
{
}

template<typename Type>
LightType<Type> *LightType<Type>::singleton;

template<typename Type>
LightType<Type>::LightType(const std::string &typeName, const uint32_t maxLightCount, std::vector<Shader *> *const litShaders) : ILightType(typeName, maxLightCount, litShaders)
{
	LightType<Type>::singleton = this;
}

template<typename Type>
void LightType<Type>::updateTransforms()
{
	for(auto &it : lights)
		((Light *)it.second)->updateTransforms();
}

template<typename Type>
Type *LightType<Type>::addLight()
{
	int32_t suitable = -1;
	for(int32_t i = 0; i < (int32_t)maxLightCount; i++)
	{
		if(lights.find(i) == lights.end())
		{
			suitable = i;
			break;
		}
	}

	if(suitable == -1)
	{
		std::cout << "No available light slot for light type: " << typeName << std::endl;
		return nullptr;
	}

	Type *light = new Type(suitable, typeName, litShaders);
	lights.insert_or_assign(suitable, light);
	return light;
}

template<typename Type>
void LightType<Type>::free()
{
	for(auto &it : lights)
	{
		((Light *)it.second)->free();
		delete it.second;
	}
	lights.clear();
}

void LightManager::updateLights()
{
	for(auto &it : lightTypes)
	{
		it.second->updateTransforms();
	}
}

LightManager::~LightManager()
{
	for(auto &it : lightTypes)
	{
		it.second->free();
		delete it.second;
	}
}

template LightType<PointLight>;
template LightType<DirectionalLight>;
template LightType<SpotLight>;