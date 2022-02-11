#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


#include <iostream>

#include <GLTypes/Shader.h>
#include <GLTypes/Texture.h>
#include <GLTypes/Renderer.h>
#include <GLTypes/Camera.h>

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void processInput(GLFWwindow *window);
void mouse_callback(GLFWwindow *window, double xpos, double ypos);
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
float texOffsetx = 0.0f, texOffsety = 0.0f;

float deltaTime = 0.0f;	// Time between current frame and last frame
float lastFrame = 0.0f; // Time of last frame

FreeCamera *camera;

glm::vec3 cameraPos(0.0f, 0.0f, 3.0f);

float yaw = -90.0f, pitch = 0.0f;
float lastX = 400, lastY = 300;
bool firstMouse = true;
float fov = 45.0f;

int main()
{


	/*---------------------------------------------------*/
	//Init glfw
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow *window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);

	if(window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	/*---------------------------------------------------*/

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

	std::vector<uint32_t> indices = {
		0, 1, 2,
		2,3,0
	};

	std::vector<uint32_t> attributeSizes = {3,2};
	std::vector<uint32_t> lightAttributeSizes = {3};
	std::vector<uint32_t> normalAttributeSizes = {3,3};

	Renderer *renderer = new Renderer(RendererData());

	camera = renderer->camera;

	camera->setCameraPos(glm::vec3(0.0f, 0.0f, 3.0f));

	/*---------------------------------------------------*/
	//textures

	Texture2D *textures[2] = {new Texture2D("./Assets/wall.jpg", false, TextureType::RGB, TextureType::RGB),new Texture2D("./Assets/awesomeface.png", true, TextureType::RGBA, TextureType::RGBA)};
	/*---------------------------------------------------*/
	//shaders
	// 
	Shader *shader = new Shader("./Assets/Shaders/def_vert.glsl", "./Assets/Shaders/def_frag.glsl");
	shader->addTexture("texture1", textures[0]);
	shader->addTexture("texture2", textures[1]);

	Shader *lightingShader = new Shader("./Assets/Shaders/light_vert.glsl", "./Assets/Shaders/light_frag.glsl");

	Shader *litShader = new Shader("./Assets/Shaders/lit_vert.glsl", "./Assets/Shaders/lit_frag.glsl");

	renderer->lightManager->litShaders.push_back(litShader);

	/*---------------------------------------------------*/



	RenderObject *lightObj = renderer->addOddObject(RenderObjectData(), lightingShader, lightVertices, lightAttributeSizes);
	renderer->lightManager->getLightType<PointLight>("pointLight", 5);
	PointLight *light = renderer->lightManager->addLight<PointLight>("pointLight");
	light->setup(glm::vec3(0.1f));

	RenderObject *litObj = renderer->addOddObject(RenderObjectData(), litShader, normalCube, normalAttributeSizes);

	RenderObject *odd = renderer->addOddObject(RenderObjectData(), shader, vertices, attributeSizes);

	glm::vec3 lightPos = glm::vec3(1.0f, 2.0f, -1.0f);

	light->setPosition(lightPos);
	lightObj->setPosition(lightPos);
	litObj->setPosition(glm::vec3());

	glm::vec3 objectColor = glm::vec3(1.0f, 0.5f, 0.31f);
	glm::vec3 lightColor = glm::vec3(1.0f);


	litObj->shader->setShaderUniform("material.ambient", glm::vec3(1.0f, 0.5f, 0.31f));
	litObj->shader->setShaderUniform("material.diffuse", glm::vec3(1.0f, 0.5f, 0.31f));
	litObj->shader->setShaderUniform("material.specular", glm::vec3(0.5f));
	litObj->shader->setShaderUniform("material.shininess", glm::vec1(32.0f));



	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	//glLineWidth(3.0f);

	//glPointSize(10.0f);

	std::vector<glm::vec3> cubePositions = {
		glm::vec3(2.0f,  5.0f, -15.0f),
		glm::vec3(-3.8f, -2.0f, -12.3f),
		glm::vec3(2.4f, -0.4f, -3.5f),
		glm::vec3(-1.7f,  3.0f, -7.5f),
		glm::vec3(1.3f, -2.0f, -2.5f),
		glm::vec3(1.5f,  2.0f, -2.5f),
		glm::vec3(1.5f,  0.2f, -1.5f),
		glm::vec3(-1.3f,  1.0f, -1.5f)
	};

	odd->setPosition(cubePositions[0]);

	std::vector<RenderObject *> objects;
	objects.push_back(odd);

	for(int i = 1; i < cubePositions.size(); i++)
	{
		glm::vec3 pos = cubePositions[i];
		RenderObject *ro = renderer->cloneObject(odd->rObjectID);

		ro->setPosition(pos);
		objects.push_back(ro);
	}

	while(!glfwWindowShouldClose(window))
	{
		//input
		float currentFrame = (float)glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		processInput(window);

		//change

		float sinTime = glm::sin(currentFrame);

		glm::vec3 lightOffset = glm::vec3(0.0f, glm::cos(currentFrame) * 3, sinTime * 3);


		light->setPosition(lightPos + lightOffset);
		lightObj->setPosition(lightPos + lightOffset);
		litObj->setRotation(glm::vec3(0, glm::radians(180 * glm::cos(0.3 * currentFrame)), 0.0f));
		litObj->setPosition(glm::vec3(glm::sin(currentFrame * 0.3f) * 5.0, 0.0f, 0.0f));

		for(int i = 0; i < objects.size(); i++)
		{
			glm::vec3 scale = glm::vec3(1.0f);
			RenderObject *ro = objects[i];

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

		//draw

		renderer->draw();

		//swap buffer
		glfwSwapBuffers(window);
		glfwPollEvents();
	}


	delete shader;
	delete textures[0];
	delete textures[1];
	delete renderer;
	glfwTerminate();
	return 0;
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