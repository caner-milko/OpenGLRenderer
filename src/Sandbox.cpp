#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <GLTypes/renderer.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <Sandbox.h>

Renderer *renderer;
GLFWwindow *window;

float texOffsetx = 0.0f, texOffsety = 0.0f;

float deltaTime = 0.0f;	// Time between current frame and last frame
float currentFrame = 0.0f;
float lastFrame = 0.0f; // Time of last frame

FreeCamera *camera;

glm::vec3 cameraPos(0.0f, 0.0f, 3.0f);

float yaw = -90.0f, pitch = 0.0f;
float lastX = 400, lastY = 300;
bool firstMouse = true;
float fov = 45.0f;

void initWindow();
void initCamera();
void initTextures();
void initShaders();
void initLights();
void initObjects();
void update();
void renderLoop();
void end();

Sandbox::Sandbox()
{
	initWindow();
	renderer = new Renderer(RendererData());
	renderer->init();
	initCamera();
	initTextures();
	initShaders();
	initLights();
	initObjects();
	renderLoop();
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void processInput(GLFWwindow *window);
void mouse_callback(GLFWwindow *window, double xpos, double ypos);
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);

void initWindow()
{
	//Init glfw
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);

	if(window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return;
	}
	glfwMakeContextCurrent(window);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return;
	}

	/*---------------------------------------------------*/
}

void initCamera()
{
	camera = renderer->camera;
	camera->setCameraPos(glm::vec3(0.0f, 0.0f, 3.0f));
}

Texture2D *wallTex, *smileTex;

void initTextures()
{
	wallTex = new Texture2D("./Assets/wall.jpg", false, TextureType::RGB, TextureType::RGB);
	smileTex = new Texture2D("./Assets/awesomeface.png", true, TextureType::RGBA, TextureType::RGBA);
}

Shader *defShader, *lightingShader, *lightShader, *litShader, *materialShader, *testShader;

glm::vec3 objectColor = glm::vec3(1.0f, 0.5f, 0.31f);
glm::vec3 lightColor = glm::vec3(1.0f);

glm::vec3 matSpecular = glm::vec3(0.5f);
glm::vec1 matShininess = glm::vec1(32.0f);

void initShaders()
{

	defShader = new Shader("./Assets/Shaders/def_vert.glsl", "./Assets/Shaders/def_frag.glsl");
	defShader->setUniformValDef<Texture2D *>("texture1", wallTex);
	defShader->setUniformValDef<Texture2D *>("texture2", smileTex);


	lightingShader = new Shader("./Assets/Shaders/light_vert.glsl", "./Assets/Shaders/light_frag.glsl");


	litShader = new Shader("./Assets/Shaders/lit_vert.glsl", "./Assets/Shaders/lit_frag.glsl");

	renderer->lightManager->litShaders.push_back(litShader);
	litShader->setUniformValDef("material.ambient", objectColor);
	litShader->setUniformValDef("material.diffuse", objectColor);
	litShader->setUniformValDef("material.specular", matSpecular);
	litShader->setUniformValDef("material.shininess", matShininess);


	materialShader = new Shader("./Assets/Shaders/material_vert.glsl", "./Assets/Shaders/material_frag.glsl");

	renderer->lightManager->litShaders.push_back(materialShader);
	materialShader->setUniformValDef("material.ambient", glm::vec3(1.0f));
	materialShader->setUniformValDef("material.diffuseTint", glm::vec3(1.0f));
	materialShader->setUniformValDef("material.specularTint", glm::vec3(1.0f));
	materialShader->setUniformValDef("material.shininess", glm::vec1(32.0f));
	materialShader->setUniformValDef("material.texture_diffuse", wallTex);
	materialShader->setUniformValDef("material.texture_specular", smileTex);


	testShader = new Shader("./Assets/Shaders/test_vert.glsl", "./Assets/Shaders/test_frag.glsl");



}

PointLight *pointLight;
SpotLight *spotLight;
DirectionalLight *dirLightStatic;

glm::vec3 pointLightPos = glm::vec3(1.0f, 2.0f, -1.0f);
glm::vec3 spotLightPos = glm::vec3(-4.0f, 0.0f, 0.0f);

void initLights()
{
	renderer->lightManager->createLightType<PointLight>("pointLight", 5);
	renderer->lightManager->createLightType<DirectionalLight>("directionalLight", 3);
	renderer->lightManager->createLightType<SpotLight>("spotLight", 5);
	pointLight = renderer->lightManager->reserveLight<PointLight>("pointLight");

	pointLight->setup();
	pointLight->setPosition(pointLightPos);

	spotLight = renderer->lightManager->reserveLight<SpotLight>("spotLight");

	spotLight->setup();
	spotLight->setPosition(spotLightPos);

	dirLightStatic = renderer->lightManager->reserveLight<DirectionalLight>("directionalLight");

	dirLightStatic->setup(glm::vec3(0.4), glm::vec3(1.0f), glm::vec3(1.0f));
	dirLightStatic->setYaw(-135.0f);
	dirLightStatic->setPitch(-30.0f);
}

SimpleRenderObject *pointLightObj, *spotLightObj, *litObj, *totObj, *odd;
std::vector<Mesh *> screw, room;
std::vector<RenderObject *> oddClones;

void initObjects()
{

	std::vector<float> vertices = {
	-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
	 0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
	-0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

	-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	-0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	 0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	 0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
	 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
	 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

	-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	-0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
	-0.5f,  0.5f, -0.5f,  0.0f, 1.0f
	};

	std::vector<float> normalCube = {
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,

		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
	};

	std::vector<float> lightVertices = {
		-0.5f, -0.5f, -0.5f,
		 0.5f, -0.5f, -0.5f,
		 0.5f,  0.5f, -0.5f,
		 0.5f,  0.5f, -0.5f,
		-0.5f,  0.5f, -0.5f,
		-0.5f, -0.5f, -0.5f,

		-0.5f, -0.5f,  0.5f,
		 0.5f, -0.5f,  0.5f,
		 0.5f,  0.5f,  0.5f,
		 0.5f,  0.5f,  0.5f,
		-0.5f,  0.5f,  0.5f,
		-0.5f, -0.5f,  0.5f,

		-0.5f,  0.5f,  0.5f,
		-0.5f,  0.5f, -0.5f,
		-0.5f, -0.5f, -0.5f,
		-0.5f, -0.5f, -0.5f,
		-0.5f, -0.5f,  0.5f,
		-0.5f,  0.5f,  0.5f,

		 0.5f,  0.5f,  0.5f,
		 0.5f,  0.5f, -0.5f,
		 0.5f, -0.5f, -0.5f,
		 0.5f, -0.5f, -0.5f,
		 0.5f, -0.5f,  0.5f,
		 0.5f,  0.5f,  0.5f,

		-0.5f, -0.5f, -0.5f,
		 0.5f, -0.5f, -0.5f,
		 0.5f, -0.5f,  0.5f,
		 0.5f, -0.5f,  0.5f,
		-0.5f, -0.5f,  0.5f,
		-0.5f, -0.5f, -0.5f,

		-0.5f,  0.5f, -0.5f,
		 0.5f,  0.5f, -0.5f,
		 0.5f,  0.5f,  0.5f,
		 0.5f,  0.5f,  0.5f,
		-0.5f,  0.5f,  0.5f,
		-0.5f,  0.5f, -0.5f,
	};

	std::vector<float> totCube = {
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f,
		 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f,

		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,  0.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,  1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,  1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,  1.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,  0.0f, 0.0f,

		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f,
		-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f,
		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f,

		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f,

		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f
	};

	std::vector<uint32_t> indices = {
		0, 1, 2,
		2,3,0
	};

	std::vector<uint32_t> attributeSizes = {3,2};
	std::vector<uint32_t> lightAttributeSizes = {3};
	std::vector<uint32_t> normalAttributeSizes = {3,3};
	std::vector<uint32_t> totAttributeSizes = {3, 3, 2};

	pointLightObj = renderer->addOddObject(RenderObjectData(), lightingShader, lightVertices, lightAttributeSizes);

	pointLightObj->setPosition(pointLightPos);


	spotLightObj = renderer->addOddObject(RenderObjectData(), lightingShader, lightVertices, lightAttributeSizes);

	spotLightObj->setPosition(spotLightPos);


	litObj = renderer->addOddObject(RenderObjectData(), litShader, normalCube, normalAttributeSizes);

	litObj->setPosition(glm::vec3(0.0f, 0.0f, 5.0f));

	totObj = renderer->addOddObject(RenderObjectData(), materialShader, totCube, totAttributeSizes);

	totObj->setPosition(glm::vec3(3.0f));
	totObj->setScale(glm::vec3(2.0f));


	screw = renderer->addModel(RenderObjectData(), materialShader, "./Assets/Models/test/vida_obj.obj");


	room = renderer->addModel(RenderObjectData(), materialShader, "./Assets/Models/room/room.obj");


	std::vector<glm::vec3> cubePositions = {
		glm::vec3(1.0f, 1.0f, 1.0f),
		glm::vec3(-3.8f, -2.0f, -12.3f),
		glm::vec3(2.4f, -0.4f, -3.5f),
		glm::vec3(-1.7f,  3.0f, -7.5f),
		glm::vec3(1.3f, -2.0f, -2.5f),
		glm::vec3(1.5f,  2.0f, -2.5f),
		glm::vec3(1.5f,  0.2f, -1.5f),
		glm::vec3(-1.3f,  1.0f, -1.5f)
	};

	odd = renderer->addOddObject(RenderObjectData(), defShader, vertices, attributeSizes);

	odd->setPosition(cubePositions[0]);

	std::vector<RenderObject *> objects;
	objects.push_back(odd);
	for(int i = 1; i < cubePositions.size(); i++)
	{
		glm::vec3 pos = cubePositions[i];
		SimpleRenderObject *ro = renderer->cloneObject(*odd);

		ro->setPosition(pos);
		objects.push_back(ro);
	}

}

void update()
{
	float sinTime = glm::sin(currentFrame);

	glm::vec3 lightOffset = glm::vec3(0.0f, glm::cos(currentFrame) * 5, sinTime * 5);


	pointLight->setPosition(pointLightPos + lightOffset);
	pointLightObj->setPosition(pointLightPos + lightOffset);
	//litObj->setRotation(glm::vec3(0, glm::radians(180 * glm::cos(0.3 * currentFrame)), 0.0f));
	//litObj->setPosition(glm::vec3(0.0f, 0.0f, glm::sin(currentFrame * 0.3f) * 5.0));


	spotLight->setYaw(-glm::sin(currentFrame / 1.5f) * 180.0f);
	spotLightObj->setRotation(glm::vec3(glm::sin(currentFrame / 1.5f) * glm::pi<float>(), 0.0f, 0.0f));


	for(int i = 0; i < oddClones.size(); i++)
	{
		glm::vec3 scale = glm::vec3(1.0f);
		RenderObject *ro = oddClones[i];

		if(i % 4 == 0)
		{
			sinTime = glm::sin(currentFrame * 2);
		}
		else if(i % 2 == 0)
		{

			scale.y = 2.0f + sinTime;
			scale.x = 2.0f - sinTime;
			sinTime = 0;
		}
		if(i % 3 == 0)
		{
			sinTime *= -1.0;
		}
		ro->setRotation(glm::vec3(0.0f, 0.0, glm::radians(sinTime * 180)));
		ro->setPosition(ro->getPosition() + glm::vec3(0.0f, sinTime * deltaTime, 0.0));
		scale.x += sinTime * 0.5f;
		ro->setScale(scale);
	}
}

void renderLoop()
{
	while(!glfwWindowShouldClose(window))
	{
		//input
		currentFrame = (float)glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		processInput(window);

		update();

		renderer->draw();

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	end();
}

void end()
{
	delete defShader;
	delete lightingShader;
	delete lightShader;
	delete litShader;
	delete materialShader;
	delete testShader;
	delete wallTex;
	delete smileTex;
	delete renderer;
	glfwTerminate();
}




void processInput(GLFWwindow *window)
{
	if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	if(glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
	{
		texOffsety -= 0.05f;
	}
	if(glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
	{
		texOffsety += 0.05f;
	}
	if(glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
	{
		texOffsetx -= 0.05f;
	}
	if(glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
	{
		texOffsetx += 0.05f;
	}

	const float cameraSpeed = 2.5f * deltaTime;
	if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		cameraPos += cameraSpeed * camera->getCameraDir();
	if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		cameraPos -= cameraSpeed * camera->getCameraDir();
	if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		cameraPos -= camera->getCameraRight() * cameraSpeed;
	if(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		cameraPos += camera->getCameraRight() * cameraSpeed;
	if(glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
		cameraPos += glm::vec3(0.0f, cameraSpeed, 0.0f);
	if(glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
		cameraPos += glm::vec3(0.0f, -cameraSpeed, 0.0f);

	camera->setCameraPos(cameraPos);

}

void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow *window, double xpos, double ypos)
{
	if(firstMouse) // initially set to true
	{
		lastX = (float)xpos;
		lastY = (float)ypos;
		firstMouse = false;
	}
	float xoffset = (float)xpos - lastX;
	float yoffset = lastY - (float)ypos; // reversed since y-coordinates range from bottom to top
	lastX = (float)xpos;
	lastY = (float)ypos;

	const float sensitivity = 0.1f;
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	camera->setYaw(camera->getYaw() + xoffset);
	camera->setPitch(camera->getPitch() + yoffset);
}

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset)
{
	camera->setFOV(camera->getFOV() - (float)yoffset);
}